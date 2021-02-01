package src.shared.osbind;

@SuppressWarnings({W.OMIT_0, W.OMIT_1})
public class NValue {

    public static NValue make(Object object, int suggestType) {
        if (object == null) {
            return null;
        }

        if (object instanceof Boolean) {
            boolean word = (boolean) object;
            return new NValue(NType.BOOLEAN, (word ? 1 : 0));
        }

        if (object instanceof Character) {
            char word = (char) object;
            return new NValue(NType.CHAR16, word);
        }

        if (object instanceof Byte) {
            byte word = (byte) object;
            return new NValue(NType.BYTE, word);
        }

        if (object instanceof Short) {
            short word = (short) object;
            return new NValue(NType.SHORT, word);
        }

        if (object instanceof Integer) {
            int word = (int) object;
            return new NValue(NType.INT, word);
        }

        if (object instanceof Long) {
            long word = (long) object;
            return new NValue(NType.LONG, word);
        }

        if (object instanceof Float) {
            long word = makeFloatWord((float) object);
            return new NValue(NType.FLOAT, word);
        }

        if (object instanceof Double) {
            long word = makeDoubleWord((double) object);
            return new NValue(NType.DOUBLE, word);
        }

        if (object instanceof String) {
            String string = (String) object;

            if (suggestType == NType.CHAR8) {
                int  type = NType.CHAR8;
                long word = allocU8Chars(string);
                return new NValue(type, word, CLEAR_TYPE_FREE);
            }

            if (suggestType == NType.CHAR16) {
                int  type = NType.CHAR16;
                long word = allocU16Chars(string);
                return new NValue(type, word, CLEAR_TYPE_FREE);
            }

            //utf32 is unsupported currently.
            //if (suggestType == NType.CHAR32) {
            //}

            if (suggestType >= NType.CUSTOM_PTR) {
                int  type = nativeStringType();
                long word = createString(string);
                return new NValue(type, word, CLEAR_TYPE_RELEASE);
            }

            return null;
        }

        if (object instanceof NValue) {
            return (NValue) object;
        }

        return null;
    }

    public static NValue hold(int type, long word, boolean retained) {
       if (isUnknown(type)) {
           return null;
       }
       if (isVoid(type)) {
           //use null to represent void.
           return null;
       }

       int clearType = retained ? CLEAR_TYPE_RELEASE : CLEAR_TYPE_NONE;
       return new NValue(type, word, clearType);
    }

    public int  nativeType() { return mNativeType; }
    public long nativeWord() { return mNativeWord; }

    public boolean asBoolean() {
        return mNativeWord != 0;
    }

    public char asChar() {
        if (isInteger(mNativeType)) {
            return (char) mNativeWord;
        } else {
            return 0;
        }
    }

    public byte asByte() {
        if (isInteger(mNativeType)) {
            return (byte) mNativeWord;
        } else {
            return 0;
        }
    }

    public short asShort() {
        if (isInteger(mNativeType)) {
            return (short) mNativeWord;
        } else {
            return 0;
        }
    }

    public int asInt() {
        if (isInteger(mNativeType)) {
            return (int) mNativeWord;
        } else {
            return 0;
        }
    }

    public long asLong() {
        if (isInteger(mNativeType)) {
            return (long) mNativeWord;
        } else {
            return 0;
        }
    }

    public float asFloat() {
        if (isInteger(mNativeType)) {
            return (float) mNativeWord;
        }
        if (isFloat(mNativeType)) {
            return floatValue(mNativeWord);
        }
        if (isDouble(mNativeType)) {
            return (float) doubleValue(mNativeWord);
        }
        return 0;
    }

    public double asDouble() {
        if (isInteger(mNativeType)) {
            return (double) mNativeWord;
        }
        if (isFloat(mNativeType)) {
            return (double) floatValue(mNativeWord);
        }
        if (isDouble(mNativeType)) {
            return doubleValue(mNativeWord);
        }
        return 0;
    }

    public String asString() {
        if (mNativeType == NType.CHAR8) {
            return castU8Chars(mNativeWord);
        }
        if (mNativeType == NType.CHAR16) {
            return castU16Chars(mNativeWord);
        }

        //utf32 is unsupported currently.
        //if (mNativeType == NType.CHAR32) {
        //}

        int stringType = nativeStringType();
        if (mNativeType == stringType) {
            return castString(mNativeWord);
        }

        return null;
    }

    private static final int CLEAR_TYPE_NONE    = 0;
    private static final int CLEAR_TYPE_RELEASE = 1;
    private static final int CLEAR_TYPE_FREE    = 2;

    private final int  mNativeType;
    private final long mNativeWord;
    private final int  mClearType ;

    private NValue(int nativeType, long nativeWord) {
        mNativeType = nativeType;
        mNativeWord = nativeWord;
        mClearType  = CLEAR_TYPE_NONE;
    }

    private NValue(int nativeType, long nativeWord, int clearType) {
        mNativeType = nativeType;
        mNativeWord = nativeWord;
        mClearType  = clearType ;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        if (mClearType == CLEAR_TYPE_RELEASE) {
            releaseObject(mNativeWord);
        } else if (mClearType == CLEAR_TYPE_FREE) {
            freeMemory(mNativeWord);
        }
    }

    private static boolean isUnknown(int t) { return t <= NType.PTR   ; }
    private static boolean isVoid   (int t) { return t == NType.VOID  ; }
    private static boolean isInteger(int t) { return t >= NType.BOOLEAN && t <= NType.ULONG; }
    private static boolean isFloat  (int t) { return t == NType.FLOAT ; }
    private static boolean isDouble (int t) { return t == NType.DOUBLE; }

    private static native long   makeFloatWord (float  value);
    private static native long   makeDoubleWord(double value);
    private static native float  floatValue    (long   word );
    private static native double doubleValue   (long   word );

    private static native int nativeStringType();

    private static native long   createString (String string);
    private static native long   allocU8Chars (String string);
    private static native long   allocU16Chars(String string);
    private static native void   releaseObject(long   word  );
    private static native void   freeMemory   (long   word  );
    private static native String castString   (long   word  );
    private static native String castU8Chars  (long   word  );
    private static native String castU16Chars (long   word  );
}