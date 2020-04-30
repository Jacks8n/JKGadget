#include <iostream>
#include <string>
#include "reflitest.h"

struct refl_sample {
    META_BEGIN(refl_sample)

    META(field_int, 42, std::string_view("the answer"))
    int field_int;

    META(field_float, 2.71828, std::string_view("natural constant"))
    float field_float;

    META_END
};

template <typename T>
T deserialize(std::string_view src) {
    T res;

    T::meta_info_t::foreach_meta([&](auto meta) {
        constexpr auto prop    = meta.template get_nth_attr<1>();
        res.*meta.member_ptr() = std::atoi(src.substr(src.find(prop) + prop.size()).data());
    });

    return res;
}

template <typename T>
std::string serialize(T ins) {
    std::string res;

    T::meta_info_t::foreach_meta([&](auto meta) {
        res.append(meta.template get_nth_attr<1>());
        res.push_back(' ');
        res.append(std::to_string(ins.*meta.member_ptr()));
        res.push_back(' ');
    });

    return res;
}

int main() {
    refl_sample foo { 12, 3.14159 };

    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");

    static_assert(std::is_same_v<GetMemberType(meta0), int>);

    constexpr auto meta0_name = meta0.member_name();
    static_assert(meta0_name == "field_int");

    constexpr int meta0_attr0 = meta0.get_nth_attr<0>();
    static_assert(meta0_attr0 == 42);

    constexpr auto meta0_attr1 = meta0.get_nth_attr<1>();
    static_assert(meta0_attr1 == "the answer");

    constexpr size_t meta_count = refl_sample::meta_info_t::get_meta_count();
    static_assert(meta_count == 2);

    refl_sample::meta_info_t::foreach_meta([](auto meta) constexpr {
        static_assert(std::is_same_v<decltype(meta.template get_nth_attr<1>()), std::string_view>);
    });

    std::string src = "natural constant 12 the answer 1024";
    refl_sample bar = deserialize<refl_sample>(src);
    std::cout << bar.field_int << ' ' << bar.field_float << std::endl;

    std::cout << serialize(foo) << std::endl;
    
    return 0;
}
