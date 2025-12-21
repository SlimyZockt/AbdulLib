#ifndef ALib_BASE_STRING_H
#define ALib_BASE_STRING_H

#if ALIB_BASE_STRING_STRIP_PREFIX 
#define printfln_array ALibPrintfln_array
#define printfln ALibPrintfln
#define printfln ALibPrintfln
#define Str ALibStr

#endif

#include <stdio.h>

#define ALibPrintfln_array(fstr, arr, count)    \
    ALibStatement(                              \
        printf("[");                            \
            for ALibEachIndex(it, (count)) {    \
                printf((fstr), (arr)[it]);      \
                printf(", ");                   \
            }                                   \
        printf("]\n");                          \
    )

#define ALibPrintfln(str, ...) printf(str "\n", ##__VA_ARGS__)
#define ALibStr(str) ((ALib(String)){(str), ALibArrayCount((str))})

ALibStruct(ALib(String)) {
    const char *data;
    ALib(u64)  *len;
};

#endif
