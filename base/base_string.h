#ifndef ALib_BASE_STRING_H
#define ALib_BASE_STRING_H

#include <stdio.h>

#define Printfln_array(fstr, arr, count)    \
    Statement(                              \
        printf("[");                            \
            for EachIndex(it, (count)) {    \
                printf((fstr), (arr)[it]);      \
                printf(", ");                   \
            }                                   \
        printf("]\n");                          \
    )

#define printfln(str, ...) printf(str "\n", ##__VA_ARGS__)
#define Str(str) ((String){(str), ArrayCount((str))})

ALibStruct(String) {
    const char *data;
    U64 *len;
};


#endif
