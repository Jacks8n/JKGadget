#include "reflitest.h"

struct refl_sample {
    META_BEGIN(refl_sample)

    META(field_int, 42, std::string_view("the answer"))
    int field_int;

    META(field_float, 2.71828, std::string_view("natural constant"))
    float field_float;

    META_END
};

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
}