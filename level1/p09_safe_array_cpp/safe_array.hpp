#pragma once // 头文件包含保护

#include <iostream>
#include <string>
#include <utility>
#include <compare>
#include <typeinfo>
#include <stdexcept>

// 错误信息 //
class my_error : public std::exception { // 继承 std::exception
private:
    std::string msg_;
public:
    explicit my_error(const std::string& msg) : msg_(msg) {}
    virtual ~my_error() noexcept = default;
    const char* what() const noexcept override { // 不能返回引用
        return msg_.c_str();
    } // 签名不一致就偷偷生成新函数
};

class my_bad_cast : public my_error { // 类型取错
public:
    explicit my_bad_cast(const std::string& words, const std::string& type)
        : my_error(words + type) {}
}; // 初始化写 正确的类型名

class my_out_of_range : public my_error { // 下标越界
public:
    explicit my_out_of_range(const std::string& words, const size_t& size)
        : my_error(words + "我的大小: " + std::to_string(size)) {}
}; // 初始化写 正确的大小

// my_any //
class my_any {
private:
    // 抽象接口 //
    class Base {
    public:
        virtual ~Base() noexcept = default;
        virtual Base* clone() const = 0; // 复制一份
        virtual const std::type_info& type() const = 0; // 取类型
        virtual       void* addr()       noexcept = 0;
        virtual const void* addr() const noexcept = 0; // 取地址

        virtual bool operator == (const Base& other) const = 0;
        virtual std::partial_ordering operator <=> (const Base& other) const = 0;

        virtual std::ostream& operator << (std::ostream& os) const = 0;
    };
    
    // 具体存储 //
    template<class T> class Holder : public Base {
    private:
        T val_; // 不需要手动释放空间
    public:
        template<class... A> Holder(A&&... a) : val_(std::forward<A>(a)...) {} // 多种构造
        Holder* clone() const override { return new Holder(val_); }
        const std::type_info& type() const noexcept override { return typeid(T); }
              void* addr()       noexcept override { return &val_; }
        const void* addr() const noexcept override { return &val_; }

        bool operator == (const Base& other) const override {
            if (type() != other.type()) { return 0; }
            if constexpr (requires { val_ == val_; } ) {
                return val_ == static_cast<const Holder&>(other).val_;
            }
            return 0;
        }
        std::partial_ordering operator <=> (const Base& other) const override {
            if (type() != other.type()) { return std::partial_ordering::unordered; }
            if constexpr (requires { val_ <=> val_; } ) {
                return val_ <=> static_cast<const Holder&>(other).val_;
            } else { return std::partial_ordering::unordered; }
        }

        std::ostream& operator << (std::ostream& os) const override {
            if constexpr (requires { os << val_; } ) { os << val_; }
            else { throw my_error(std::string(type().name()) + " 不支持 << 输出"); }
            return os;
        }
    };

    Base* p_ = nullptr; // 实际指向Holder

    template<class T, class... A> Base* make(A&&... a) {
        return new Holder<T>(std::forward<A>(a)...); // 造一个Base*
    }
    
public:
    // 构造 //
    my_any(                   ) noexcept = default;                                    // 空
    my_any(const my_any& other)          : p_(other.p_? other.p_->clone() : nullptr) {}// 拷贝
    my_any(     my_any&& other) noexcept : p_(other.p_) { other.p_ = nullptr; }        // 移动
    template<class T> my_any(T val) : p_(make<std::decay_t<T> >(std::move(val))) {}; // T自动匹配左右值(不能多参)
    template<class T> my_any(std::initializer_list<T>) = delete; // 防止传入多参时隐式转换到这个, 可能出事
    
    template<class T, class... A> std::decay_t<T>& emplace(A&&... val) { // 多参构造, 直接返回引用, 方便外面操作
        using TT = std::decay_t<T>;
        static_assert(!std::is_same_v<TT, my_any>, "把any存到any很好玩吗?"); // 万一嵌套出事
        clear();
        p_ = make<TT>(std::forward<A>(val)...);
        return *static_cast<TT*>(p_->addr());
    }

    // 清空 //
    ~my_any()    noexcept { delete p_; p_=nullptr; }
    void clear() noexcept { delete p_; p_=nullptr; }

    // 空否 //
    bool has_value() const noexcept { return p_ != nullptr; }

    // 交换 //
    friend void swap(my_any& a, my_any& b) noexcept { std::swap(a.p_, b.p_); } // 就只有指针

    // 类型 //
    const std::type_info& type() const {
        if(p_ == nullptr) { throw my_bad_cast("这是空的! ", "没有类型"); }
        return p_->type();
    }

    // 取值 //
    template<class T> friend const T& cast(const my_any& any) {
        if (typeid(T) != any.type()) { throw my_bad_cast("我的类型: ", any.type().name()); } // type()里判空了
        return *static_cast<const T*>(any.p_->addr());
    }
    template<class T> friend       T& cast (     my_any& any) {
        return const_cast<T&>( cast<T>( static_cast<const my_any&>(any) ) );
    }

    template<class T> friend const T* cast(const my_any* any) {
        if (!any    ) { return nullptr; }
        if (!any->p_) { return nullptr; }
        if (typeid(T) != any->p_->type()) { return nullptr; }
        return static_cast<const T*>(any->p_->addr());
    }
    template<class T> friend       T* cast(      my_any* any) {
        return const_cast<T*>( cast<T>( static_cast<const my_any*>(any) ) );
    }
    template<class T> operator T() const { return cast<T>(*this); } // 类型转换, 在这相当于取值, 还有我把隐式转换开了

    // 赋值 //
    my_any& operator = (const my_any& other)          { // 拷贝
        if (this != &other) {
            my_any tmp(other); // tmp离开后调~my_any()
            swap(*this, tmp); // 把原本的指针丢给tmp析构
        }
        return *this;
    }

    my_any& operator = (     my_any&& other) noexcept { // 移动
        if (this != &other) { swap(*this, other); } // 同上
        return *this;
    }

    template<class T> my_any& operator = (T val) { // 多种赋值(除了多参)
        emplace<std::decay_t<T>>(std::move(val));
        return *this;
    }

    // 判等 //
    friend bool operator == (const my_any& a, const my_any& b) {
        if ( a.p_ ==  b.p_) { return 1; }
        if (!a.p_ || !b.p_) { return 0; }
        return *a.p_ == *b.p_; // 因为这里拿不到类型
    }
    friend bool operator != (const my_any& a, const my_any& b) { return !(a==b); }

    // 比较 //
    friend std::partial_ordering operator <=> (const my_any& a, const my_any& b) {
        if (a.p_ == b.p_) { return std::partial_ordering::equivalent; }
        if (!a.p_) { return std::partial_ordering::less;    }
        if (!b.p_) { return std::partial_ordering::greater; }
        return *a.p_ <=> *b.p_; // 因为这里拿不到类型
    }

    // 输出 //
    friend std::ostream& operator << (std::ostream& os, const my_any& a) {
        if (!a.p_) { return os; }
        return *a.p_ << os; // 为什么要求成员函数的左操作数是本类对象
    }

    // 很多标准函数用这些就能带替, 只是麻烦点
    // 更多运算符不想加了, 老老实实cast吧
    // 没写小对象优化
};
template<class T, class... A> my_any make_my_any(A&&... a) { // 构造临时
    my_any tmp;
    tmp.emplace<T>(std::forward<A>(a)...);
    return tmp;
} // 呜呜呜, 放不进去

// my_vector //
template<class T> class my_vector {
private:
    T* buf_ = nullptr;
    size_t n_ = 0, cap_ = 0;
    void new_move(const size_t& size) { // 外面自己判断越界等等
        if (n_>size) { n_ = size; }
        T* tmp = new T[size];
        std::swap(buf_, tmp);
        for (size_t i=0; i<n_; ++i) { buf_[i] = std::move(tmp[i]); }
        delete[] tmp;
        cap_ = size;
    }
    void grow() { new_move(cap_? cap_<<1 : 7); }

public:
    // 构造 //
    my_vector(                      ) noexcept = default;
    my_vector(const my_vector& other)          : n_(other.n_), cap_(other.cap_) { // 拷贝
        if(!cap_) { return; }
        buf_ = new T[cap_];
        for (size_t i=0; i<n_; ++i) { buf_[i] = other.buf_[i]; }
    }
    my_vector(     my_vector&& other) noexcept : n_(other.n_), cap_(other.cap_) { // 移动
        buf_ = std::move(other.buf_);
        other.buf_ = nullptr;
        other.n_ = other.cap_ = 0; // 防止你作死传变量右值
    }
    my_vector(std::initializer_list<T> il) : cap_(il.size()) { // {} 构造
        if (!cap_) { return; }
        buf_ = new T[cap_];
        for (const T& x : il) { buf_[n_++] = x; } // 这个语法真是太好用了
    }

    // 清空 //
    ~my_vector() noexcept { delete[] buf_; buf_ = nullptr; }
    void clear() noexcept {
        delete[] buf_;
        buf_ = nullptr;
        n_ = cap_ = 0;
    }

    // 空否 //
    bool empty() const noexcept { return n_ == 0; }

    // 交换 //
    friend void swap(my_vector& a, my_vector& b) noexcept {
        std::swap(a.buf_, b.buf_);
        std::swap(a.n_  , b.n_  );
        std::swap(a.cap_, b.cap_);
    }

    // 大小 //
    size_t size() const noexcept { return n_; }
    void resize(const long long& size) { resize(size, T{}); }
    void resize(const long long& size, const T& val) {
        if (size<0) { throw my_error("改大小也不能为负啊"); }
        new_move(size); // 标准不改cap_
        while (n_<size) { buf_[n_++] = val; }
        n_ = size;
    }

    size_t capacity() const noexcept { return cap_; }
    void reserve(const long long& size) {
        if (size<0) { throw my_error("改大小也不能为负啊"); }
        if (size > cap_) { new_move(size); }
        if (size <=  n_) { new_move(n_  ); } // 标准 <n_ 不干
    }

    // 插入 //
    void in() { in(T{}); }
    void in(const T& val) {
        if (n_ == cap_) { grow(); }
        buf_[n_++] = val;
    }
    void in(     T&& val) {
        if (n_ == cap_) { grow(); }
        buf_[n_++] = std::move(val);
    }
    void in(std::initializer_list<T> il) {
        if (il.size() == 0) { return; } // 虽然放过去没事
        if (n_ + il.size() > cap_) { new_move(n_ + il.size()); }
        for (size_t i=n_; const T& x : il) { buf_[i++] = x ; }
        n_ += il.size();
    }
    void in(const long long& pos, const T& val) {
        if (pos<0 || pos>n_) { throw my_out_of_range("往哪插呢? ", n_); }
        if (n_ == cap_) { grow(); }
        for (size_t i=n_++; i>pos; --i) { buf_[i] = std::move(buf_[i-1]); }
        buf_[pos] = val;
    }
    void in(const long long& pos,      T&& val) {
        if (pos<0 || pos>n_) { throw my_out_of_range("往哪插呢? ", n_); } // =n 不越界
        if (n_ == cap_) { grow(); }
        for (size_t i=n_++; i>pos; --i) { buf_[i] = std::move(buf_[i-1]); }
        buf_[pos] = std::move(val);
    }
    void in(const long long& pos, const long long& count, const T& val) { // 反正拷贝这么多次就不写移动了
        if (pos<0 || pos>n_) { throw my_out_of_range("往哪插呢? ", n_); }
        if (count<=0) { return; } // 我觉得还是静默好
        if (n_+count > cap_) { new_move(n_ + count); }
        n_ += count;
        for (size_t i=n_-1; i >= pos+count; --i) { buf_[i] = std::move(buf_[i - count]); } // pos+count >0
        for (size_t i=pos; i < pos + count; ++i) { buf_[i] = val ; }
    }
    void in(const long long& pos, std::initializer_list<T> il) {
        if (pos<0 || pos>n_) { throw my_out_of_range("往哪插呢? ", n_); }
        if (il.size() == 0) { return; } // 我觉得还是静默好
        if (n_ + il.size() > cap_) { new_move(n_ + il.size()); }
        n_ += il.size();
        for (size_t i=n_-1; i >= pos + il.size(); --i) { buf_[i] = std::move(buf_[i - il.size()]); }
        for (size_t i=pos; const T& x : il) { buf_[i++] = x ; }
    }
    template<class It, class = std::enable_if_t< !std::is_integral_v<It> > > // 防止整形匹配进来(浮点会但是报错)
    void in (const long long& pos, It begin, It end) { // end是末尾+1
        if (pos<0 || pos>n_) { throw my_out_of_range("往哪插呢? ", n_); }
        if (end <= begin) { return; } // 我觉得还是静默好
        const size_t place = end - begin;
        if (n_+place > cap_) { new_move(n_ + place); }
        n_ += place;
        for (size_t i=n_-1; i >= pos+place; --i) { buf_[i] = std::move(buf_[i - place]); }
        for (It i=begin; i<end; ++i) { buf_[i - begin + pos] = *i ; }
    }

    // 清除 //
    void out() { // 末尾
        if (!n_) { throw my_error("空的你还删"); }
        buf_[--n_] = T{};
    }
    void out(const long long& pos) { // 位置
        if (pos<0 || pos>=n_) { throw my_out_of_range("想删哪呢? ", n_); }
        --n_;
        for (size_t i=pos; i<n_; ++i) { buf_[i] = std::move(buf_[i+1]); }
        buf_[n_] = T{};
    }
    void out(const long long& left, const long long& right) { // 区间
        if (left<0 || right>=n_) { throw my_out_of_range("想删哪呢? ", n_); }
        if (right < left) { return; } // 我觉得还是静默好
        const size_t empty = right - left +1;
        n_ -= empty;
        for (size_t i=left; i<n_; ++i) { buf_[i] = std::move(buf_[i + empty]); }
        for (size_t i=0; i<empty; ++i) { buf_[n_ + i] = T{} ; }
    }

    // 指针 //
    T*       begin()       noexcept { return buf_;      }
    const T* begin() const noexcept { return buf_;      }
    T*         end()       noexcept { return buf_ + n_; }
    const T*   end() const noexcept { return buf_ + n_; }

    // 下标 //
    T& operator [] (const long long& pos) { // 不写at()了
        if (pos<0 || pos>=n_) { throw my_out_of_range("下标越界 ", n_); }
        return buf_[pos];
    }
    const T& operator [] (const long long& pos) const {
        if (pos<0 || pos>=n_) { throw my_out_of_range("下标越界 ", n_); }
        return buf_[pos];
    }

    // 赋值 //
    my_vector& operator = (const my_vector& other)          {
        if (this != &other) {
            my_vector tmp(other);
            swap(*this, tmp); // 同my_any
        }
        return *this;
    }
    my_vector& operator = (     my_vector&& other) noexcept {
        if (this != &other) { swap(*this, other); } // 我去, 你传变量右值有惊喜
        return *this;
    }

    // 判等 //
    friend bool operator == (const my_vector& a, const my_vector& b) {
        if (a.n_ != b.n_) { return 0; }
        const size_t mn = (a.n_ < b.n_)? a.n_ : b.n_;
        if (!mn) { return a.n_ == b.n_; }
        if constexpr (requires { a[0] == b[0]; } ) { // 尝试有没有比较
            for (size_t i=0; i<mn; ++i) { if (a[i] != b[i]) return 0; }
        } else { return 0; }
        return 1;
    }
    friend bool operator != (const my_vector& a, const my_vector& b) { return !(a==b); }
    
    // 比较 //
    friend std::partial_ordering operator <=> (const my_vector& a, const my_vector& b) {
        const size_t mn = (a.n_ < b.n_)? a.n_ : b.n_;
        if (!mn) { return a.n_ <=> b.n_; }
        if constexpr (requires { a[0] <=> b[0]; } ) { // 尝试有没有比较
            for (size_t i=0; i<mn; ++i) {
                if (auto o = (a[i] <=> b[i]); o!=0) { return o; } // 这里隐式转换了
            }
        } else { return std::partial_ordering::unordered; }
        return a.n_ <=> b.n_; // 这里隐式转换了
    }

    // 输出 //
    friend std::ostream& operator << (std::ostream& os, const my_vector& v) {
        os << "{ ";
        if(v.n_) { os << v.buf_[0]; }
        for (size_t i=1; i<v.n_; ++i) { os << ", " << v.buf_[i]; }
        os << " }";
        return os;
    }

    // 下标,大小传入都不能用unsigned
    // 一堆无符号比较已经拦了 <0 的情况, 但是没转类型
    // 定义时实例化天然挡不同类型操作
    // out内存释放晚
    // 很多标准函数用这些就能带替, 只是麻烦点
    // 把指针当迭代器应该没问题
};
// 把my_vector套在my_any外面就是safe_array了
// 虽然但是my_vector本身就是传统safe_array


/*  深度学习
构造, 析构, 继承 等的写法自己看
    注: 类继承默认private

有个细节, 进去时去掉(const/volatile/引用/数组), 之后全都不干这个

&变量 产生临时指针, 寿命短, 不能引用

#pragma once 头文件包含保护

protected: 能给派生用, 但外部不能用

explicit 禁止参数不匹配时隐式转换, 不过一般不会发生
    =a 在初始化时可能转成(a)

noexcept 函数不抛异常

函数右const 表示不改类内变量

virtual 虚函数, 子类重写, 调用父类虚函数时实际调用子类

virtual ~MyErr() noexcept = default: "= default" 自动调用对应析构

override 显式, 这是重写父类虚函数的函数

virtual Base* clone() const = 0: "= 0" 纯虚函数, 把该类变成抽象类
    必须全部实例化才能创建实例 (Base a), 但是可以指针

template<class T> 创建模版, 使用 <> 或 变量 告诉编译器类型, 并替换T,
    typename 告诉编译器后面是类型名
    模版变量初始化写 T{}

template<class T> my_any(std::initializer_list<T>) = delete: "= delete" 删掉

..., 具体三类用法(模版参数包, 函数参数包, 包展开)见L52-53等
    写(A,B), 依次传入, 跟在函数后面依次使用函数
    这里用这个可以调用类型的多参构造

forward<T>(a)... 将a的包展开依次保留左右值分发

new delete 配套, new 类型[大小] , delete[] 变量 ([]在数组时用)

写const分支防止意外构造const

using T 定义类型

static_cast<T>(a) 更好的类型转换
    static 是编译期干的

const_cast<T>() 去const

move() 把变量转为右值引用

friend 定义为非成员函数, 实参里得有这个类的对象

decay_t<T> 返回不带(const/volatile/引用/数组)的类型

is_same_v<A,B> 比较类型

SFINAE (Substitution Failure Is Not An Error) 替换失败不是错误.  用于模版类型替换
    template<class It, class = std::enable_if_t< !std::is_integral_v<It> > >

typeid() 得到不带(const/volatile/引用/数组)的类型信息

运算符重载时
    成员函数 =/[]/()/->/类型转换符 传右, 左用this指针
    非成员 ==/!=/<=>/... 传两边

<=> c++20的, 一次性< > <= >= ,配合一种返回类型
    equal/ less/ greater
    以及比较模式类型
    strong_ordering	    整数、指针、string
    weak_ordering	    忽略大小写的字符串等
    partial_ordering    浮点	            有第四种状态 unordered（不可比）
    std::compare_three_way_result_t<T> 这个返回对应类型
    还有它们也会隐式转换

if(init; bool) 我去, 好用, 作用域直接限制 (c++20)
    for(init; auto x : a) 同理

if constexpr (requires { a <=> b; } ) 尝试有没有比较(里面可替换)
*/
