#pragma once

#pragma region MISC

#define VA_ARGS_EXPAND(...) __VA_ARGS__

#define VA_ARGS_CONCAT(a, b) a##b
#define VA_ARGS_COMMA ,
#define VA_ARGS_SEMICOLON ;
#define VA_ARGS_LPARENTHESIS (
#define VA_ARGS_RPARENTHESIS )

#pragma endregion

#pragma region INDEXING

#define VA_ARGS_AT1(p0, ...) p0
#define VA_ARGS_AT2(p0, p1, ...) p1
#define VA_ARGS_AT3(p0, p1, p2, ...) p2
#define VA_ARGS_AT4(p0, p1, p2, p3, ...) p3
#define VA_ARGS_AT5(p0, p1, p2, p3, p4, ...) p4
#define VA_ARGS_AT6(p0, p1, p2, p3, p4, p5, ...) p5
#define VA_ARGS_AT7(p0, p1, p2, p3, p4, p5, p6, ...) p6
#define VA_ARGS_AT8(p0, p1, p2, p3, p4, p5, p6, p7, ...) p7
#define VA_ARGS_AT9(p0, p1, p2, p3, p4, p5, p6, p7, p8, ...) p8
#define VA_ARGS_AT10(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, ...) p9
#define VA_ARGS_AT11(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, ...) p10
#define VA_ARGS_AT12(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, ...) p11
#define VA_ARGS_AT13(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     ...)                                                      \
    p12
#define VA_ARGS_AT14(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, ...)                                                 \
    p13
#define VA_ARGS_AT15(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, ...)                                            \
    p14
#define VA_ARGS_AT16(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, ...)                                       \
    p15
#define VA_ARGS_AT17(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, ...)                                  \
    p16
#define VA_ARGS_AT18(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, ...)                             \
    p17
#define VA_ARGS_AT19(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, ...)                        \
    p18
#define VA_ARGS_AT20(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, ...)                   \
    p19
#define VA_ARGS_AT21(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, ...)              \
    p20
#define VA_ARGS_AT22(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, ...)         \
    p21
#define VA_ARGS_AT23(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, ...)    \
    p22
#define VA_ARGS_AT24(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     ...)                                                      \
    p23
#define VA_ARGS_AT25(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, ...)                                                 \
    p24
#define VA_ARGS_AT26(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, ...)                                            \
    p25
#define VA_ARGS_AT27(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, ...)                                       \
    p26
#define VA_ARGS_AT28(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, ...)                                  \
    p27
#define VA_ARGS_AT29(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, p28, ...)                             \
    p28
#define VA_ARGS_AT30(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, p28, p29, ...)                        \
    p29
#define VA_ARGS_AT31(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, p28, p29, p30, ...)                   \
    p30
#define VA_ARGS_AT32(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, p28, p29, p30, p31, ...)              \
    p31
#define VA_ARGS_AT33(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, p28, p29, p30, p31, p32, ...)         \
    p32
#define VA_ARGS_AT34(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,    \
                     p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23,    \
                     p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, ...)    \
    p33

#define MACRO_HELPER_HEAD(...) VA_ARGS_AT1(__VA_ARGS__)

#define VA_ARGS_COUNT(...)                                                     \
    VA_ARGS_AT34(, ##__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22,  \
                 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6,   \
                 5, 4, 3, 2, 1, 0)

#define VA_ARGS_AT_HALF2(p0, p1, ...) p1
#define VA_ARGS_AT_HALF4(p0, p1, p2, p3, ...) p2
#define VA_ARGS_AT_HALF6(p0, p1, p2, p3, p4, p5, ...) p3
#define VA_ARGS_AT_HALF8(p0, p1, p2, p3, p4, p5, p6, p7, ...) p4
#define VA_ARGS_AT_HALF10(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, ...) p5
#define VA_ARGS_AT_HALF12(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          ...)                                                 \
    p6
#define VA_ARGS_AT_HALF14(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, ...)                                       \
    p7
#define VA_ARGS_AT_HALF16(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, ...)                             \
    p8
#define VA_ARGS_AT_HALF18(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, ...)                   \
    p9
#define VA_ARGS_AT_HALF20(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, ...)         \
    p10
#define VA_ARGS_AT_HALF22(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,    \
                          ...)                                                 \
    p11
#define VA_ARGS_AT_HALF24(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,    \
                          p22, p23, ...)                                       \
    p12
#define VA_ARGS_AT_HALF26(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,    \
                          p22, p23, p24, p25, ...)                             \
    p13
#define VA_ARGS_AT_HALF28(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,    \
                          p22, p23, p24, p25, p26, p27, ...)                   \
    p14
#define VA_ARGS_AT_HALF30(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,    \
                          p22, p23, p24, p25, p26, p27, p28, p29, ...)         \
    p15
#define VA_ARGS_AT_HALF32(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,    \
                          p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,    \
                          p22, p23, p24, p25, p26, p27, p28, p29, p30, p31,    \
                          ...)                                                 \
    p16

#define VA_ARGS_AT_HALF_N(n) VA_ARGS_CONCAT(VA_ARGS_AT_HALF, n)
#define MACRO_HELPER_AT_HALF(...)                                              \
    VA_ARGS_AT_HALF_N(VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__))

#pragma endregion

#pragma region TRUNCATE

#define VA_ARGS_RM_HEAD_IMPL(p0, ...) __VA_ARGS__
#define MACRO_HELPER_RM_HEAD(...)                                              \
    VA_ARGS_EXPAND(VA_ARGS_RM_HEAD_IMPL(__VA_ARGS__))

#define VA_ARGS_HEAD_HALF2(p0, p1) p0
#define VA_ARGS_HEAD_HALF4(p0, p1, p2, p3) p0, p1
#define VA_ARGS_HEAD_HALF6(p0, p1, p2, p3, p4, p5) p0, p1, p2
#define VA_ARGS_HEAD_HALF8(p0, p1, p2, p3, p4, p5, p6, p7) p0, p1, p2, p3
#define VA_ARGS_HEAD_HALF10(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)            \
    p0, p1, p2, p3, p4
#define VA_ARGS_HEAD_HALF12(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)  \
    p0, p1, p2, p3, p4, p5
#define VA_ARGS_HEAD_HALF14(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13)                                          \
    p0, p1, p2, p3, p4, p5, p6
#define VA_ARGS_HEAD_HALF16(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15)                                \
    p0, p1, p2, p3, p4, p5, p6, p7
#define VA_ARGS_HEAD_HALF18(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17)                      \
    p0, p1, p2, p3, p4, p5, p6, p7, p8
#define VA_ARGS_HEAD_HALF20(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19)            \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9
#define VA_ARGS_HEAD_HALF22(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21)  \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10
#define VA_ARGS_HEAD_HALF24(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23)                                          \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11
#define VA_ARGS_HEAD_HALF26(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25)                                \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12
#define VA_ARGS_HEAD_HALF28(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25, p26, p27)                      \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13
#define VA_ARGS_HEAD_HALF30(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25, p26, p27, p28, p29)            \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14
#define VA_ARGS_HEAD_HALF32(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25, p26, p27, p28, p29, p30, p31)  \
    p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15

#define VA_ARGS_HEAD_HALF_N(n) VA_ARGS_CONCAT(VA_ARGS_HEAD_HALF, n)
#define MACRO_HELPER_HEAD_HALF(...)                                            \
    VA_ARGS_HEAD_HALF_N(VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__))

#define VA_ARGS_TAIL_HALF2(p0, p1) p1
#define VA_ARGS_TAIL_HALF4(p0, p1, p2, p3) p2, p3
#define VA_ARGS_TAIL_HALF6(p0, p1, p2, p3, p4, p5) p3, p4, p5
#define VA_ARGS_TAIL_HALF8(p0, p1, p2, p3, p4, p5, p6, p7) p4, p5, p6, p7
#define VA_ARGS_TAIL_HALF10(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)            \
    p5, p6, p7, p8, p9
#define VA_ARGS_TAIL_HALF12(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)  \
    p6, p7, p8, p9, p10, p11
#define VA_ARGS_TAIL_HALF14(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13)                                          \
    p7, p8, p9, p10, p11, p12, p13
#define VA_ARGS_TAIL_HALF16(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15)                                \
    p8, p9, p10, p11, p12, p13, p14, p15
#define VA_ARGS_TAIL_HALF18(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17)                      \
    p9, p10, p11, p12, p13, p14, p15, p16, p17
#define VA_ARGS_TAIL_HALF20(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19)            \
    p10, p11, p12, p13, p14, p15, p16, p17, p18, p19
#define VA_ARGS_TAIL_HALF22(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21)  \
    p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21
#define VA_ARGS_TAIL_HALF24(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23)                                          \
    p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23
#define VA_ARGS_TAIL_HALF26(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25)                                \
    p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25
#define VA_ARGS_TAIL_HALF28(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25, p26, p27)                      \
    p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27
#define VA_ARGS_TAIL_HALF30(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25, p26, p27, p28, p29)            \
    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29
#define VA_ARGS_TAIL_HALF32(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,  \
                            p12, p13, p14, p15, p16, p17, p18, p19, p20, p21,  \
                            p22, p23, p24, p25, p26, p27, p28, p29, p30, p31)  \
    p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, \
        p31

#define VA_ARGS_TAIL_HALF_N(n) VA_ARGS_CONCAT(VA_ARGS_TAIL_HALF, n)
#define MACRO_HELPER_TAIL_HALF(...)                                            \
    VA_ARGS_TAIL_HALF_N(VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__))

#pragma endregion

#define MACRO_HELPER_COUNT(...) VA_ARGS_COUNT(__VA_ARGS__)

#define MACRO_HELPER_SELECT(n, ...)                                            \
    VA_ARGS_EXPAND(VA_ARGS_CONCAT(VA_ARGS_AT, n)(__VA_ARGS__))

#pragma region VA_ARGS_TO_INIT_LIST

#define VA_ARGS_INIT_LIST1(name) name(name)
#define VA_ARGS_INIT_LIST2(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST1(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST3(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST2(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST4(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST3(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST5(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST4(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST6(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST5(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST7(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST6(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST8(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST7(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST9(...)                                                \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST8(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST10(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST9(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST11(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST10(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST12(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST11(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST13(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST12(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST14(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST13(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST15(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST14(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST16(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST15(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST17(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST16(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST18(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST17(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST19(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST18(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST20(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST19(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST21(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST20(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST22(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST21(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST23(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST22(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST24(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST23(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST25(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST24(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST26(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST25(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST27(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST26(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST28(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST27(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST29(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST28(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST30(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST29(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST31(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIS30(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_INIT_LIST32(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    (MACRO_HELPER_HEAD(__VA_ARGS__)),                                          \
        VA_ARGS_INIT_LIST31(MACRO_HELPER_RM_HEAD(__VA_ARGS__))

#define VA_ARGS_INIT_LIST_N(n) VA_ARGS_CONCAT(VA_ARGS_INIT_LIST, n)

#define MACRO_HELPER_INIT_LIST(...)                                            \
    VA_ARGS_INIT_LIST_N(VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__))

#pragma endregion

#pragma region BINARY_OP

#define VA_ARGS_BINARY_OP2(op, l, r) l op r
#define VA_ARGS_BINARY_OP4(op, ...)                                            \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_EXPAND(VA_ARGS_AT_HALF4(__VA_ARGS__)),                          \
        VA_ARGS_BINARY_OP2(                                                    \
            op, VA_ARGS_EXPAND(VA_ARGS_AT2(__VA_ARGS__)),                      \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF4(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP6(op, ...)                                            \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF6(__VA_ARGS__),                                          \
        VA_ARGS_BINARY_OP4(                                                    \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF6(__VA_ARGS__)),         \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF6(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP8(op, ...)                                            \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF8(__VA_ARGS__),                                          \
        VA_ARGS_BINARY_OP6(                                                    \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF8(__VA_ARGS__)),         \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF8(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP10(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF10(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP8(                                                    \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF10(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF10(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP12(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF12(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP10(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF12(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF12(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP14(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF14(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP12(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF14(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF14(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP16(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF16(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP14(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF16(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF16(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP18(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF18(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP16(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF18(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF18(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP20(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF20(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP18(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF20(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF20(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP22(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF22(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP20(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF22(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF22(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP24(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF24(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP22(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF24(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF24(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP26(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF26(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP24(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF26(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF26(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP28(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF28(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP26(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF28(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF28(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP30(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF30(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP28(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF30(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF30(__VA_ARGS__)))
#define VA_ARGS_BINARY_OP32(op, ...)                                           \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    op VA_ARGS_AT_HALF32(__VA_ARGS__),                                         \
        VA_ARGS_BINARY_OP30(                                                   \
            op, MACRO_HELPER_RM_HEAD(VA_ARGS_HEAD_HALF32(__VA_ARGS__)),        \
            MACRO_HELPER_RM_HEAD(VA_ARGS_TAIL_HALF32(__VA_ARGS__)))

#define VA_ARGS_BINARY_OP_N(n) VA_ARGS_CONCAT(VA_ARGS_BINARY_OP, n)

#define MACRO_HELPER_BINARY_OP(op, ...)                                        \
    VA_ARGS_EXPAND(                                                            \
        VA_ARGS_BINARY_OP_N(VA_ARGS_COUNT(__VA_ARGS__))(op, __VA_ARGS__))

#pragma endregion

#pragma region ADD_PREFIX

#define VA_ARGS_ADD_PREFIX1(prefix, expr) prefix expr
#define VA_ARGS_ADD_PREFIX2(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX1(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX3(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX2(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX4(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX3(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX5(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX4(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX6(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX5(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX7(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX6(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX8(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX7(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX9(prefix, ...)                                       \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX8(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX10(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX9(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX11(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX10(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX12(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX11(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX13(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX12(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX14(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX13(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX15(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX14(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX16(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX15(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX17(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX16(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX18(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX17(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX19(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX18(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX20(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX18(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX21(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX20(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX22(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX21(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX23(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX22(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX24(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX23(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX25(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX24(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX26(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX25(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX27(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX26(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX28(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX27(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX29(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX28(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX30(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX29(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX31(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX30(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_PREFIX32(prefix, ...)                                      \
    prefix MACRO_HELPER_HEAD(__VA_ARGS__),                                     \
        VA_ARGS_ADD_PREFIX31(prefix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))

#define VA_ARGS_ADD_PREFIX_N(n) VA_ARGS_CONCAT(VA_ARGS_ADD_PREFIX, n)

#define MACRO_HELPER_ADD_PREFIX(prefix, ...)                                   \
    VA_ARGS_ADD_PREFIX_N(VA_ARGS_COUNT(__VA_ARGS__)(prefix, __VA_ARGS__))

#pragma endregion

#pragma region ADD_POSTIX

#define VA_ARGS_ADD_POSTFIX1(postfix, expr) expr postfix
#define VA_ARGS_ADD_POSTFIX2(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX1(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX3(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX2(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX4(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX3(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX5(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX4(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX6(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX5(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX7(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX6(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX8(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX7(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX9(postfix, ...)                                     \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX8(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX10(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX9(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX11(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX10(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX12(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX11(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX13(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX12(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX14(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX13(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX15(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX14(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX16(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX15(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX17(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX16(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX18(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX17(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX19(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX18(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX20(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX18(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX21(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX20(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX22(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX21(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX23(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX22(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX24(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX23(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX25(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX24(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX26(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX25(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX27(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX26(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX28(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX27(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX29(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX28(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX30(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX29(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX31(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX30(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_ADD_POSTFIX32(postfix, ...)                                    \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    postfix, VA_ARGS_ADD_POSTFIX31(postfix, MACRO_HELPER_RM_HEAD(__VA_ARGS__))

#define VA_ARGS_ADD_POSTFIX_N(n) VA_ARGS_CONCAT(VA_ARGS_ADD_POSTFIX, n)

#define MACRO_HELPER_ADD_POSTFIX(postfix, ...)                                 \
    VA_ARGS_ADD_POSTFIX_N(VA_ARGS_COUNT(__VA_ARGS__)(postfix, __VA_ARGS__))

#pragma endregion

#pragma region CONCAT_ALL

#define VA_ARGS_CONCAT_ALL1(expr) expr
#define VA_ARGS_CONCAT_ALL2(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL1(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL3(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL2(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL4(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL3(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL5(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL4(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL6(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL5(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL7(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL6(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL8(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL7(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL9(...)                                               \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL8(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL10(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL9(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL11(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL10(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL12(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL11(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL13(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL12(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL14(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL13(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL15(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL14(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL16(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL15(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL17(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL16(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL18(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL17(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL19(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL18(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL20(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL18(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL21(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL20(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL22(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL21(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL23(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL22(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL24(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL23(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL25(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL24(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL26(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL25(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL27(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL26(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL28(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL27(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL29(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL28(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL30(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL29(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL31(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL30(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL32(...)                                              \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    VA_ARGS_CONCAT_ALL31(MACRO_HELPER_RM_HEAD(__VA_ARGS__))
#define VA_ARGS_CONCAT_ALL_N(n) VA_ARGS_CONCAT(VA_ARGS_CONCAT_ALL, n)

#define MACRO_HELPER_CONCAT_ALL(...)                                           \
    VA_ARGS_CONCAT_ALL_N(VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__))

#pragma endregion

#define MACRO_HELPER_CONNECT(symbol, ...)                                      \
    MACRO_HELPER_HEAD(__VA_ARGS__)                                             \
    MACRO_HELPER_CONCAT_ALL(                                                   \
        MACRO_HELPER_ADD_PREFIX(symbol, MACRO_HELPER_RM_HEAD(__VA_ARGS__)))

#pragma region REPEAT

#define VA_ARGS_REPEAT1(...) __VA_ARGS__
#define VA_ARGS_REPEAT2(...) __VA_ARGS__, VA_ARGS_REPEAT1(__VA_ARGS__)
#define VA_ARGS_REPEAT3(...) __VA_ARGS__, VA_ARGS_REPEAT2(__VA_ARGS__)
#define VA_ARGS_REPEAT4(...) __VA_ARGS__, VA_ARGS_REPEAT3(__VA_ARGS__)
#define VA_ARGS_REPEAT5(...) __VA_ARGS__, VA_ARGS_REPEAT4(__VA_ARGS__)
#define VA_ARGS_REPEAT6(...) __VA_ARGS__, VA_ARGS_REPEAT5(__VA_ARGS__)
#define VA_ARGS_REPEAT7(...) __VA_ARGS__, VA_ARGS_REPEAT6(__VA_ARGS__)
#define VA_ARGS_REPEAT8(...) __VA_ARGS__, VA_ARGS_REPEAT7(__VA_ARGS__)
#define VA_ARGS_REPEAT9(...) __VA_ARGS__, VA_ARGS_REPEAT8(__VA_ARGS__)
#define VA_ARGS_REPEAT10(...) __VA_ARGS__, VA_ARGS_REPEAT9(__VA_ARGS__)
#define VA_ARGS_REPEAT11(...) __VA_ARGS__, VA_ARGS_REPEAT10(__VA_ARGS__)
#define VA_ARGS_REPEAT12(...) __VA_ARGS__, VA_ARGS_REPEAT11(__VA_ARGS__)
#define VA_ARGS_REPEAT13(...) __VA_ARGS__, VA_ARGS_REPEAT12(__VA_ARGS__)
#define VA_ARGS_REPEAT14(...) __VA_ARGS__, VA_ARGS_REPEAT13(__VA_ARGS__)
#define VA_ARGS_REPEAT15(...) __VA_ARGS__, VA_ARGS_REPEAT14(__VA_ARGS__)
#define VA_ARGS_REPEAT16(...) __VA_ARGS__, VA_ARGS_REPEAT15(__VA_ARGS__)
#define VA_ARGS_REPEAT17(...) __VA_ARGS__, VA_ARGS_REPEAT16(__VA_ARGS__)
#define VA_ARGS_REPEAT18(...) __VA_ARGS__, VA_ARGS_REPEAT17(__VA_ARGS__)
#define VA_ARGS_REPEAT19(...) __VA_ARGS__, VA_ARGS_REPEAT18(__VA_ARGS__)
#define VA_ARGS_REPEAT20(...) __VA_ARGS__, VA_ARGS_REPEAT18(__VA_ARGS__)
#define VA_ARGS_REPEAT21(...) __VA_ARGS__, VA_ARGS_REPEAT20(__VA_ARGS__)
#define VA_ARGS_REPEAT22(...) __VA_ARGS__, VA_ARGS_REPEAT21(__VA_ARGS__)
#define VA_ARGS_REPEAT23(...) __VA_ARGS__, VA_ARGS_REPEAT22(__VA_ARGS__)
#define VA_ARGS_REPEAT24(...) __VA_ARGS__, VA_ARGS_REPEAT23(__VA_ARGS__)
#define VA_ARGS_REPEAT25(...) __VA_ARGS__, VA_ARGS_REPEAT24(__VA_ARGS__)
#define VA_ARGS_REPEAT26(...) __VA_ARGS__, VA_ARGS_REPEAT25(__VA_ARGS__)
#define VA_ARGS_REPEAT27(...) __VA_ARGS__, VA_ARGS_REPEAT26(__VA_ARGS__)
#define VA_ARGS_REPEAT28(...) __VA_ARGS__, VA_ARGS_REPEAT27(__VA_ARGS__)
#define VA_ARGS_REPEAT29(...) __VA_ARGS__, VA_ARGS_REPEAT28(__VA_ARGS__)
#define VA_ARGS_REPEAT30(...) __VA_ARGS__, VA_ARGS_REPEAT29(__VA_ARGS__)
#define VA_ARGS_REPEAT31(...) __VA_ARGS__, VA_ARGS_REPEAT30(__VA_ARGS__)
#define VA_ARGS_REPEAT32(...) __VA_ARGS__, VA_ARGS_REPEAT31(__VA_ARGS__)
#define VA_ARGS_REPEAT_N(n) VA_ARGS_CONCAT(VA_ARGS_REPEAT, n)

#define MACRO_HELPER_REPEAT(n, ...) VA_ARGS_REPEAT_N(n(__VA_ARGS__))

#pragma endregion