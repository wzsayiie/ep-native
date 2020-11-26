#pragma once

#include "ndef.h"

nfunc(void *, NAlloc  , (int size));
nfunc(void *, NRealloc, (void *ptr, int size));
nfunc(void *, NDup    , (const void *ptr));
nfunc(void  , NFree   , (void *ptr));

nfunc(int , NMemorySize, (const void *ptr));
nfunc(void, NZeroMemory, (void *ptr, int size));
nfunc(void, NMoveMemory, (void *dst, const void *src, int size));

//NCreate : allocate a pod object and set its reference count to 1.
//NRetain : increment the reference count by 1.
//NRelease: decrease the count by 1. if the count reduced to 0, delete the object.
nfunc(void *, NCreate , (int   length, void *clear));
nfunc(void *, NRetain , (void *object));
nfunc(void  , NRelease, (void *object));
