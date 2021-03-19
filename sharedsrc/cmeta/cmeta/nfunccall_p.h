#pragma once

#if !__cplusplus
    #error "c++ needed"
#endif

#include "nenviron.h"

typedef int64_t _NWord;

void FuncPrepare();
void FuncPushArg(NType type, _NWord word);

int64_t FuncCall(int funcIndex);
