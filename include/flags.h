/* s6502 - tdwsl 2022 */

#ifndef FLAGS_H
#define FLAGS_H

enum {
	f_N = 0x80, /* negative */
	f_V = 0x40, /* overflow */
	f_BH= 0x20, /* hi-bit of b flag */
	f_BL= 0x10, /* lo-bit of b flag */
	f_D = 0x08, /* decimal */
	f_I = 0x04, /* interrupt disable */
	f_Z = 0x02, /* zero */
	f_C = 0x01, /* carry */
};

#endif
