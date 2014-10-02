#ifndef __LINUXWINDOWS_H__
#define __LINUXWINDOWS_H__
	/*
	 * Dla zachowania kompatybilnoci z Windowsem powsta � plik nag�kowy.
	 * Nanoszone dodatki odzwierciedlaj system Linuksowy, na kt�ym prowadziem
	 * kompilacje.
	 */

#ifdef WIN32
/*
	<bits/wordsize.h>
	#define __WORDSIZE 32

	<bits/types.h>
	#if __WORDSIZE == 32
		#define __SWORD_TYPE int
	#elif __WORDSIZE == 64
		#define __SWORD_TYPE long int
	#else
		#error
	#endif

	<bits/types.h + unistd.h>
	#define ssize_t __SWORD_TYPE

	<stdint.h>
	typedef char int8_t;
	typedef unsigned char uint8_t;
	typedef int int32_t;
	typedef unsigned int uint32_t;
	typedef unsigned short uint16_t;

	math.h libm.so
	int ilogb(double x);
	double scalbn (double x, int n);
	double copysign(double x, double y);

	getopt.h libc.so
	extern int opterr;
	extern int optopt;
	extern int optind;
	extern char *optarg;
	int getopt(int argc, char * const argv[], const char *optstring);
	syserrors
	#define EX_UNAVAILABLE		69
	#define EX_USAGE		64
	#define EX_DATAERR		65
	#define EX_OSERR		71
*/
	
	#ifdef LIBBMDASN1_CORE_EXPORTS
		#define LIBBMDASN1_CORE_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDASN1_CORE_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDASN1_CORE_SCOPE
		#endif
	#endif
	
	/* funkcja allokujca przestrze�struktury wraz z jej kopi */
	LIBBMDASN1_CORE_SCOPE void *allocStructure(void *, unsigned int);
	LIBBMDASN1_CORE_SCOPE void *mallocStructure(unsigned int size);

#else /* ifdef _WIN32*/
	#define LIBBMDASN1_CORE_SCOPE
	#include <stdlib.h>
	LIBBMDASN1_CORE_SCOPE void *allocStructure(void *, unsigned int);
	LIBBMDASN1_CORE_SCOPE void *mallocStructure(unsigned int size);
#endif /* ifdef _WIN32*/

#endif /* ifndef __LINUXWINDOWS_H__*/

