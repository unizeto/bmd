#include <bmd/libbmdasn1_core/linuxwindows.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

/* Funkcja ilogb, scnalb, copysign - kody Ÿród³owe z biblioteki libm */

#ifdef _WIN32

//#include "MetaData.h"
#include <windows.h>

BOOL __cdecl DllMain(
		HANDLE hModule,
		DWORD ul_reason_for_call,
		LPVOID lpReserved
		)
{

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//			initMetaData();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
		return TRUE;
}


void *allocStructure(void *src, unsigned int size)
{
	char *tmp = (char *)malloc(size);
	memmove(tmp, src, size);
	return tmp;
}

void *mallocStructure(unsigned int size)
{
	void *tmp = malloc(size);
	if ( tmp == NULL )
		return NULL;
	memset(tmp, 0, size);
	return tmp;
}
/*
typedef union 
{
  double value;
  struct 
  {
    unsigned long lsw;
    unsigned long msw;
  } parts;
} mtest_double_shape_type;

#define __IEEE_BIG_ENDIAN

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN
 #error Must define endianness
#endif
#endif


#ifdef __IEEE_BIG_ENDIAN

typedef union 
{
  struct 
  {
    uint32_t msw;
    uint32_t lsw;
  } parts;
  double value;
} ieee_double_shape_type;

#endif

#ifdef __IEEE_LITTLE_ENDIAN

typedef union 
{
  struct 
  {
    __uint32_t lsw;
    __uint32_t msw;
  } parts;
  double value;
} ieee_double_shape_type;

#endif



#define EXTRACT_WORDS(ix0,ix1,d)		\
	do {								\
		mtest_double_shape_type ew_u;	\
		ew_u.value = (d);				\
		(ix0) = ew_u.parts.msw;			\
		(ix1) = ew_u.parts.lsw;			\
	} while (0)


#define GET_HIGH_WORD(i,d)			\
do {								\
  ieee_double_shape_type gh_u;		\
  gh_u.value = (d);					\
  (i) = gh_u.parts.msw;				\
} while (0)


#define GET_LOW_WORD(i,d)			\
do {								\
  ieee_double_shape_type gl_u;		\
  gl_u.value = (d);					\
  (i) = gl_u.parts.lsw;				\
} while (0)


#define INSERT_WORDS(d,ix0,ix1)		\
do {								\
  ieee_double_shape_type iw_u;		\
  iw_u.parts.msw = (ix0);			\
  iw_u.parts.lsw = (ix1);			\
  (d) = iw_u.value;					\
} while (0)


#define SET_HIGH_WORD(d,v)			\
do {								\
  ieee_double_shape_type sh_u;		\
  sh_u.value = (d);					\
  sh_u.parts.msw = (v);				\
  (d) = sh_u.value;					\
} while (0)


#define SET_LOW_WORD(d,v)			\
do {								\
  ieee_double_shape_type sl_u;		\
  sl_u.value = (d);					\
  sl_u.parts.lsw = (v);				\
  (d) = sl_u.value;					\
} while (0)

double copysign(double x, double y)
{
	uint32_t hx,hy;
	GET_HIGH_WORD(hx,x);
	GET_HIGH_WORD(hy,y);
	SET_HIGH_WORD(x,(hx&0x7fffffff)|(hy&0x80000000U));
    return x;
}

int ilogb(double x)
{
	int32_t hx,lx,ix;

	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	if(hx<0x00100000) {
	    if((hx|lx)==0) 
		return - INT_MAX;
	    else
		if(hx==0) {
		    for (ix = -1043; lx>0; lx<<=1) ix -=1;
		} else {
		    for (ix = -1022,hx<<=11; hx>0; hx<<=1) ix -=1;
		}
	    return ix;
	}
	else if (hx<0x7ff00000) return (hx>>20)-1023;
	else return INT_MAX;
}

static const double	two54   =  1.80143985094819840000e+16;
static const double twom54  =  5.55111512312578270212e-17;
static const double	huge_d   = 1.0e+300; // HIGH_VAL ??
static const double	tiny_d   = 1.0e-300;

double scalbn (double x, int n)
{
	int32_t  k,hx,lx;
	EXTRACT_WORDS(hx,lx,x);
        k = (hx&0x7ff00000)>>20;
        if (k==0) {
            if ((lx|(hx&0x7fffffff))==0) return x;
	    x *= two54; 
	    GET_HIGH_WORD(hx,x);
	    k = ((hx&0x7ff00000)>>20) - 54; 
            if (n< -50000) return tiny_d*x;
	    }
        if (k==0x7ff) return x+x;
        k = k+n; 
        if (k >  0x7fe) return copysign(huge_d,x);
        if (k > 0)
	    {SET_HIGH_WORD(x,(hx&0x800fffffU)|(k<<20)); return x;}
        if (k <= -54) {
            if (n > 50000)
		return copysign(huge_d,x);
	    else return tiny_d*copysign(tiny_d,x);
	}
        k += 54;
	SET_HIGH_WORD(x,(hx&0x800fffffU)|(k<<20));
        return x*twom54;
}







int opterr=1;
int optopt;

int optind=1;
char *optarg;

static void getopterror(int which) {
  static char error1[]="Unknown option `-x'.\n";
  static char error2[]="Missing argument for `-x'.\n";
  if (opterr) {
    if (which) {
      error2[23]=optopt;
	  fprintf(stderr, error2);
    } else {
      error1[17]=optopt;
	  fprintf(stderr, error1);
    }
  }
}

int getopt(int argc, char * const argv[], const char *optstring) {
  static int lastidx,lastofs;
  char *tmp;
  if (optind==0) optind=1;
again:
  if (optind>argc || !argv[optind] || *argv[optind]!='-' || argv[optind][1]==0)
    return -1;
  if (argv[optind][1]=='-' && argv[optind][2]==0) {
    ++optind;
    return -1;
  }
  if (lastidx!=optind) {
    lastidx=optind; lastofs=0;
  }
  optopt=argv[optind][lastofs+1];
  if ((tmp=strchr(optstring,optopt))) {
    if (*tmp==0) {
      ++optind;
      goto again;
    }
    if (tmp[1]==':') {
      if (tmp[2]==':' || argv[optind][lastofs+2]) {
	if (!*(optarg=argv[optind]+lastofs+2)) optarg=0;
	goto found;
      }
      optarg=argv[optind+1];
      if (!optarg) {
	++optind;
	if (*optstring==':') return ':';
	getopterror(1);
	return ':';
      }
      ++optind;
    } else {
      ++lastofs;
      return optopt;
    }
found:
    ++optind;
    return optopt;
  } else {
    getopterror(0);
    ++optind;
    return '?';
  }
}
*/

#else

void *allocStructure(void *src, unsigned int size)
{
	char *tmp = (char *)malloc(size);
	memmove(tmp, src, size);
	return tmp;
}

void *mallocStructure(unsigned int size)
{
	void *tmp = malloc(size);
	if ( tmp == NULL )
		return NULL;
	memset(tmp, 0, size);
	return tmp;
}


#endif // ifdef _WIN32

