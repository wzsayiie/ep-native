#include "nstring.h"

nclass(NString, NObject, {
    int       length  ;
    char32_t *u32chars;
    char16_t *u16chars;
    char     *u8chars ;
});

static void StringClear(NString *string) {
    NFreeMemory(string->u32chars);
    NFreeMemory(string->u16chars);
    NFreeMemory(string->u8chars );
}

NString *NStringCreateWithUTFBytes(NUTFType type, const void *begin, const void *end) {
    NString *self = NCreate(nisizeof(NString), StringClear);

    self->length = -1;

    if /**/ (type == NUTF32) {self->u32chars = NDupU32FromBytes(type, begin, end);}
    else if (type == NUTF16) {self->u16chars = NDupU16FromBytes(type, begin, end);}
    else if (type == NUTF8 ) {self->u8chars  = NDupU8FromBytes (type, begin, end);}

    return self;
}

NString *NStringCreateWithUTFChars(NUTFType type, const void *chars) {
    NString *self = NCreate(nisizeof(NString), StringClear);

    self->length = -1;

    if /**/ (type == NUTF32) {self->u32chars = NDupU32FromChars(type, chars);}
    else if (type == NUTF16) {self->u16chars = NDupU16FromChars(type, chars);}
    else if (type == NUTF8 ) {self->u8chars  = NDupU8FromChars (type, chars);}

    return self;
}

NString *NStringCreate(void) {
    NString *self = NCreate(nisizeof(NString), StringClear);
    self->length = -1;
    return self;
}

NString *NStringCopy(NString *that) {
    if (!that) {
        return NULL;
    }

    NString *self = NStringCreate();

    self->length = that->length;
    //only copy one format.
    if /**/ (that->u32chars) {self->u32chars = NDupMemory(that->u32chars);}
    else if (that->u16chars) {self->u16chars = NDupMemory(that->u16chars);}
    else if (that->u8chars ) {self->u8chars  = NDupMemory(that->u8chars );}

    return self;
}

static const void *StringTryMake(NString *s, NUTFType type) {
    if (type == NUTF32) {
        if (!s->u32chars) {
            if /**/ (s->u16chars) {s->u32chars = NDupU32FromChars(NUTF16, s->u16chars);}
            else if (s->u8chars ) {s->u32chars = NDupU32FromChars(NUTF8 , s->u8chars );}
        }
        return s->u32chars;
    }
    if (type == NUTF16) {
        if (!s->u16chars) {
            if /**/ (s->u32chars) {s->u16chars = NDupU16FromChars(NUTF32, s->u32chars);}
            else if (s->u8chars ) {s->u16chars = NDupU16FromChars(NUTF8 , s->u8chars );}
        }
        return s->u16chars;
    }
    if (type == NUTF8) {
        if (!s->u8chars) {
            if /**/ (s->u32chars) {s->u8chars = NDupU8FromChars(NUTF32, s->u32chars);}
            else if (s->u16chars) {s->u8chars = NDupU8FromChars(NUTF16, s->u16chars);}
        }
        return s->u8chars;
    }
    return NULL;
}

const void *StringUTFChars(NString *string, NUTFType type, const void *defaults) {
    if (string) {
        const void *chars = StringTryMake(string, type);
        return chars ? chars : defaults;
    }
    return defaults;
}

const char32_t *NStringU32Chars(NString *s) {return StringUTFChars(s, NUTF32, U"" );}
const char16_t *NStringU16Chars(NString *s) {return StringUTFChars(s, NUTF16, u"" );}
const char     *NStringU8Chars (NString *s) {return StringUTFChars(s, NUTF8 , u8"");}

static int StringUTFSize(NString *string, NUTFType type) {
    if (!string) {
        return 0;
    }

    const void *chars = StringTryMake(string, type);
    if (chars) {
        int size = NMemorySize(chars);

        if (type == NUTF32) {return size - nisizeof(char32_t);}
        if (type == NUTF16) {return size - nisizeof(char16_t);}
        if (type == NUTF8 ) {return size - nisizeof(char    );}

        return 0;
    }
    return 0;
}

int NStringU32Size(NString *s) {return StringUTFSize(s, NUTF32);}
int NStringU16Size(NString *s) {return StringUTFSize(s, NUTF16);}
int NStringU8Size (NString *s) {return StringUTFSize(s, NUTF8 );}

int NStringLength(NString *self) {
    if (!self) {
        return 0;
    }

    if (self->length >= 0) {
        return self->length;
    }

    NUTFCharsStat stat = {0};
    if /**/ (self->u32chars) {NCheckUTFChars(NUTF32, self->u32chars, &stat);}
    else if (self->u16chars) {NCheckUTFChars(NUTF16, self->u16chars, &stat);}
    else if (self->u8chars ) {NCheckUTFChars(NUTF8 , self->u8chars , &stat);}

    self->length = NUTFCharsCount(&stat);
    return self->length;
}

nstruct(StringIterator, {
    NIterator super;

    int (*step)(const void *, char32_t *);
    char32_t current;
    void *remaining;
});

static bool StringIteratorNext(StringIterator *it) {
    int size = it->step(it->remaining, &it->current);
    if (size > 0) {
        it->remaining = (uint8_t *)it->remaining + size;
        return true;
    }
    return false;
}

static void *StringIteratorGet(StringIterator *it) {
    return &it->current;
}

NIterator *NStringRange(NString *self) {
    if (NStringIsEmpty(self)) {
        return NStoreIterator(NULL, 0);
    }

    StringIterator it = {0};
    it.super.next = (NIteratorNextFunc)StringIteratorNext;
    it.super.get  = (NIteratorGetFunc )StringIteratorGet ;

    if /**/ (self->u32chars) {it.step = NReadFromU32Chars; it.remaining = self->u32chars;}
    else if (self->u16chars) {it.step = NReadFromU16Chars; it.remaining = self->u16chars;}
    else if (self->u8chars ) {it.step = NReadFromU8Chars ; it.remaining = self->u8chars ;}

    return NStoreIterator(&it, sizeof(it));
}

bool NStringIsEmpty(NString *self) {
    if (self) {
        return (!self->u32chars
        /**/ && !self->u16chars
        /**/ && !self->u8chars);
    } else {
        return true;
    }
}

static void StringJoin(void **head, void *tail, int zeroSize) {
    if (*head) {
        int headSize = NMemorySize(*head);
        int tailSize = NMemorySize( tail);

        *head = NReallocMemory(*head, headSize - zeroSize + tailSize);
        NMoveMemory((int8_t *)*head + headSize - zeroSize, tail, tailSize);

    } else {
        *head = NDupMemory(tail);
    }
}

static void StringOnlyReserve(NString *string, NUTFType type) {
    if (type != NUTF32) {NFreeMemory(string->u32chars); string->u32chars = NULL;}
    if (type != NUTF16) {NFreeMemory(string->u16chars); string->u16chars = NULL;}
    if (type != NUTF8 ) {NFreeMemory(string->u8chars ); string->u8chars  = NULL;}
}

void NStringAppend(NString *self, NString *that) {
    if (!self || !that) {
        return;
    }

    NUTFType type = 0;

    if /**/ (self->u32chars && that->u32chars) {type = NUTF32;}
    else if (self->u16chars && that->u16chars) {type = NUTF16;}
    else if (self->u8chars  && that->u8chars ) {type = NUTF8 ;}

    else if (that->u32chars) {type = NUTF32; StringTryMake(self, NUTF32);}
    else if (that->u16chars) {type = NUTF16; StringTryMake(self, NUTF16);}
    else if (that->u8chars ) {type = NUTF8 ; StringTryMake(self, NUTF8 );}

    if /**/ (type == NUTF32) {StringJoin((void **)&self->u32chars, that->u32chars, 4);}
    else if (type == NUTF16) {StringJoin((void **)&self->u16chars, that->u16chars, 2);}
    else if (type == NUTF8 ) {StringJoin((void **)&self->u8chars , that->u8chars , 1);}

    StringOnlyReserve(self, type);
    self->length = -1; //invalidate length.
}

int NStringCompare(NString *self, NString *that) {
    NIterator *selfIter = NStringRange(self);
    NIterator *thatIter = NStringRange(that);

    while (true) {
        bool selfValid = selfIter->next(selfIter);
        bool thatValid = thatIter->next(thatIter);

        if (selfValid && thatValid) {
            char32_t selfChar = *(char32_t *)selfIter->get(selfIter);
            char32_t thatChar = *(char32_t *)thatIter->get(thatIter);

            if (selfChar > thatChar) {return  1;}
            if (selfChar < thatChar) {return -1;}

        } else if (selfValid) {
            return 1;

        } else if (thatValid) {
            return -1;

        } else {
            return 0;
        }
    }
}
