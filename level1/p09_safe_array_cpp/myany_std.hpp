#include <typeinfo>
#include <utility>
#include <type_traits>
#include <stdexcept>

//══════ 规范版 myany ══════
// 存储层用「抽象基类 + 每类型一个子类」(和 libstdc++ 的 std::any 同思路),
// 好处: 虚函数表可扩展、类型加新功能不用改 any 本体

class bad_any_cast : public std::exception {          //★ 继承 std::exception
public:
    const char* what () const noexcept override { return "bad_any_cast: 类型不对"; }
};

class any {                                          //════ 盒子 ════
    //──── 存储层: 抽象接口, 只认 void* + typeid ────
    struct Store {
        virtual ~Store () = default;
        virtual Store*    clone () const = 0;                    // 复制
        virtual const std::type_info& type () const noexcept = 0; // 什么类型
        virtual void*       addr ()       noexcept = 0;         // 取地址
        virtual const void* addr () const noexcept = 0;
    };
    //──── 每种类型的具体存储 ────
    template<class T>
    struct StoreOf : Store {
        T val;
        //★ 变参透传: 把参数原样交给 val 的构造
        //  一条同时覆盖 拷贝/移动/默认/任意多参 构造 (等价 std::any 的 _S_create)
        template<class... A>
        StoreOf (A&&... a) : val(std::forward<A>(a)...) {}
        Store*    clone () const override { return new StoreOf (val); }
        const std::type_info& type () const noexcept override { return typeid(T); }
        void*       addr ()       noexcept override { return &val; }
        const void* addr () const noexcept override { return &val; }
    };

    Store* store_ = nullptr;                           // 存活的 Store, 空盒子为 nullptr

    template<class T, class... A> Store* make (A&&... a) {   // 造一个新 Store
        return new StoreOf<T> (std::forward<A>(a)...);
    }

public:
    //════ 构造 ════
    any () = default;                                  // 空盒子
    any (const any& o) : store_(o.store_ ? o.store_->clone() : nullptr) {}
    any (any&& o)      noexcept : store_(o.store_) { o.store_ = nullptr; }   //★ 移动: 只搬指针
    //★ 存值: 存 decay 后的类型(剥掉顶层 const/volatile, 拆引用与数组)
    template<class T> any (T v) : store_(make<std::decay_t<T>>(std::move(v))) {}
    template<class T> any (std::initializer_list<T>) = delete;              // 防歧义

    //════ 赋值 ════
    any& operator= (const any& o) {
        if (this != &o) { any t(o); swap(*this, t); }  //★ 复制构造 + swap
        return *this;
    }
    any& operator= (any&& o) noexcept {                //★ 移动赋值
        if (this != &o) { reset(); store_ = o.store_; o.store_ = nullptr; }
        return *this;
    }
    template<class T> any& operator= (T v) { emplace<std::decay_t<T>>(std::move(v)); return *this; }

    //════ 析构 ════
    ~any () { reset(); }

    //════ 装填 ════
    //★ emplace: 存的是 decay 后的 D, 返回的也是 D&
    //  (对齐 std::any 的 __emplace_t<decay_t<_Tp>>; 这样 emplace<int&> 存的是副本 int)
    //  ★与 any_cast 的区别: 那个按调用者写的 T 返回, 写 const 才拿到只读引用
    template<class T, class... A>
    std::decay_t<T>& emplace (A&&... a) {
        using D = std::decay_t<T>;
        static_assert (!std::is_same_v<D, any>, "any 不能装 any 自身");
        reset();
        store_ = make<D> (std::forward<A>(a)...);
        return *(D*)store_->addr();
    }

    //════ 查询 ════
    bool has_value () const noexcept { return store_ != nullptr; }
    void reset () noexcept { delete store_; store_ = nullptr; }

    const std::type_info& type () const {              //★ 不能 noexcept: 空盒子要抛
        if (!store_) throw bad_any_cast();
        return store_->type();
    }

    friend void swap (any& a, any& b) noexcept {      //★ 自由 swap, ADL 友好
        any t(std::move(a)); a = std::move(b); b = std::move(t);
    }

    //════ 取地址(供 any_cast 用) ════
    void*       raw ()       noexcept       { return store_ ? store_->addr()       : nullptr; }
    const void* raw () const noexcept       { return store_ ? store_->addr () : nullptr; }
    const std::type_info* ti () const noexcept { return store_ ? &store_->type() : nullptr; }

    //════ 判等 ════
    friend bool operator== (const any& a, const any& b) {
        if (a.store_ == b.store_) return true;         // 同一个
        if (!a.has_value() || !b.has_value()) return false;
        return a.type() == b.type();                 //★ C++20: type_info 的 == 
    }
    friend bool operator!= (const any& a, const any& b) { return !(a == b); }
};

//════ 取值: 类型不对抛异常 ════
template<class T>
T& any_cast (any& a) {
    using D = std::decay_t<T>;                       //★ 只在比对时 decay
    if (!a.has_value() || a.ti() != &typeid(D)) throw bad_any_cast();
    return *(T*)a.raw();
}
template<class T>
const T& any_cast (const any& a) {
    using D = std::decay_t<T>;
    if (!a.has_value() || a.ti() != &typeid(D)) throw bad_any_cast();
    return *(const T*)a.raw();
}
//════ 取值: 类型不对返回 nullptr ════
template<class T>
T* any_cast (any* a) {
    using D = std::decay_t<T>;
    if (!a || a->ti() != &typeid(D)) return nullptr;
    return (T*)a->raw();
}
template<class T>
const T* any_cast (const any* a) {
    using D = std::decay_t<T>;
    if (!a || a->ti() != &typeid(D)) return nullptr;
    return (const T*)a->raw();
}

//★ 标准签名: T 从参数推导, 不是从显式模板参数
template<class T> any make_any (T&& v) { return any (std::forward<T>(v)); }
