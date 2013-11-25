
unsigned long
float32()
    CODE:
    RETVAL = paFloat32;
    OUTPUT:
    RETVAL


unsigned long
int16()
    CODE:
    RETVAL = paInt16;
    OUTPUT:
    RETVAL


unsigned long
int32()
    CODE:
    RETVAL = paInt32;
    OUTPUT:
    RETVAL


unsigned long
int24()
    CODE:
    RETVAL = paInt24;
    OUTPUT:
    RETVAL


unsigned long
int8()
    CODE:
    RETVAL = paInt8;
    OUTPUT:
    RETVAL


unsigned long
uint8()
    CODE:
    RETVAL = paUInt8;
    OUTPUT:
    RETVAL


unsigned long
CLIP_OFF()
    CODE:
    RETVAL = paClipOff;
    OUTPUT:
    RETVAL


unsigned long
DITHER_OFF()
    CODE:
    RETVAL = paDitherOff;
    OUTPUT:
    RETVAL


unsigned long
NEVER_DROPINPUT()
    CODE:
    RETVAL = paNeverDropInput;
    OUTPUT:
    RETVAL

