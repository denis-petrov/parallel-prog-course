#include <memory>

#include <iostream>
#include <optional>
#include <windows.h>
#include <string>
#include <vector>
#include <cstdio>

#include <strsafe.h>
#define BUF_SIZE 255

typedef struct ThreadDTO {
    int* addressOfIndex;
    int* addressOfLocalIndex;
    int indexByValue;
} THREADDTO, *PTHREADDTO;
