#ifndef _MIXX_PPHACK_H_
#define _MIXX_PPHACK_H_

#define MIXX_PP_EXPAND(x) x
#define MIXX_PP_STRING2(x) #x
#define MIXX_PP_STRING(x) MIXX_PP_STRING2(x)
#define __MIXX_PP_CAT(x, ...) x ## __VA_ARGS__
#define MIXX_PP_CAT(x, ...) __MIXX_PP_CAT(x, __VA_ARGS__)
#define __MIXX_PP_ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __MIXX_PP_RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0
#define __MIXX_PP_NARG_I(...) MIXX_PP_EXPAND(__MIXX_PP_ARG_N(__VA_ARGS__))
#define MIXX_PP_NARG(...)  __MIXX_PP_NARG_I(__VA_ARGS__, __MIXX_PP_RSEQ_N())

#define MIXX_PP_LOOP_1(i, ...) { constexpr int i = 0; __VA_ARGS__; }
#define MIXX_PP_LOOP_2(i, ...) MIXX_PP_LOOP_1(i, __VA_ARGS__); { constexpr int i = 1; __VA_ARGS__; }
#define MIXX_PP_LOOP_3(i, ...) MIXX_PP_LOOP_2(i, __VA_ARGS__); { constexpr int i = 2; __VA_ARGS__; }
#define MIXX_PP_LOOP_4(i, ...) MIXX_PP_LOOP_3(i, __VA_ARGS__); { constexpr int i = 3; __VA_ARGS__; }
#define MIXX_PP_LOOP_5(i, ...) MIXX_PP_LOOP_4(i, __VA_ARGS__); { constexpr int i = 4; __VA_ARGS__; }
#define MIXX_PP_LOOP_6(i, ...) MIXX_PP_LOOP_5(i, __VA_ARGS__); { constexpr int i = 5; __VA_ARGS__; }
#define MIXX_PP_LOOP_7(i, ...) MIXX_PP_LOOP_6(i, __VA_ARGS__); { constexpr int i = 6; __VA_ARGS__; }
#define MIXX_PP_LOOP_8(i, ...) MIXX_PP_LOOP_7(i, __VA_ARGS__); { constexpr int i = 7; __VA_ARGS__; }
#define MIXX_PP_LOOP_9(i, ...) MIXX_PP_LOOP_8(i, __VA_ARGS__); { constexpr int i = 8; __VA_ARGS__; }
#define MIXX_PP_LOOP_10(i, ...) MIXX_PP_LOOP_9(i, __VA_ARGS__); { constexpr int i = 9; __VA_ARGS__; }
#define MIXX_PP_LOOP_11(i, ...) MIXX_PP_LOOP_10(i, __VA_ARGS__); { constexpr int i = 10; __VA_ARGS__; }
#define MIXX_PP_LOOP_12(i, ...) MIXX_PP_LOOP_11(i, __VA_ARGS__); { constexpr int i = 11; __VA_ARGS__; }
#define MIXX_PP_LOOP_13(i, ...) MIXX_PP_LOOP_12(i, __VA_ARGS__); { constexpr int i = 12; __VA_ARGS__; }
#define MIXX_PP_LOOP_14(i, ...) MIXX_PP_LOOP_13(i, __VA_ARGS__); { constexpr int i = 13; __VA_ARGS__; }
#define MIXX_PP_LOOP_15(i, ...) MIXX_PP_LOOP_14(i, __VA_ARGS__); { constexpr int i = 14; __VA_ARGS__; }
#define MIXX_PP_LOOP_16(i, ...) MIXX_PP_LOOP_15(i, __VA_ARGS__); { constexpr int i = 15; __VA_ARGS__; }
#define MIXX_PP_LOOP_17(i, ...) MIXX_PP_LOOP_16(i, __VA_ARGS__); { constexpr int i = 16; __VA_ARGS__; }
#define MIXX_PP_LOOP_18(i, ...) MIXX_PP_LOOP_17(i, __VA_ARGS__); { constexpr int i = 17; __VA_ARGS__; }
#define MIXX_PP_LOOP_19(i, ...) MIXX_PP_LOOP_18(i, __VA_ARGS__); { constexpr int i = 18; __VA_ARGS__; }
#define MIXX_PP_LOOP_20(i, ...) MIXX_PP_LOOP_19(i, __VA_ARGS__); { constexpr int i = 19; __VA_ARGS__; }
#define MIXX_PP_LOOP_21(i, ...) MIXX_PP_LOOP_20(i, __VA_ARGS__); { constexpr int i = 20; __VA_ARGS__; }
#define MIXX_PP_LOOP_22(i, ...) MIXX_PP_LOOP_21(i, __VA_ARGS__); { constexpr int i = 21; __VA_ARGS__; }
#define MIXX_PP_LOOP_23(i, ...) MIXX_PP_LOOP_22(i, __VA_ARGS__); { constexpr int i = 22; __VA_ARGS__; }
#define MIXX_PP_LOOP_24(i, ...) MIXX_PP_LOOP_23(i, __VA_ARGS__); { constexpr int i = 23; __VA_ARGS__; }
#define MIXX_PP_LOOP_25(i, ...) MIXX_PP_LOOP_24(i, __VA_ARGS__); { constexpr int i = 24; __VA_ARGS__; }
#define MIXX_PP_LOOP_26(i, ...) MIXX_PP_LOOP_25(i, __VA_ARGS__); { constexpr int i = 25; __VA_ARGS__; }
#define MIXX_PP_LOOP_27(i, ...) MIXX_PP_LOOP_26(i, __VA_ARGS__); { constexpr int i = 26; __VA_ARGS__; }
#define MIXX_PP_LOOP_28(i, ...) MIXX_PP_LOOP_27(i, __VA_ARGS__); { constexpr int i = 27; __VA_ARGS__; }
#define MIXX_PP_LOOP_29(i, ...) MIXX_PP_LOOP_28(i, __VA_ARGS__); { constexpr int i = 28; __VA_ARGS__; }
#define MIXX_PP_LOOP_30(i, ...) MIXX_PP_LOOP_29(i, __VA_ARGS__); { constexpr int i = 29; __VA_ARGS__; }
#define MIXX_PP_LOOP_31(i, ...) MIXX_PP_LOOP_30(i, __VA_ARGS__); { constexpr int i = 30; __VA_ARGS__; }
#define MIXX_PP_LOOP_32(i, ...) MIXX_PP_LOOP_31(i, __VA_ARGS__); { constexpr int i = 31; __VA_ARGS__; }
#define MIXX_PP_LOOP_33(i, ...) MIXX_PP_LOOP_32(i, __VA_ARGS__); { constexpr int i = 32; __VA_ARGS__; }
#define MIXX_PP_LOOP_34(i, ...) MIXX_PP_LOOP_33(i, __VA_ARGS__); { constexpr int i = 33; __VA_ARGS__; }
#define MIXX_PP_LOOP_35(i, ...) MIXX_PP_LOOP_34(i, __VA_ARGS__); { constexpr int i = 34; __VA_ARGS__; }
#define MIXX_PP_LOOP_36(i, ...) MIXX_PP_LOOP_35(i, __VA_ARGS__); { constexpr int i = 35; __VA_ARGS__; }
#define MIXX_PP_LOOP_37(i, ...) MIXX_PP_LOOP_36(i, __VA_ARGS__); { constexpr int i = 36; __VA_ARGS__; }
#define MIXX_PP_LOOP_38(i, ...) MIXX_PP_LOOP_37(i, __VA_ARGS__); { constexpr int i = 37; __VA_ARGS__; }
#define MIXX_PP_LOOP_39(i, ...) MIXX_PP_LOOP_38(i, __VA_ARGS__); { constexpr int i = 38; __VA_ARGS__; }
#define MIXX_PP_LOOP_40(i, ...) MIXX_PP_LOOP_39(i, __VA_ARGS__); { constexpr int i = 39; __VA_ARGS__; }
#define MIXX_PP_LOOP_41(i, ...) MIXX_PP_LOOP_40(i, __VA_ARGS__); { constexpr int i = 40; __VA_ARGS__; }
#define MIXX_PP_LOOP_42(i, ...) MIXX_PP_LOOP_41(i, __VA_ARGS__); { constexpr int i = 41; __VA_ARGS__; }
#define MIXX_PP_LOOP_43(i, ...) MIXX_PP_LOOP_42(i, __VA_ARGS__); { constexpr int i = 42; __VA_ARGS__; }
#define MIXX_PP_LOOP_44(i, ...) MIXX_PP_LOOP_43(i, __VA_ARGS__); { constexpr int i = 43; __VA_ARGS__; }
#define MIXX_PP_LOOP_45(i, ...) MIXX_PP_LOOP_44(i, __VA_ARGS__); { constexpr int i = 44; __VA_ARGS__; }
#define MIXX_PP_LOOP_46(i, ...) MIXX_PP_LOOP_45(i, __VA_ARGS__); { constexpr int i = 45; __VA_ARGS__; }
#define MIXX_PP_LOOP_47(i, ...) MIXX_PP_LOOP_46(i, __VA_ARGS__); { constexpr int i = 46; __VA_ARGS__; }
#define MIXX_PP_LOOP_48(i, ...) MIXX_PP_LOOP_47(i, __VA_ARGS__); { constexpr int i = 47; __VA_ARGS__; }
#define MIXX_PP_LOOP_49(i, ...) MIXX_PP_LOOP_48(i, __VA_ARGS__); { constexpr int i = 48; __VA_ARGS__; }
#define MIXX_PP_LOOP_50(i, ...) MIXX_PP_LOOP_49(i, __VA_ARGS__); { constexpr int i = 49; __VA_ARGS__; }
#define MIXX_PP_LOOP_51(i, ...) MIXX_PP_LOOP_50(i, __VA_ARGS__); { constexpr int i = 50; __VA_ARGS__; }
#define MIXX_PP_LOOP_52(i, ...) MIXX_PP_LOOP_51(i, __VA_ARGS__); { constexpr int i = 51; __VA_ARGS__; }
#define MIXX_PP_LOOP_53(i, ...) MIXX_PP_LOOP_52(i, __VA_ARGS__); { constexpr int i = 52; __VA_ARGS__; }
#define MIXX_PP_LOOP_54(i, ...) MIXX_PP_LOOP_53(i, __VA_ARGS__); { constexpr int i = 53; __VA_ARGS__; }
#define MIXX_PP_LOOP_55(i, ...) MIXX_PP_LOOP_54(i, __VA_ARGS__); { constexpr int i = 54; __VA_ARGS__; }
#define MIXX_PP_LOOP_56(i, ...) MIXX_PP_LOOP_55(i, __VA_ARGS__); { constexpr int i = 55; __VA_ARGS__; }
#define MIXX_PP_LOOP_57(i, ...) MIXX_PP_LOOP_56(i, __VA_ARGS__); { constexpr int i = 56; __VA_ARGS__; }
#define MIXX_PP_LOOP_58(i, ...) MIXX_PP_LOOP_57(i, __VA_ARGS__); { constexpr int i = 57; __VA_ARGS__; }
#define MIXX_PP_LOOP_59(i, ...) MIXX_PP_LOOP_58(i, __VA_ARGS__); { constexpr int i = 58; __VA_ARGS__; }
#define MIXX_PP_LOOP_60(i, ...) MIXX_PP_LOOP_59(i, __VA_ARGS__); { constexpr int i = 59; __VA_ARGS__; }
#define MIXX_PP_LOOP_61(i, ...) MIXX_PP_LOOP_60(i, __VA_ARGS__); { constexpr int i = 60; __VA_ARGS__; }
#define MIXX_PP_LOOP_62(i, ...) MIXX_PP_LOOP_61(i, __VA_ARGS__); { constexpr int i = 61; __VA_ARGS__; }
#define MIXX_PP_LOOP_63(i, ...) MIXX_PP_LOOP_62(i, __VA_ARGS__); { constexpr int i = 62; __VA_ARGS__; }
#define MIXX_PP_LOOP_64(i, ...) MIXX_PP_LOOP_63(i, __VA_ARGS__); { constexpr int i = 63; __VA_ARGS__; }

#define _MIXX_TENSOR_1(X) [X]
#define _MIXX_TENSOR_2(X,Y) [Y][X]
#define _MIXX_TENSOR_3(X,Y,Z) [Z][Y][X]
#define _MIXX_TENSOR_4(X,Y,Z,W) [W][Z][Y][X]
#define __MIXX_TENSOR_N(N) _MIXX_TENSOR_##N
#define _MIXX_TENSOR_N(N) __MIXX_TENSOR_N(N)
#define MIXX_TENSOR(Name, ElemTy, ...) \
    ElemTy Name MIXX_PP_EXPAND(_MIXX_TENSOR_N(MIXX_PP_NARG(__VA_ARGS__)) (__VA_ARGS__))
#define MIXX_TENSOR_PTR(Name, ElemTy, ...) \
    ElemTy (*Name) MIXX_PP_EXPAND(_MIXX_TENSOR_N(MIXX_PP_NARG(__VA_ARGS__)) (__VA_ARGS__))
#define MIXX_TENSOR_REF(Name, ElemTy, ...) \
    ElemTy (&Name) MIXX_PP_EXPAND(_MIXX_TENSOR_N(MIXX_PP_NARG(__VA_ARGS__)) (__VA_ARGS__))
#define _MIXX_PP_MUL_1(X) (X)
#define _MIXX_PP_MUL_2(X, Y) ((X) * (Y))
#define _MIXX_PP_MUL_3(X, Y, Z) ((X) * (Y) * (Z))
#define _MIXX_PP_MUL_4(X, Y, Z, W) ((X) * (Y) * (Z) * (W))
#define __MIXX_PP_MUL(N) _MIXX_PP_MUL_##N
#define _MIXX_PP_MUL(N) __MIXX_PP_MUL(N)
#define MIXX_PP_MUL(...) MIXX_PP_EXPAND(_MIXX_PP_MUL(MIXX_PP_NARG(__VA_ARGS__)) (__VA_ARGS__))
#define MIXX_TENSOR_SIZE(ElemTy, ...) (sizeof(ElemTy) * MIXX_PP_MUL(__VA_ARGS__))
#define MIXX_TENSOR_ELEM_COUNT(...) (MIXX_PP_MUL(__VA_ARGS__))
#define MIXX_ALLOC_TENSOR(ElemTy, ...) \
    (ElemTy(*)\
        MIXX_PP_EXPAND(_MIXX_TENSOR_N(MIXX_PP_NARG(__VA_ARGS__)) (__VA_ARGS__))) malloc(MIXX_TENSOR_SIZE(ElemTy, __VA_ARGS__))

#define _mixx_unused(x) ((void)(x))

#endif // !_MIXX_PPHACK_H_
