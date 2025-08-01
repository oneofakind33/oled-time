#ifndef FONT_H
#define FONT_H

#include <stdint.h>

extern const uint8_t Font6x10[][10]; //extern used to tell the compiler, not to load the fucntion in memory but just know that it exists.
extern const uint8_t FontThin6x10[][10];
extern const uint8_t FontWide6x10[][10];
extern const uint8_t FontItalic6x10[][10];

typedef enum {
    FONT_BOLD = 0,
    FONT_THIN = 1,
    FONT_WIDE = 2,
    FONT_ITALIC = 3
} FontType;

const uint8_t (*get_font(FontType type))[10];

#endif
