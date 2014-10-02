#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <string.h>
#ifndef WIN32
#include <stdint.h>
#endif
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

#ifdef WIN32
#include <windows.h>
#include <stdarg.h>
#endif

#include <bmd/common/global_types.h>
#include <stdio.h>
#ifdef WIN32
#pragma warning(disable:4100) /* nie ostrzegaj o nieuzywanych parametrach */
#endif

#if !defined(__linux__) && !defined( __FreeBSD__ ) && !defined(__bsdi__ ) && \
	!defined( __OpenBSD__ ) && !defined( __NetBSD__ )
/* prototypes */

/**
  The  function  asprintf  is analogue of sprintf, except that it allocates a string large enough to hold
the output including the terminating NULL, and returns a pointer to it via the first  parameter.
This  pointer  should  be  passed  to free to release the allocated storage when it is no longer needed.

@param ptr pointer to newly allocated string
@param fmt printf()-style format string

@return number of bytes printed or -1

@see man asprintf
*/

int asprintf(char **ptr, const char *fmt, /*args*/ ...)
{
	va_list ap;
	size_t str_m;
	int str_l;

	*ptr = NULL;
	va_start(ap, fmt);                            /* measure the required size */
	str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
	va_end(ap);
	assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	*ptr = (char *) malloc(str_m = (size_t)str_l + 1);
	if (*ptr == NULL)
	{
		errno = ENOMEM;
		str_l = -1;
	}
	else
	{
		int str_l2;
		va_start(ap, fmt);
		str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
		va_end(ap);
		assert(str_l2 == str_l);
	}
	return str_l;
}
/**
  Portable version of vsnprintf() function.

  @see man vsnprintf
*/
int portable_vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap)
{
	size_t str_l = 0;
	const char *p = fmt;

	/* In contrast with POSIX, the ISO C99 now says
	* that str can be NULL and str_m can be 0.
	* This is more useful than the old:  if (str_m < 1) return -1; */

	if (!p)
		p = "";
	while (*p)
	{
	    if (*p != '%')
		{
			const char *q = strchr(p+1,'%');
			size_t n = !q ? strlen(p) : (q-p);
			if (str_l < str_m)
			{
				size_t avail = str_m-str_l;
				memcpy(str+str_l, p, (n>avail?avail:n));
			}
			p += n;
			str_l += n;
		}
		else
		{
			const char *starting_p;
			size_t min_field_width = 0, precision = 0;
			int zero_padding = 0, precision_specified = 0, justify_left = 0;
			int alternate_form = 0, force_sign = 0;
			int space_for_positive = 1; /* If both the ' ' and '+' flags appear,the ' ' flag should be ignored. */
			char length_modifier = '\0';            /* allowed values: \0, h, l, L */
			char tmp[32];/* temporary buffer for simple numeric->string conversion */

			const char *str_arg=NULL;      /* string address in case of string argument */
			size_t str_arg_l;         /* natural field width of arg without padding
                                   and sign */
			unsigned char uchar_arg;
			/* unsigned char argument value - only defined for c conversion.
			N.B. standard explicitly states the char argument for
			the c conversion is unsigned */

			size_t number_of_zeros_to_pad = 0;
			/* number of zeros to be inserted for numeric conversions
			as required by the precision or minimal field width */

			size_t zero_padding_insertion_ind = 0;
			/* index into tmp where zero padding is to be inserted */

			char fmt_spec = '\0';
			/* current conversion specifier character */

			starting_p = p; p++;  /* skip '%' */
			/* parse flags */
			while (*p == '0' || *p == '-' || *p == '+' || *p == ' ' || *p == '#' || *p == '\'')
			{
				switch (*p) {
					case '0':
						zero_padding = 1;
						break;
					case '-':
						justify_left = 1;
						break;
					case '+':
						force_sign = 1;
						space_for_positive = 0;
						break;
					case ' ':
						force_sign = 1;
						/* If both the ' ' and '+' flags appear, the ' ' flag should be ignored */
						break;
					case '#':
						alternate_form = 1;
						break;
					case '\'':
						break;
			}
			p++;
		}
		/* If the '0' and '-' flags both appear, the '0' flag should be ignored. */

		/* parse field width */
		if (*p == '*')
		{
			int j;
			p++; j = va_arg(ap, int);
			if (j >= 0)
				min_field_width = j;
			else
			{
				min_field_width = -j;
				justify_left = 1;
			}
		}
		else
			if (isdigit((int)(*p)))
			{
				/* size_t could be wider than unsigned int;
				make sure we treat argument like common implementations do */
				unsigned int uj = *p++ - '0';
				while (isdigit((int)(*p)))
					uj = 10*uj + (unsigned int)(*p++ - '0');
				min_field_width = uj;
			}
		/* parse precision */
		if (*p == '.')
		{
	        p++;
			precision_specified = 1;
			if (*p == '*')
			{
				int j = va_arg(ap, int);
				p++;
				if (j >= 0)
					precision = j;
				else
				{
					precision_specified = 0;
					precision = 0;
				}
			}
			else
				if (isdigit((int)(*p)))
				{
					/* size_t could be wider than unsigned int;
					make sure we treat argument like common implementations do */
					unsigned int uj = *p++ - '0';
					while (isdigit((int)(*p)))
						uj = 10*uj + (unsigned int)(*p++ - '0');
					precision = uj;
				}
		}
		/* parse 'h', 'l' and 'll' length modifiers */
		if (*p == 'h' || *p == 'l')
		{
			length_modifier = *p; p++;
			if (length_modifier == 'l' && *p == 'l')
			{   /* double l = long long */
				length_modifier = 'l';                 /* treat it as a single 'l' */
				p++;
			}
		}
		fmt_spec = *p;
		/* common synonyms: */
		switch (fmt_spec) {
			case 'i':
				fmt_spec = 'd';
				break;
			case 'D':
				fmt_spec = 'd';
				length_modifier = 'l';
				break;
			case 'U':
				fmt_spec = 'u';
				length_modifier = 'l';
				break;
			case 'O':
				fmt_spec = 'o'; length_modifier = 'l'; break;
			default: break;
		}
		/* get parameter value, do initial processing */
		switch (fmt_spec) {
			case '%': /* % behaves similar to 's' regarding flags and field widths */
			case 'c': /* c behaves similar to 's' regarding flags and field widths */
			case 's':
			length_modifier = '\0';          /* wint_t and wchar_t not supported */
			str_arg_l = 1;
			switch (fmt_spec) {
				case '%':
					str_arg = p; break;
				case 'c': {
					int j = va_arg(ap, int);
					uchar_arg = (unsigned char) j;   /* standard demands unsigned char */
					str_arg = (const char *) &uchar_arg;
					break;
					}
				case 's':
					str_arg = va_arg(ap, const char *);
					if (!str_arg) str_arg_l = 0;
						/* make sure not to address string beyond the specified precision !!! */
					else if (!precision_specified) str_arg_l = strlen(str_arg);
       /* truncate string if necessary as requested by precision */
          else if (precision == 0) str_arg_l = 0;
          else {
       /* memchr on HP does not like n > 2^31  !!! */
            const char *q = memchr(str_arg, '\0',
                             precision <= 0x7fffffff ? precision : 0x7fffffff);
            str_arg_l = !q ? precision : (q-str_arg);
          }
          break;
        default: break;
        }
        break;
      case 'd': case 'u': case 'o': case 'x': case 'X': case 'p': {
        /* NOTE: the u, o, x, X and p conversion specifiers imply
                 the value is unsigned;  d implies a signed value */

        int arg_sign = 0;
          /* 0 if numeric argument is zero (or if pointer is NULL for 'p'),
            +1 if greater than zero (or nonzero for unsigned arguments),
            -1 if negative (unsigned argument is never negative) */

        int int_arg = 0;  unsigned int uint_arg = 0;
          /* only defined for length modifier h, or for no length modifiers */

        long int long_arg = 0;  unsigned long int ulong_arg = 0;
          /* only defined for length modifier l */

        void *ptr_arg = NULL;
          /* pointer argument value -only defined for p conversion */

        if (fmt_spec == 'p') {

          length_modifier = '\0';

          ptr_arg = va_arg(ap, void *);
          if (ptr_arg != NULL) arg_sign = 1;
        } else if (fmt_spec == 'd') {  /* signed */
          switch (length_modifier) {
          case '\0':
          case 'h':
         /* It is non-portable to specify a second argument of char or short
          * to va_arg, because arguments seen by the called function
          * are not char or short.  C converts char and short arguments
          * to int before passing them to a function.
          */
            int_arg = va_arg(ap, int);
            if      (int_arg > 0) arg_sign =  1;
            else if (int_arg < 0) arg_sign = -1;
            break;
          case 'l':
            long_arg = va_arg(ap, long int);
            if      (long_arg > 0) arg_sign =  1;
            else if (long_arg < 0) arg_sign = -1;
            break;
          }
        } else {  /* unsigned */
          switch (length_modifier) {
          case '\0':
          case 'h':
            uint_arg = va_arg(ap, unsigned int);
            if (uint_arg) arg_sign = 1;
            break;
          case 'l':
            ulong_arg = va_arg(ap, unsigned long int);
            if (ulong_arg) arg_sign = 1;
            break;
          }
        }
        str_arg = tmp; str_arg_l = 0;

        if (fmt_spec == 'd') {
          if (force_sign && arg_sign >= 0)
            tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
         /* leave negative numbers for sprintf to handle,
            to avoid handling tricky cases like (short int)(-32768) */
        } else if (alternate_form) {
          if (arg_sign != 0 && (fmt_spec == 'x' || fmt_spec == 'X') )
            { tmp[str_arg_l++] = '0'; tmp[str_arg_l++] = fmt_spec; }
         /* alternate form should have no effect for p conversion, but ... */

        }
        zero_padding_insertion_ind = str_arg_l;
        if (!precision_specified) precision = 1;   /* default precision is 1 */
        if (precision == 0 && arg_sign == 0

        ) {
         /* converted to null string */
         /* When zero value is formatted with an explicit precision 0,
            the resulting formatted string is empty (d, i, u, o, x, X, p).   */
        } else {
          char f[5]; int f_l = 0;
          f[f_l++] = '%';    /* construct a simple format string for sprintf */
          if (!length_modifier) { }
          else if (length_modifier=='2') { f[f_l++] = 'l'; f[f_l++] = 'l'; }
          else f[f_l++] = length_modifier;
          f[f_l++] = fmt_spec; f[f_l++] = '\0';
          if (fmt_spec == 'p') str_arg_l += sprintf(tmp+str_arg_l, f, ptr_arg);
          else if (fmt_spec == 'd') {  /* signed */
            switch (length_modifier) {
            case '\0':
            case 'h': str_arg_l+=sprintf(tmp+str_arg_l, f, int_arg);  break;
            case 'l': str_arg_l+=sprintf(tmp+str_arg_l, f, long_arg); break;
            }
          } else {  /* unsigned */
            switch (length_modifier) {
            case '\0':
            case 'h': str_arg_l+=sprintf(tmp+str_arg_l, f, uint_arg);  break;
            case 'l': str_arg_l+=sprintf(tmp+str_arg_l, f, ulong_arg); break;
            }
          }
         /* include the optional minus sign and possible "0x"
            in the region before the zero padding insertion point */
          if (zero_padding_insertion_ind < str_arg_l &&
              tmp[zero_padding_insertion_ind] == '-') {
            zero_padding_insertion_ind++;
          }
          if (zero_padding_insertion_ind+1 < str_arg_l &&
              tmp[zero_padding_insertion_ind]   == '0' &&
             (tmp[zero_padding_insertion_ind+1] == 'x' ||
              tmp[zero_padding_insertion_ind+1] == 'X') ) {
            zero_padding_insertion_ind += 2;
          }
        }
        { size_t num_of_digits = str_arg_l - zero_padding_insertion_ind;
          if (alternate_form && fmt_spec == 'o') {        /* assure leading zero for alternate-form octal numbers */
            if (!precision_specified || precision < num_of_digits+1) {
             /* precision is increased to force the first character to be zero,
                except if a zero value is formatted with an explicit precision
                of zero */
              precision = num_of_digits+1; precision_specified = 1;
            }
          }
       /* zero padding to specified precision? */
          if (num_of_digits < precision)
            number_of_zeros_to_pad = precision - num_of_digits;
        }
     /* zero padding to specified minimal field width? */
        if (!justify_left && zero_padding) {
          int n = (int)(min_field_width - (str_arg_l+number_of_zeros_to_pad));
          if (n > 0) number_of_zeros_to_pad += n;
        }
        break;
      }
      default: /* unrecognized conversion specifier, keep format string as-is*/
        zero_padding = 0;  /* turn zero padding off for non-numeric convers. */

        if (*p) str_arg_l++;  /* include invalid conversion specifier unchanged
                                 if not at end-of-string */
        break;
      }
      if (*p) p++;      /* step over the just processed conversion specifier */
   /* insert padding to the left as requested by min_field_width;
      this does not include the zero padding in case of numerical conversions*/
      if (!justify_left) {                /* left padding with blank or zero */
        int n = (int)(min_field_width - (str_arg_l+number_of_zeros_to_pad));
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            memset(str+str_l, (zero_padding?'0':' '), ((size_t)n>avail?avail:n));
          }
          str_l += n;
        }
      }
   /* zero padding as requested by the precision or by the minimal field width
    * for numeric conversions required? */
      if (number_of_zeros_to_pad <= 0) {
     /* will not copy first part of numeric right now, *
      * force it to be copied later in its entirety    */
        zero_padding_insertion_ind = 0;
      } else {
     /* insert first part of numerics (sign or '0x') before zero padding */
        size_t n = zero_padding_insertion_ind;
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            memcpy(str+str_l, str_arg, ((size_t)n>avail?avail:n));
          }
          str_l += n;
        }
     /* insert zero padding as requested by the precision or min field width */
        n = number_of_zeros_to_pad;
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            memset(str+str_l, '0', ((size_t)n>avail?avail:n));
          }
          str_l += n;
        }
      }
   /* insert formatted string
    * (or as-is conversion specifier for unknown conversions) */
      { int n = (int)(str_arg_l - zero_padding_insertion_ind);
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            memcpy(str+str_l, str_arg+zero_padding_insertion_ind,
                        ((size_t)n>avail?avail:n));
          }
          str_l += n;
        }
      }
   /* insert right padding */
      if (justify_left) {          /* right blank padding to the field width */
        int n = (int)(min_field_width - (str_arg_l+number_of_zeros_to_pad));
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            memset(str+str_l, ' ', ((size_t)n>avail?avail:n));
          }
          str_l += n;
        }
      }
    }
  }
  if (str_m > 0) { /* make sure the string is null-terminated
                      even at the expense of overwriting the last character
                      (shouldn't happen, but just in case) */
    str[str_l <= str_m-1 ? str_l : str_m-1] = '\0';
  }
  /* Return the number of characters formatted (excluding trailing null
   * character), that is, the number of characters that would have been
   * written to the buffer if it were large enough.
   */
  return (int) str_l;
}

#endif


/**
@file

Ten plik zawiera implementację systemu obsługi błędów. Błędy są generowane
na podstawie klasy i ich opisu, tzn.
\code
kod błędu = - (klasa + bmd_str2int(opis)%1000)
\codeend
Powyższą operację wykonuje makro ERR.
*/

/**

Funkcja oblicza sumę kontrolną podanego stringu. Suma kontrolna jest liczona
za pomocą XORowania poszczególnych bajtów ze sobą.

*/

uint32_t bmd_str2int (char * str)
{
    int i;
    char ret[4]={0,0,0,0};
    uint32_t *retval;

    if (str == NULL || *str == 0) {
	return 0;
    }
    i=0;
    while (*str) {
	ret[i%4] ^= *str;
	str ++;
	i++;
    }
    retval = (uint32_t*) ret;
    return *retval;
}

#ifdef WIN32
	BOOL __stdcall DllMain(
		HANDLE hModule,
		DWORD ul_reason_for_call,
		LPVOID lpReserved
		)
{

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
		return TRUE;
}
#endif

void ___debug_printtime(void)
{
#ifndef WIN32
	struct timeval tv;
	char buf[100];
	gettimeofday(&tv, 0);
	memset(buf, 0, 100);
#ifdef _SunOS_
	ctime_r(&tv.tv_sec, buf, 100);
#else
	ctime_r(&tv.tv_sec, buf);
#endif
	buf[strlen(buf)-1]='\0';
	printf("%s %05lu ", buf, (long) tv.tv_usec);
#endif
}

/*
(added by WSZ)
Pod Linuxem cialo funkcji GetErrMsg() jest generowane podczas budowania trunka do pliku err_dict.c
Pod Windows musze zapewnic atrape tej funckji, aby mozna bylo skompilowac biblioteki z niej korzystajace.
funckja GetErrorWinNum() docelowo miala byc na potrzeby serwera pod Windows - tez definiuje atrape ciala
*/
#ifdef WIN32
	char* GetErrorMsg(long err_code)
	{
		return "";
	}

	long GetErrorWinNum(long error_code)
	{
		return 0;
	}

int bmd_err_set_win32_global_log_file(const char *log_file)
{
	if( log_file != NULL )
	{
		_GLOBAL_win32_log_file=fopen(log_file,"a+");
	}
	else
	{
		_GLOBAL_win32_log_file=NULL;
	}

	return 0;
}

long bmd_err_win32_log_enabled(void)
{
	if( _GLOBAL_win32_log_file == NULL )
		return 0;

	return 1;
}

FILE *bmd_err_win32_get_log_file(void)
{
	return _GLOBAL_win32_log_file;
}

char* dlerror(void)
{
	return NULL;
}
#endif //#ifdef WIN32
