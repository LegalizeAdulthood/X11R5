/* Fixnum multiplication
 */

#include "scheme.h"

Object Fixnum_Multiply (a, b) {
    register unsigned aa = a;
    register unsigned ab = b;
    register unsigned prod, prod2;
    register sign = 1;
    if (a < 0) {
	aa = -a;
	sign = -1;
    }
    if (b < 0) {
	ab = -b;
	sign = -sign;
    }
    prod = (aa & 0xFFFF) * (ab & 0xFFFF);
    if (aa & 0xFFFF0000) {
	if (ab & 0xFFFF0000)
	    return Null;
	prod2 = (aa >> 16) * ab;
    } else {
	prod2 = aa * (ab >> 16);
    }
    prod2 += prod >> 16;
    prod &= 0xFFFF;
    if (prod2 > (1 << (VALBITS - 1 - 16)) - 1) {
	if (sign == 1 || prod2 != (1 << (VALBITS - 1 - 16)) || prod != 0)
	    return Null;
	return Make_Fixnum (-SIGNBIT);
    }
    prod += prod2 << 16;
    if (sign == -1)
	prod = - prod;
    return Make_Fixnum (prod);
}
