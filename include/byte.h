/* s6502 - tdwsl 2022 */

#ifndef BYTE_H
#define BYTE_H

#include <stdint.h>

#define getWord(h, l) ( (((word)(h)) << 8) | (word)(l) )

typedef unsigned char byte;
typedef uint16_t word;

#endif
