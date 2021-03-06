#include "def.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#if ANDROID
    #include <android/log.h>
#else
    #include <stdio.h>
#endif

//word:

word pw(const void *p) {
    word w = {0};
    w.asptr = (void *)p;
    return w;
}

word iw(int   i) {word w = {0}; w.asint = i; return w;}
word fw(float f) {word w = {0}; w.asflt = f; return w;}

//comparer:

#define cmp(a, b) (a == b ? 0 : (a > b ? 1 : -1))

int pcmp(word a, word b) {return cmp(a.asptr, b.asptr);}
int icmp(word a, word b) {return cmp(a.asint, b.asint);}
int fcmp(word a, word b) {return cmp(a.asflt, b.asflt);}

int scmp(word a, word b) {
    return strcmp(a.asptr, b.asptr);
}

//memory management:

typedef struct mblock {
    union {
        void *padd;
        int   size;
    };
    char load[];
} mblock;

static void *syscalloc (int   c, int s) {return calloc ((size_t)c, (size_t)s);}
static void *sysrealloc(void *p, int s) {return realloc(/* v* */p, (size_t)s);}

void *mcalloc(int cnt, int sin) {
    int size = szof(mblock) + cnt * sin;
    //"calloc" will initialize all bytes to zero.
    mblock *block = syscalloc(1, size);
    block->size = size;
    return block->load;
}

void *mrealloc(void *ptr, int cnt, int sin) {
    if (!ptr) {
        return mcalloc(cnt, sin);
    }
    
    mblock *block = pmove(ptr, -szof(mblock));
    int     osize = block->size;
    int     nsize = szof(mblock) + cnt * sin;
    
    block = sysrealloc(block, nsize);
    block->size = nsize;
    
    //NOTE: initialize new bytes.
    if (nsize > osize) {
        void *dst = pmove(block, osize);
        int   len = nsize - osize;
        
        mzero(dst, len);
    }
    
    return block->load;
}

void mfree(void *ptr) {
    free(ptr);
}

void mmove(void *dst, const void *src, int size) {
    memmove(dst, src, (size_t)size);
}

void mzero(void *dst, int size) {
    memset(dst, 0, (size_t)size);
}

void *pmove(const void *ptr, int offset) {
    return (char *)ptr + offset;
}

//debug print:

void print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    #if ANDROID
        __android_log_vprint(ANDROID_LOG_DEBUG, "nnn", fmt, args);
    #else
        vprintf(fmt, args);
        printf("\n");
    #endif

    va_end(args);
}
