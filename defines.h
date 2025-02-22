#ifndef DEFINES_H
#define DEFINES_H

#include <Wire.h>

const uint8_t addr_1 = 0x70; // address of first driver IC, can be changed

#define SPACE 0x0000


//ascii
const uint8_t zero = 48; 
const uint8_t nine = 57;

const uint8_t A_UPPER = 65;
const uint8_t Z_UPPER = 90;

const uint8_t a_lower = 97;
const uint8_t z__lower = 112;

const uint8_t digit_place[8] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E};
const uint16_t numbers[10] = {
    0x0C3F, /* 0 */
    0x0406, /* 1 */
    0x00DB, /* 2 */
    0x008F, /* 3 */
    0x00E6, /* 4 */
    0x2069, /* 5 */
    0x00FD, /* 6 */
    0x0007, /* 7 */
    0x00FF, /* 8 */
    0x00EF  /* 9 */
};

const uint16_t letters_upper[26] = {
    0x00F7, /* A */
    0x128F, /* B */
    0x0039, /* C */
    0x120F, /* D */
    0x0079, /* E */
    0x0071, /* F */
    0x00BD, /* G */
    0x00F6, /* H */
    0x1209, /* I */
    0x001E, /* J */
    0x2470, /* K */
    0x0038, /* L */
    0x0536, /* M */
    0x2136, /* N */
    0x003F, /* O */
    0x00F3, /* P */
    0x203F, /* Q */
    0x20F3, /* R */
    0x00ED, /* S */
    0x1201, /* T */
    0x003E, /* U */
    0x0C30, /* V */
    0x2836, /* W */
    0x2D00, /* X */
    0x00EE, /* Y */
    0x0C09  /* Z */
};

const uint16_t letters_lower[26] = {
    0x1058, /* a */
    0x2078, /* b */
    0x00D8, /* c */
    0x088E, /* d */
    0x0858, /* e */
    0x14C0, /* f */
    0x048E, /* g */
    0x1070, /* h */
    0x1000, /* i */
    0x0A10, /* j */
    0x3600, /* k */
    0x0030, /* l */
    0x10D4, /* m */
    0x1050, /* n */
    0x00DC, /* o */
    0x0170, /* p */
    0x0486, /* q */
    0x0050, /* r */
    0x2088, /* s */
    0x0078, /* t */
    0x001C, /* u */
    0x0810, /* v */
    0x2814, /* w */
    0x2D00, /* x */
    0x028E, /* y */
    0x0848  /* z */
};

#endif