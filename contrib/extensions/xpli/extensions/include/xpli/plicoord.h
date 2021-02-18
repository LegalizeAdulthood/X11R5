/*
 * Copyright IBM Corporation 1991
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
#ifndef PLICOORD_H
#define PLICOORD_H

/*******************************************************************
*
*  PLI coordinates and conversion functions
*
*  Coordinates reported by the PLI must relate to the display,
*  but must also have greater precision for optimum recognition.
*  The simplest model involves adding a fraction to the display
*  coordinates - eliminate the fraction, and you have display
*  coordinates, keep the fraction and you have the sub-pixel
*  precision needed by recognition.
*
*  Most arithmetic needed for display and recognition, is done
*  using integers. The fractional part is obtained by storing the
*  coordinate as if it were multipled by a power of 2. Conversion
*  can easily be done using shifts, and macros are supplied below
*  to perform this conversion.
*
*  The coordinates are stored as 16 bit, signed numbers. If the
*  power of two used is too great, the coordinates may overflow
*  the 16 bits. If the power of two is too low, precision will
*  be lost. With current technology for digitizers limited to a dynamic
*  range of 2**12, no additional information is gained by
*  having the coordinates stored with greater dynamic range.
* 
*  The following algorithm, which is used to compute the power
*  of two, addresses these issues. A power of two is found which
*  when multiplied by the range of the display coordinates is greater
*  than the range of digitizer coordinates, i.e.
*            range(display)*2**ds > range(digitizer)
*  For example, if range(display) == 640 and range(digitizer) == 2048,
*  then 640 * 2**2 == 2560 > 2048, and ds == 2.
*
*  The digitizer and display are not integrated units, and the
*  digitizer may be tilted or shifted with respect to the display.
*  Variations in the gain of the digitizer may cause less than the
*  full dynamic range to be mapped onto the display. These variations
*  occur on a unit by unit basis, and therefore each unit must be
*  aligned so that digitizer coordinates are mapped onto display
*  coordinates. In addition, this alignment tends to compensate
*  for the visual parallax that results from the pen tip being
*  separated from the plane of the display by a glass or plastic
*  plate.
*
*  A simple linear transform can compensate for these variations.
*  The equations for the transform look like -
*          xd = af*xp + bf*yp + cf
*          yd = df*xp + ef*yp * ff
*  where xp, yp are raw digitizer coordinates and xd, yd are the
*  display coordinates. But, since we want scaled display coordinates,
*  it is trivial to multiply the coefficients a,b,... by the 
*  power of two used to perform scaling and the result is
*          xs = asf*xp + bsf*yp + csf
*          ys = dsf*xp + esf*yp * fsf
*  where as == a*2**ds.
*
*  Normally, the linear transform would be done using floating
*  point arithmetic. But, floating point arithmetic is not available
*  in the device handler where the conversion is performed.
*  Scaled integer arithmetic is used. The coefficients are stored
*  as 32 bit signed numbers, and a scale must be chosen which
*  preserves the precision of the computation and does not overflow
*  the range of the numbers used in the arithmetic. The scale of
*  as must be such that when the least significant bit of as is
*  multiplied by the maximum value of xp, the result is less than
*  0.5, in other words,
*          2**(-cs) * max(xp) * 2 < 1
*  Approximating max(xp) by a power of two, e.g. 2**12, gives
*          2**(-cs) * 2**12 * 2 < 1
*  or
*          2**(-cs) < 2**(-13) -> cs = 14
*  Now that we have a value for k, the actual conversion formula is
*          xs = (a * xp + b * yp + c +- 1) * 2**(-cs)          
*          ys = (d * xp + e * yp + f +- 1) * 2**(-cs)
*  Where, a is effectively (int)(asf * 2**k), and the +- 1 at the end
*  of the formula implements rounding.
*
*********************************************************************/ 

typedef struct {
  long a, b, c, d, e, f;  
} t_plicoeff, *t_plicoeffP;

/* typedefs to remind programmer of different interpretations
   of the underlying unsigned int type */

typedef short t_tCoord;     /* tablet (scaled display) coordinates */
typedef short t_dCoord;     /* display coordinates */

/***************************************************
*
*    COORDINATE CONVERSION FUNCTIONS (MACROS)
*
*    ntc2dc(x) - returns the pixel coordinate nearest to the
*                  scaled coordinate (by rounding), x is the coord,
*
*    ndc2tc(x) - returns the scaled coordinate that is exactly
*                  the pixel coordinate (fractional part == 0),
*                  x is the coordinate, sc is the scale power of 2
*
*    The following macros are included for backward compatibility.
*    the scale and rounding factors are ignored.

*    tc2dc(x,sc,sr) - returns the pixel coordinate nearest to the
*                  scaled coordinate (by rounding), x is the coord,
*                  sc is the power of two used for scaling. sr is the
*                  rounding factor reported by the device driver.
*
*    dc2tc(x,sc) - returns the scaled coordinate that is exactly
*                  the pixel coordinate (fractional part == 0),
*                  x is the coordinate, sc is the scale power of 2. sr is the
*                  rounding factor reported by the device driver.
*
***************************************************/ 

/* COORDSCALE defines number of fraction bits in coordinate */
#define COORDSCALE 2
/* COEFFSCALE defines number of fraction bits in coefficients */
#define COEFFSCALE 14

#define COORDROUND (1<<(COORDSCALE-1))
#define COEFFROUND (1<<(COEFFSCALE-COORDSCALE-1))

/* NOTE these macros may not work on older compilers because of
   the use of shifts on signed integers  */

#define ntc2dc(x) \
  ((t_dCoord)((x >= 0) ? ((x+COORDROUND)>>COORDSCALE) : \
                         ((x-COORDROUND)>>COORDSCALE)))

#define ndc2tc(x) (t_tCoord)(x << COORDSCALE)

/* older forms used to take shift and round as parameters */

#define tc2dc(x, sc, sr) \
  ((t_dCoord)((x >= 0) ? ((x+COORDROUND)>>COORDSCALE) : \
                         ((x-COORDROUND)>>COORDSCALE)))

#define dc2tc(x, sc) (t_tCoord)(x << COORDSCALE)

#endif  /* ifndef PLICOORD_H */

