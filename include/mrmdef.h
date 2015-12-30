/* 
 * mrmdef.h - misc definitions for the MRM board
 *
 * History
 * 19 December 2004 - srainwater@ncc.com
 *  - renamed file from stddef.h to mrmdef.h to prevent conflict with 
 *    the real stddef.h that's part of the standard C library.
 */

#define FALSE 0
#define TRUE  1

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define sign(a)  ((a)<0?(-1):(1))
#define signzero(a) ((a)<0?(-1):((a)==0?(0):(1)))
#define abs(a)   ((a)<0?(-(a)):(a))
#define abslim(a,b) ((b)>(a)?(a):((b)<(-(a))?(-(a)):(b)))

#define hextochar(a) ((a)>9?((a)-10+'A'):((a)+'0'))
