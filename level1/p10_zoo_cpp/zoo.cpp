#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <stdexcept>
#include <windows.h>
#include <random>
using namespace std;

const string A = R"(animal\)";
const string E = R"(.mav)";

// 要加动物就构造一个 //
class animal {
public:
    const string name_, sound_;

    animal(const string& name, const string& sound) noexcept : name_(name), sound_(sound) {};
    animal(const animal& a) noexcept : name_(a.name_), sound_(a.sound_) {};

    const void call() const noexcept {
        PlaySoundA((A + name_ + E).c_str(), NULL, SND_FILENAME | SND_NODEFAULT);
        // 文件格式自己看, 不一样就滚 //

        cout << sound_ << '\n';
    }
};

// 装动物的动物园 //
class zoo {
private:
    vector<animal*> p_;
public:
    zoo() {};
    zoo(const zoo&) = delete;
    zoo& operator=(const zoo&) = delete;

    ~zoo() {
        for (size_t i=0; i<p_.size(); ++i) { delete p_[i]; }
        p_.clear();
    }

    void add(const animal& a) {
        p_.push_back(new animal(a));
    }
    void remove(const long long& pos) {
        if (pos < 0 || pos >= p_.size()) return; // 懒得写error
        delete p_[pos];
        p_.erase(p_.begin() + pos);
    }

    void sort() {
        std::sort(p_.begin(), p_.end(), [](const animal* a, const animal* b){ return a->name_ < b->name_; });
    }
    void shuffle() {
        random_device RD;
        mt19937 gen(RD());
        for (size_t i=0; i < p_.size(); ++i) {
            uniform_int_distribution<int> dist(i, p_.size() - 1);
            std::swap( p_[i], p_[dist(gen)] );
        }
    }

    const void call() const noexcept {
        for (auto a : p_) { a->call(); }
    }
    const void call(const long long& pos) const noexcept {
        if (pos < 0 || pos >= p_.size()) return; // 懒得写error
        p_[pos]->call();
    }
};

int main() {
    SetConsoleOutputCP(65001); // UTF-8, 保证中文不乱码
    SetConsoleCP(65001);
    system("cls");

    zoo Z_7;
    animal cat("cat", "neow 喵");
    animal dog("dog", "bark 汪");
    Z_7.add(cat);
    Z_7.add(dog);

    Z_7.call();

    animal bird("bird", "chirp 啾啾啾");
    Z_7.add(bird);
    Z_7.call(2); // 从零开始的下标

    animal wolfdog("wolfdog", "howl 嗷呜");
    Z_7.add(wolfdog);
    Z_7.remove(1); // 狼狗把狗赶出去了
    Z_7.call();
    
    return 0;
}
