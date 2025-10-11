#ifndef __SERIALFLEX_H__
#define __SERIALFLEX_H__

#define EXPAND(args) args

#define MAKE_CALL(t, f) t.f

#define PARAMS_COUNT(TAG, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53, _52, _51, _50,    \
                     _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _39, _38, _37, _36, _35,    \
                     _34, _33, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21, _20,    \
                     _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, \
                     _2, _1, N, ...)                                                               \
    TAG##N

#define SERIALIZE_0(p) convert(#p, p)
#define SERIALIZE_1(p) SERIALIZE_0(p)
#define SERIALIZE_2(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_1(__VA_ARGS__))
#define SERIALIZE_3(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_2(__VA_ARGS__))
#define SERIALIZE_4(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_3(__VA_ARGS__))
#define SERIALIZE_5(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_4(__VA_ARGS__))
#define SERIALIZE_6(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_5(__VA_ARGS__))
#define SERIALIZE_7(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_6(__VA_ARGS__))
#define SERIALIZE_8(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_7(__VA_ARGS__))
#define SERIALIZE_9(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_8(__VA_ARGS__))
#define SERIALIZE_10(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_9(__VA_ARGS__))
#define SERIALIZE_11(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_10(__VA_ARGS__))
#define SERIALIZE_12(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_11(__VA_ARGS__))
#define SERIALIZE_13(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_12(__VA_ARGS__))
#define SERIALIZE_14(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_13(__VA_ARGS__))
#define SERIALIZE_15(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_14(__VA_ARGS__))
#define SERIALIZE_16(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_15(__VA_ARGS__))
#define SERIALIZE_17(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_16(__VA_ARGS__))
#define SERIALIZE_18(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_17(__VA_ARGS__))
#define SERIALIZE_19(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_18(__VA_ARGS__))
#define SERIALIZE_20(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_19(__VA_ARGS__))
#define SERIALIZE_21(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_20(__VA_ARGS__))
#define SERIALIZE_22(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_21(__VA_ARGS__))
#define SERIALIZE_23(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_22(__VA_ARGS__))
#define SERIALIZE_24(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_23(__VA_ARGS__))
#define SERIALIZE_25(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_24(__VA_ARGS__))
#define SERIALIZE_26(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_25(__VA_ARGS__))
#define SERIALIZE_27(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_26(__VA_ARGS__))
#define SERIALIZE_28(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_27(__VA_ARGS__))
#define SERIALIZE_29(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_28(__VA_ARGS__))
#define SERIALIZE_30(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_29(__VA_ARGS__))
#define SERIALIZE_31(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_30(__VA_ARGS__))
#define SERIALIZE_32(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_31(__VA_ARGS__))
#define SERIALIZE_33(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_32(__VA_ARGS__))
#define SERIALIZE_34(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_33(__VA_ARGS__))
#define SERIALIZE_35(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_34(__VA_ARGS__))
#define SERIALIZE_36(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_35(__VA_ARGS__))
#define SERIALIZE_37(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_36(__VA_ARGS__))
#define SERIALIZE_38(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_37(__VA_ARGS__))
#define SERIALIZE_39(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_38(__VA_ARGS__))
#define SERIALIZE_40(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_39(__VA_ARGS__))
#define SERIALIZE_41(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_40(__VA_ARGS__))
#define SERIALIZE_42(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_41(__VA_ARGS__))
#define SERIALIZE_43(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_42(__VA_ARGS__))
#define SERIALIZE_44(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_43(__VA_ARGS__))
#define SERIALIZE_45(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_44(__VA_ARGS__))
#define SERIALIZE_46(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_45(__VA_ARGS__))
#define SERIALIZE_47(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_46(__VA_ARGS__))
#define SERIALIZE_48(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_47(__VA_ARGS__))
#define SERIALIZE_49(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_48(__VA_ARGS__))
#define SERIALIZE_50(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_49(__VA_ARGS__))
#define SERIALIZE_51(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_50(__VA_ARGS__))
#define SERIALIZE_52(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_51(__VA_ARGS__))
#define SERIALIZE_53(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_52(__VA_ARGS__))
#define SERIALIZE_54(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_53(__VA_ARGS__))
#define SERIALIZE_55(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_54(__VA_ARGS__))
#define SERIALIZE_56(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_55(__VA_ARGS__))
#define SERIALIZE_57(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_56(__VA_ARGS__))
#define SERIALIZE_58(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_57(__VA_ARGS__))
#define SERIALIZE_59(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_58(__VA_ARGS__))
#define SERIALIZE_60(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_59(__VA_ARGS__))
#define SERIALIZE_61(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_60(__VA_ARGS__))
#define SERIALIZE_62(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_61(__VA_ARGS__))
#define SERIALIZE_63(p, ...) SERIALIZE_0(p).EXPAND(SERIALIZE_62(__VA_ARGS__))

#define SERIALIZE(t, ...)                                                                          \
    MAKE_CALL(t, EXPAND(PARAMS_COUNT(SERIALIZE, __VA_ARGS__, _63, _62, _61, _60, _59, _58, _57,    \
                                     _56, _55, _54, _53, _52, _51, _50, _49, _48, _47, _46, _45,   \
                                     _44, _43, _42, _41, _40, _39, _38, _37, _36, _35, _34, _33,   \
                                     _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21,   \
                                     _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9,    \
                                     _8, _7, _6, _5, _4, _3, _2, _1))(__VA_ARGS__))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NISERIALIZE_0(c, p) convert(#p, c.p)
#define NISERIALIZE_1(c, p) NISERIALIZE_0(c, p)
#define NISERIALIZE_2(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_1(c, __VA_ARGS__))
#define NISERIALIZE_3(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_2(c, __VA_ARGS__))
#define NISERIALIZE_4(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_3(c, __VA_ARGS__))
#define NISERIALIZE_5(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_4(c, __VA_ARGS__))
#define NISERIALIZE_6(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_5(c, __VA_ARGS__))
#define NISERIALIZE_7(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_6(c, __VA_ARGS__))
#define NISERIALIZE_8(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_7(c, __VA_ARGS__))
#define NISERIALIZE_9(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_8(c, __VA_ARGS__))
#define NISERIALIZE_10(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_9(c, __VA_ARGS__))
#define NISERIALIZE_11(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_10(c, __VA_ARGS__))
#define NISERIALIZE_12(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_11(c, __VA_ARGS__))
#define NISERIALIZE_13(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_12(c, __VA_ARGS__))
#define NISERIALIZE_14(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_13(c, __VA_ARGS__))
#define NISERIALIZE_15(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_14(c, __VA_ARGS__))
#define NISERIALIZE_16(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_15(c, __VA_ARGS__))
#define NISERIALIZE_17(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_16(c, __VA_ARGS__))
#define NISERIALIZE_18(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_17(c, __VA_ARGS__))
#define NISERIALIZE_19(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_18(c, __VA_ARGS__))
#define NISERIALIZE_20(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_19(c, __VA_ARGS__))
#define NISERIALIZE_21(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_20(c, __VA_ARGS__))
#define NISERIALIZE_22(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_21(c, __VA_ARGS__))
#define NISERIALIZE_23(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_22(c, __VA_ARGS__))
#define NISERIALIZE_24(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_23(c, __VA_ARGS__))
#define NISERIALIZE_25(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_24(c, __VA_ARGS__))
#define NISERIALIZE_26(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_25(c, __VA_ARGS__))
#define NISERIALIZE_27(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_26(c, __VA_ARGS__))
#define NISERIALIZE_28(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_27(c, __VA_ARGS__))
#define NISERIALIZE_29(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_28(c, __VA_ARGS__))
#define NISERIALIZE_30(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_29(c, __VA_ARGS__))
#define NISERIALIZE_31(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_30(c, __VA_ARGS__))
#define NISERIALIZE_32(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_31(c, __VA_ARGS__))
#define NISERIALIZE_33(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_32(c, __VA_ARGS__))
#define NISERIALIZE_34(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_33(c, __VA_ARGS__))
#define NISERIALIZE_35(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_34(c, __VA_ARGS__))
#define NISERIALIZE_36(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_35(c, __VA_ARGS__))
#define NISERIALIZE_37(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_36(c, __VA_ARGS__))
#define NISERIALIZE_38(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_37(c, __VA_ARGS__))
#define NISERIALIZE_39(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_38(c, __VA_ARGS__))
#define NISERIALIZE_40(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_39(c, __VA_ARGS__))
#define NISERIALIZE_41(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_40(c, __VA_ARGS__))
#define NISERIALIZE_42(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_41(c, __VA_ARGS__))
#define NISERIALIZE_43(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_42(c, __VA_ARGS__))
#define NISERIALIZE_44(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_43(c, __VA_ARGS__))
#define NISERIALIZE_45(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_44(c, __VA_ARGS__))
#define NISERIALIZE_46(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_45(c, __VA_ARGS__))
#define NISERIALIZE_47(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_46(c, __VA_ARGS__))
#define NISERIALIZE_48(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_47(c, __VA_ARGS__))
#define NISERIALIZE_49(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_48(c, __VA_ARGS__))
#define NISERIALIZE_50(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_49(c, __VA_ARGS__))
#define NISERIALIZE_51(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_50(c, __VA_ARGS__))
#define NISERIALIZE_52(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_51(c, __VA_ARGS__))
#define NISERIALIZE_53(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_52(c, __VA_ARGS__))
#define NISERIALIZE_54(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_53(c, __VA_ARGS__))
#define NISERIALIZE_55(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_54(c, __VA_ARGS__))
#define NISERIALIZE_56(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_55(c, __VA_ARGS__))
#define NISERIALIZE_57(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_56(c, __VA_ARGS__))
#define NISERIALIZE_58(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_57(c, __VA_ARGS__))
#define NISERIALIZE_59(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_58(c, __VA_ARGS__))
#define NISERIALIZE_60(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_59(c, __VA_ARGS__))
#define NISERIALIZE_61(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_60(c, __VA_ARGS__))
#define NISERIALIZE_62(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_61(c, __VA_ARGS__))
#define NISERIALIZE_63(c, p, ...) NISERIALIZE_0(c, p).EXPAND(NISERIALIZE_62(c, __VA_ARGS__))

#define NISERIALIZE(t, c, ...)                                                                     \
    MAKE_CALL(t, EXPAND(PARAMS_COUNT(NISERIALIZE, __VA_ARGS__, _63, _62, _61, _60, _59, _58, _57,  \
                                     _56, _55, _54, _53, _52, _51, _50, _49, _48, _47, _46, _45,   \
                                     _44, _43, _42, _41, _40, _39, _38, _37, _36, _35, _34, _33,   \
                                     _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21,   \
                                     _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9,    \
                                     _8, _7, _6, _5, _4, _3, _2, _1))(c, __VA_ARGS__))

#define VISITCLASS(class, ...)                                                                     \
    template <typename T>                                                                          \
    void serialize(T& t, class& c) {                                                               \
        NISERIALIZE(t, c, __VA_ARGS__);                                                            \
    }

#endif
