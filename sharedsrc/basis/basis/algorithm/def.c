#include "def.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

word pw(const void *p) {
    word w = {0};
    w.asptr = (void *)p;
    return w;
}

word iw(int   i) {word w = {0}; w.asint = i; return w;}
word fw(float f) {word w = {0}; w.asflt = f; return w;}

#define cmp(a, b) (a == b ? 0 : (a > b ? 1 : -1))

int pcmp(word a, word b) {return cmp(a.asptr, b.asptr);}
int icmp(word a, word b) {return cmp(a.asint, b.asint);}
int fcmp(word a, word b) {return cmp(a.asflt, b.asflt);}

int scmp(word a, word b) {
    return strcmp(a.asptr, b.asptr);
}

void *mcalloc(int cnt, int sin) {
    return calloc((size_t)cnt, (size_t)sin);
}

void *mrealloc(void *ptr, int cnt, int sin) {
    return realloc(ptr, (size_t)cnt * sin);
}

void mfree(void *ptr) {
    free(ptr);
}

void mmove(void *dst, const void *src, int size) {
    memmove(dst, src, (size_t)size);
}

void *pmove(void *ptr, int offset) {
    return (char *)ptr + offset;
}

void mzero(void *dst, int size) {
    memset(dst, 0, (size_t)size);
}

void println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    
    printf("\n");
}

void print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
