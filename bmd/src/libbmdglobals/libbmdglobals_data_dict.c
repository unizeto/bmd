#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdsql/data_dict.h>


#ifdef WIN32

/*zasplepka*/
char *GetErrorMsg(long error_code)
{
	error_code;
	return NULL;
}

/*zaslepka*/
FILE* bmd_err_win32_get_log_file(void)
{
	return NULL;
}

/*zaslepka*/
long bmd_err_win32_log_enabled(void)
{
	return 0;
}

#endif /*ifdef WIN32*/


#if !defined(__linux__) && !defined( __FreeBSD__ ) && !defined(__bsdi__ ) && \
	!defined( __OpenBSD__ ) && !defined( __NetBSD__ )

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
/*-------------------------------------------*/


dictionary_data_t *getGlobalUserGroupsDictionary(void)
{
	return _GLOBAL_UserGroupsDictionary;
}


long mask_user_action_with_roles_and_actions(	long **actions,
						long *no_of_ations,
						GenBuf_t *user_chosen_role)
{
char **ans		= NULL;
long i			= 0;
long ans_count		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (user_chosen_role==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (user_chosen_role->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (user_chosen_role->size<=0)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	if ((*actions)!=NULL)
	{
		free0(*actions);
		*no_of_ations=0;
	}

	/******************************************************/
	/* odnalezienie akcji przypisanych do rol uzytkownika */
	/******************************************************/
	//SELECT roles.id, roles.name, actions.id, actions.name FROM roles_and_actions LEFT JOIN roles ON (roles_and_actions.fk_roles_id=roles.id) LEFT JOIN actions ON (roles_and_actions.fk_actions_id=actions.id);
	getColumnWithCondition(	_GLOBAL_RolesAndActionsDictionary, 1, user_chosen_role->buf, 2, &ans_count, &ans);

	if (ans_count>0)
	{
		(*actions)=(long *)malloc(sizeof(long)*(ans_count+1));
		memset((*actions), 0, sizeof(long)*(ans_count+1));
		(*no_of_ations)=ans_count;
		for (i=0; i<ans_count; i++)
		{
			(*actions)[i]=strtol(ans[i], (char**)NULL, 10);
		}
	}

	return BMD_OK;
}

long initSingleDataDictionary(	dictionary_data_t **dictionary,
				char *SQLQuery)
{
long retVal = 0;

	if(dictionary == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(*dictionary != NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }

	(*dictionary)=(dictionary_data_t *)malloc(sizeof(dictionary_data_t));
	if(*dictionary == NULL)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	(*dictionary)->cols=0;
	(*dictionary)->rows=0;

	retVal = asprintf(&((*dictionary)->SQLQuery), "%s", SQLQuery);
	if (retVal == -1)
	{
		free(*dictionary); *dictionary = NULL;
		BMD_FOK(NO_MEMORY);
	}

	return BMD_OK;
}

long freeAllDataDictionary()
{
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_UsersDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_GroupsDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_RolesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_ClassesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_GroupsGraphDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_UserRolesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_RolesAndActionsDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_SecurityDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_AddMetadataTypesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_RoleRightsStructureDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_RoleRightsDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_SecurityCategoriesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_SecurityLevelsDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_UserGroupsDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_UserClassesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_SecurityCategoriesGraphDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_MetadataValuesDictionary));
	BMD_FOK(freeSingleDataDictionary(&_GLOBAL_ActionsDictionary));

	return BMD_OK;
}

long printSingleDataDictionary(	dictionary_data_t *dictionary)
{
long i		= 0;
long j		= 0;
// 	PRINT_TEST("Dictionary SQL:\n%s\n\n",dictionary->SQLQuery);
	/************************/
	/* walidacja parametrow */
	/************************/
	if (dictionary==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	printf("\n");
	printf("\n");

	for (i=0; i<dictionary->rows; i++)
	{
		for (j=0; j<dictionary->cols; j++)
		{
			printf("%s\t",dictionary->data[i][j]);
		}

		printf("\n");
	}
	printf("\n");
	printf("\n");
	return BMD_OK;
}

long freeSingleDataDictionary(	dictionary_data_t **dictionary)
{
long i		= 0;
long j		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (dictionary==NULL)		{	return BMD_ERR_PARAM1;	}
	if ((*dictionary)==NULL)	{	return BMD_ERR_PARAM2;	}

	for (i=0; i<(*dictionary)->rows; i++)
	{
		for (j=0; j<(*dictionary)->cols; j++)
		{
			free((*dictionary)->data[i][j]); (*dictionary)->data[i][j]=NULL;
		}
		free((*dictionary)->data[i]); (*dictionary)->data[i]=NULL;
	}
	free((*dictionary)->data); (*dictionary)->data=NULL;
	free((*dictionary)->SQLQuery); (*dictionary)->SQLQuery=NULL;
	free((*dictionary)); (*dictionary)=NULL;

	return BMD_OK;
}

long getColumnWithCondition(	dictionary_data_t *dictionary,
				long conditionColumnNumber,
				char *conditionValue,
				long ansColumnNumber,
				long *anssCount,
				char ***anss)
{
long i		= 0;

	if (dictionary==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dictionary->data==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (conditionColumnNumber<0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionColumnNumber>dictionary->cols)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ansColumnNumber<0)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (ansColumnNumber>dictionary->cols)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (anss==NULL)					{	BMD_FOK(BMD_ERR_PARAM6);	}
	if ((*anss)!=NULL)				{	BMD_FOK(BMD_ERR_PARAM6);	}

	(*anssCount)=0;

#ifndef WIN32
	if ((_GLOBAL_debug_level==-4) || (_GLOBAL_debug_level==-5))
	{
//  		PRINT_TEST("Wyszukiwana wartość: %s w kolumnie: %li\n", conditionValue, conditionColumnNumber);
//    		printSingleDataDictionary(	dictionary);
	}
#endif //ifndef WIN32

	if (conditionValue==NULL)
	{
		for (i=0; i<dictionary->rows; i++)
		{
			(*anssCount)++;
			(*anss)=(char **)realloc((*anss), sizeof(char *) * ((*anssCount)));
			asprintf(&((*anss)[(*anssCount)-1]), "%s", dictionary->data[i][ansColumnNumber]);
			if ((*anss)[(*anssCount)-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}
	else
	{
		for (i=0; i<dictionary->rows; i++)
		{
//  PRINT_TEST(" *************** pkl ***** dictionary: %s    conditionValue: %s\n", dictionary->data[i][conditionColumnNumber], conditionValue);
			if (strcmp(dictionary->data[i][conditionColumnNumber], conditionValue)==0)
			{
				(*anssCount)++;
				(*anss)=(char **)realloc((*anss), sizeof(char *) * ((*anssCount)));
				asprintf(&((*anss)[(*anssCount)-1]), "%s", dictionary->data[i][ansColumnNumber]);
				if ((*anss)[(*anssCount)-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
		}

	}

	if ((*anssCount)<=0)	{	BMD_FOK_NP(LIBBMDSQL_DICT_VALUE_NOT_FOUND);	}

	return BMD_OK;

}

long getElementWithCondition(	dictionary_data_t *dictionary,
				long conditionColumnNumber,
				char *conditionValue,
				long ansColumnNumber,
				char **ans)
{
long i		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (dictionary==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dictionary->data==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (conditionColumnNumber<0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionColumnNumber>dictionary->cols)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionValue==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (ansColumnNumber<0)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (ansColumnNumber>dictionary->cols)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (ans==NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}
	if ((*ans)!=NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}

#ifndef WIN32
	if ((_GLOBAL_debug_level==-4) || (_GLOBAL_debug_level==-5))
	{
//     		PRINT_TEST("Wyszukiwana wartość: %s w kolumnie: %li\n", conditionValue, conditionColumnNumber);
//     		printSingleDataDictionary(	dictionary);

	}
#endif //ifndef WIN32

	for (i=0; i<dictionary->rows; i++)
	{
		PRINT_DEBUG("MSILEWICZ_DBG_ADD (D)%s == (V)%s\n",dictionary->data[i][conditionColumnNumber], conditionValue);
		if (strcmp(dictionary->data[i][conditionColumnNumber], conditionValue)==0)
		{
			asprintf(&(*ans), "%s", dictionary->data[i][ansColumnNumber]);
			if ((*ans)==NULL)	{	BMD_FOK(NO_MEMORY);	}
			break;
		}
	}


	if ((*ans)==NULL)
	{
		PRINT_ERROR("DICTERR conditionColumnIndex:<%li>; conditionValue:<%s>; resultColumnIndex:<%li>\n", conditionColumnNumber, conditionValue, ansColumnNumber);
		BMD_FOK_NP(LIBBMDSQL_DICT_VALUE_NOT_FOUND);
	}

	return BMD_OK;

}

long getRowWithCondition(	dictionary_data_t *dictionary,
				long conditionColumnNumber,
				char *conditionValue,
				long *ansColIndexs,
				long ansColNumber,
				long *anssCount,
				char ***anss)
{
long i		= 0;
long j		= 0;
long k		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (dictionary==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dictionary->data==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (conditionColumnNumber<0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionColumnNumber>dictionary->cols)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionValue==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (ansColIndexs==NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (ansColNumber<0)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (anss==NULL)					{	BMD_FOK(BMD_ERR_PARAM7);	}
	if ((*anss)!=NULL)				{	BMD_FOK(BMD_ERR_PARAM7);	}

#ifndef WIN32
	if ((_GLOBAL_debug_level==-4) || (_GLOBAL_debug_level==-5))
	{
//  		PRINT_TEST("Wyszukiwana wartość: %s w kolumnie: %li\n", conditionValue, conditionColumnNumber);
//  		printSingleDataDictionary(	dictionary);
	}
#endif //ifndef WIN32

	/*****************************************************************************/
	/* pobieranie danych ze slownika zgodnie z warunkiem okreslonym w parametrze */
	/*****************************************************************************/
	(*anssCount)=0;
	for (i=0; i<dictionary->rows; i++)
	{
		if (strcmp(dictionary->data[i][conditionColumnNumber], conditionValue)==0)
		{
			for (j=0; j<dictionary->cols; j++)
			{
				for (k=0; k<ansColNumber; k++)
				{
					if (ansColIndexs[k]==j)
					{
						(*anssCount)++;
						(*anss)=(char **)realloc((*anss), sizeof(char *) * ((*anssCount)));
						asprintf(&((*anss)[(*anssCount)-1]), "%s", dictionary->data[i][j]);
						if ((*anss)[(*anssCount)-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
					}
				}
			}
			break;
		}
	}

	if ((*anssCount)<=0)	{	BMD_FOK(LIBBMDSQL_DICT_VALUE_NOT_FOUND);	}

	return BMD_OK;

}

long getRowIndexWithCondition(	dictionary_data_t *dictionary,
				long conditionColumnNumber,
				char *conditionValue,
				long *ansIndex)
{
long i		= 0;


	/************************/
	/* walidacja parametrow */
	/************************/
	if (dictionary==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dictionary->data==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (conditionColumnNumber<0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionColumnNumber>dictionary->cols)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionValue==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

#ifndef WIN32
	if ((_GLOBAL_debug_level==-4) || (_GLOBAL_debug_level==-5))
	{
//  		PRINT_TEST("Wyszukiwana wartość: %s w kolumnie: %li\n", conditionValue, conditionColumnNumber);
//  		printSingleDataDictionary(	dictionary);
	}
#endif //ifndef WIN32

	/*****************************************************************************/
	/* pobieranie danych ze slownika zgodnie z warunkiem okreslonym w parametrze */
	/*****************************************************************************/
	(*ansIndex)=-1;
	for (i=0; i<dictionary->rows; i++)
	{
		if (strcmp(dictionary->data[i][conditionColumnNumber], conditionValue)==0)
		{
			(*ansIndex)=i;
			break;
		}
	}

	if ((*ansIndex)<=0)	{	BMD_FOK(LIBBMDSQL_DICT_VALUE_NOT_FOUND);	}

	return BMD_OK;

}

long getCountWithCondition(	dictionary_data_t *dictionary,
				long conditionColumnNumber,
				char *conditionValue,
				long *ansCount)
{
long i		= 0;

	if (dictionary==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dictionary->data==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (conditionColumnNumber<0)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (conditionColumnNumber>dictionary->cols)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*ansCount)=0;

	if (conditionValue==NULL)
	{
		(*ansCount)=dictionary->rows;
	}
	else
	{
		for (i=0; i<dictionary->rows; i++)
		{
			if (strcmp(dictionary->data[i][conditionColumnNumber], conditionValue)==0)
			{
				(*ansCount)++;
			}
		}
	}

	return BMD_OK;

}

long DatabaseDictionaryTypeToID(	char *OID,
					char *table_name,
					char **id)
{
	PRINT_INFO("LIBBMDSQLINF Converting database dictionary type to id\n");
	PRINT_INFO("LIBBMDSQLINF table name: %s OID: %s\n",table_name, OID);
	PRINT_TEST("------------ table name: %s OID: %s\n",table_name, OID);

	/************************/
	/* walidacja parametrow */
	/************************/
	if (OID==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (table_name==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	if (strcmp(table_name, "add_metadata")==0)
	{
		//SELECT id, type, name, sql_cast_string FROM add_metadata_types;
		BMD_FOK_CHG_NP(getElementWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 1, OID, 0, id),
				LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY);
	}
	else if(!strcmp(table_name, "sys_metadata"))
	{
		if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_ID)==0)				*id = STRDUPL("121");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP)==0)		*id = STRDUPL("122");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)==0)		*id = STRDUPL("123");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE)==0)		*id = STRDUPL("124");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE)==0)		*id = STRDUPL("125");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT)==0)		*id = STRDUPL("126");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE)==0)		*id = STRDUPL("127");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)	*id = STRDUPL("128");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID)==0)	*id = STRDUPL("129");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)		*id = STRDUPL("130");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID)==0)	*id = STRDUPL("131");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_LOCATION_ID)==0)		*id = STRDUPL("132");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP)==0)			*id = STRDUPL("133");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER)==0)			*id = STRDUPL("134");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR)==0)		*id = STRDUPL("135");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT)==0)		*id = STRDUPL("136");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE)==0)		*id = STRDUPL("137");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE)==0)	*id = STRDUPL("138");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_TIMESTAMPING)==0)	*id = STRDUPL("139");
		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else if(!strcmp(table_name, "action_metadata"))
	{
		if (strcmp(OID, OID_ACTION_METADATA_ID)==0)					asprintf(id,"34");
		else if (strcmp(OID, OID_ACTION_METADATA_ACTION_LOCATION_ID)==0)		asprintf(id,"35");
		else if (strcmp(OID, OID_ACTION_METADATA_OBJECT_ID)==0)				asprintf(id,"36");
		else if (strcmp(OID, OID_ACTION_METADATA_OBJECT_TYPE)==0)			asprintf(id,"37");
		else if (strcmp(OID, OID_ACTION_METADATA_ACTION)==0)				asprintf(id,"38");
		else if (strcmp(OID, OID_ACTION_METADATA_USER)==0)				asprintf(id,"39");
		else if (strcmp(OID, OID_ACTION_METADATA_USER_CLASS)==0)			asprintf(id,"40");
		else if (strcmp(OID, OID_ACTION_METADATA_STATUS)==0)				asprintf(id,"41");
		else if (strcmp(OID, OID_ACTION_METADATA_STATUS_MESSAGE)==0)			asprintf(id,"42");
		else if (strcmp(OID, OID_ACTION_METADATA_DATE)==0)				asprintf(id,"43");
		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else if(!strcmp(table_name, "pki_metadata"))
	{
		if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_ID)==0)				asprintf(id,"51");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_LOCATION_ID)==0)		asprintf(id,"52");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_CRYPTO_OBJECTS)==0)		asprintf(id,"53");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_OBJECTS)==0)			asprintf(id,"54");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_VERSION)==0)			asprintf(id,"55");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_POLICYOID)==0)			asprintf(id,"56");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_SERIALNUMBER)==0)		asprintf(id,"57");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_GENTIME)==0)			asprintf(id,"58");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMPCERTVALIDTO)==0)	asprintf(id,"59");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TSADN)==0)			asprintf(id,"60");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TSASN)==0)			asprintf(id,"61");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_STANDARD)==0)			asprintf(id,"62");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_USERORSYSTEM)==0)		asprintf(id,"63");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_CONSERVED)==0)			asprintf(id,"64");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP)==0)			asprintf(id,"65");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP_TYPE)==0)		asprintf(id,"66");
		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else
	{
		BMD_FOK(-666666);
	}

	return BMD_OK;
}

long DatabaseDictionaryTypeToSQLCastString(	char *OID,
						char *table_name,
						char **sql_cast_string)
{
	PRINT_INFO("LIBBMDSQLINF Converting database dictionary type to sql cast string\n");
	PRINT_INFO("LIBBMDSQLINF table name: %s OID: %s\n",table_name, OID);
// 	PRINT_TEST("------------ table name: %s OID: %s\n",table_name, OID);

	/************************/
	/* walidacja parametrow */
	/************************/
	if (OID==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (table_name==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(sql_cast_string == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	if (strcmp(table_name, "add_metadata")==0)
	{
		//SELECT id, type, name, sql_cast_string FROM add_metadata_types;
		BMD_FOK_CHG_NP(getElementWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 1, OID, 3, sql_cast_string),
				LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY);
	}
	else if(!strcmp(table_name, "sys_metadata"))
	{
		if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_ID)==0)				*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP)==0)		*sql_cast_string = STRDUPL("timestamp");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)==0)		*sql_cast_string = STRDUPL("varchar");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE)==0)		*sql_cast_string = STRDUPL("varchar");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)	*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID)==0)	*sql_cast_string = STRDUPL("varchar");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID)==0)	*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_LOCATION_ID)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP)==0)			*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER)==0)			*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT)==0)		*sql_cast_string = STRDUPL("integer");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE)==0)		*sql_cast_string = STRDUPL("varchar");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE)==0)	*sql_cast_string = STRDUPL("timestamp");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_TIMESTAMPING)==0)	*sql_cast_string = STRDUPL("integer");
		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else if(!strcmp(table_name, "action_metadata"))
	{
		if (strcmp(OID, OID_ACTION_METADATA_ID)==0)				asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_ACTION_LOCATION_ID)==0)	asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_ACTION_METADATA_OBJECT_ID)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_OBJECT_TYPE)==0)		asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_ACTION)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_USER)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_USER_CLASS)==0)		asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_STATUS)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_ACTION_METADATA_STATUS_MESSAGE)==0)		asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_ACTION_METADATA_DATE)==0)			asprintf(sql_cast_string,"timestamp");
		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else if(!strcmp(table_name, "pki_metadata"))
	{
		if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_ID)==0)				asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_LOCATION_ID)==0)		asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_CRYPTO_OBJECTS)==0)		asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_OBJECTS)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_VERSION)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_POLICYOID)==0)			asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_SERIALNUMBER)==0)		asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_GENTIME)==0)			asprintf(sql_cast_string,"timestamp");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMPCERTVALIDTO)==0)	asprintf(sql_cast_string,"timestamp");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TSADN)==0)			asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TSASN)==0)			asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_STANDARD)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_USERORSYSTEM)==0)		asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_CONSERVED)==0)			asprintf(sql_cast_string,"integer");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP)==0)			asprintf(sql_cast_string,"varchar");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP_TYPE)==0)		asprintf(sql_cast_string,"varchar");
		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else
	{
		BMD_FOK(-666666);
	}

	return BMD_OK;
}

long DatabaseDictionaryTypeToName(	char *OID,
					char **metadataName)
{
long i		= 0;

	PRINT_INFO("LIBBMDSQLINF Converting database dictionary type to name\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (OID==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(metadataName == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }

	getElementWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 1, OID, 2, metadataName);
	if (*metadataName==NULL)
	{
		if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_ID)==0)				*metadataName = STRDUPL("doc_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP)==0)		*metadataName = STRDUPL("doc_timestamp");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)==0)		*metadataName = STRDUPL("doc_filename");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE)==0)		*metadataName = STRDUPL("doc_filesize");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE)==0)		*metadataName = STRDUPL("doc_visible");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT)==0)		*metadataName = STRDUPL("doc_present");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE)==0)		*metadataName = STRDUPL("doc_file_type");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)	*metadataName = STRDUPL("doc_corresponding_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID)==0)	*metadataName = STRDUPL("doc_transaction_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)		*metadataName = STRDUPL("doc_pointing_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID)==0)	*metadataName = STRDUPL("doc_pointing_location_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_LOCATION_ID)==0)		*metadataName = STRDUPL("doc_location_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP)==0)			*metadataName = STRDUPL("doc_group_id");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER)==0)			*metadataName = STRDUPL("doc_owner_identity");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR)==0)		*metadataName = STRDUPL("doc_creator_identity");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT)==0)		*metadataName = STRDUPL("doc_for_grant");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE)==0)		*metadataName = STRDUPL("doc_hash_value");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE)==0)	*metadataName = STRDUPL("doc_expiration_date");
		else if (strcmp(OID, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_TIMESTAMPING)==0)	*metadataName = STRDUPL("doc_for_timestamping");

		else if (strcmp(OID, OID_PKI_METADATA_SIGNATURE)==0)				asprintf(metadataName,"signature");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP)==0)				asprintf(metadataName,"timestamp");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_FROM_SIG)==0)			asprintf(metadataName,"signature_timestamp");
		else if (strcmp(OID, OID_PKI_METADATA_DVCS)==0)					asprintf(metadataName,"dvcs");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_FROM_DVCS)==0)			asprintf(metadataName,"dvcs_timestamp");
		else if (strcmp(OID, OID_PKI_METADATA_DVCS_OF_DVCS)==0)				asprintf(metadataName,"dvcs_dvcs");
		else if (strcmp(OID, OID_PKI_METADATA_DVCS_OF_TIMESTAMP)==0)			asprintf(metadataName,"timestampDvcs");
		else if (strcmp(OID, OID_PKI_METADATA_DVCS_OF_DOCUMENT)==0)			asprintf(metadataName,"file_dvcs");
		else if (strcmp(OID, OID_PKI_METADATA_DVCS_OF_CRYPTO_SIGNATURE)==0)		asprintf(metadataName,"signature_dvcs");
		else if (strcmp(OID, OID_PKI_METADATA_SIGNATURE_XADES)==0)			asprintf(metadataName,"signature_xades");

		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_ID)==0)				asprintf(metadataName,"timestamp_id");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_LOCATION_ID)==0)		asprintf(metadataName,"timestamp_location_id");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_CRYPTO_OBJECTS)==0)		asprintf(metadataName,"timestamp_crypto_objects");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_OBJECTS)==0)			asprintf(metadataName,"timestamp_objects");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_VERSION)==0)			asprintf(metadataName,"timestamp_version");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_POLICYOID)==0)			asprintf(metadataName,"timestamp_policyoid");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_SERIALNUMBER)==0)		asprintf(metadataName,"timestamp_serialnumber");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_GENTIME)==0)			asprintf(metadataName,"timestamp_gentime");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMPCERTVALIDTO)==0)	asprintf(metadataName,"timestamp_timestampcertvalidto");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TSADN)==0)			asprintf(metadataName,"timestamp_tsadn");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TSASN)==0)			asprintf(metadataName,"timestamp_tsasn");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_STANDARD)==0)			asprintf(metadataName,"timestamp_standard");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_USERORSYSTEM)==0)		asprintf(metadataName,"timestamp_userorsystem");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_CONSERVED)==0)			asprintf(metadataName,"timestamp_conserved");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP)==0)			asprintf(metadataName,"timestamp_timestamp");
		else if (strcmp(OID, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP_TYPE)==0)		asprintf(metadataName,"timestamp_timestamp_type");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_USER_ID)==0)				asprintf(metadataName,"user_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_USER_ISSUER)==0)			asprintf(metadataName,"user_certificate_issuer");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_USER_SN)==0)				asprintf(metadataName,"user_certificate_serial_number");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_USER_NAME)==0)			asprintf(metadataName,"user_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_USER_IDENTITY)==0)			asprintf(metadataName,"user_identity");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_USER_ACCEPTED)==0)			asprintf(metadataName,"user_accepted");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_ROLE_ID)==0)				asprintf(metadataName,"user_role_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_ROLE_NAME)==0)			asprintf(metadataName,"user_role_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CURRENT_ROLE_NAME)==0)		asprintf(metadataName,"user_current_role_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_DEFAULT_ROLE_NAME)==0)		asprintf(metadataName,"user_default_role_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID)==0)			asprintf(metadataName,"user_default_role_id");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_GROUP_ID)==0)			asprintf(metadataName,"user_group_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_GROUP_NAME)==0)			asprintf(metadataName,"user_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CURRENT_GROUP_NAME)==0)		asprintf(metadataName,"user_current_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CURRENT_GROUP_ID)==0)		asprintf(metadataName,"user_current_group_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CURRENT_ROLE_NAME)==0)		asprintf(metadataName,"user_current_role_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CURRENT_ROLE_ID)==0)			asprintf(metadataName,"user_current_role_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_PARENT_GROUP_NAME)==0)		asprintf(metadataName,"user_parent_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_PARENT_GROUP_ID)==0)			asprintf(metadataName,"user_parent_group_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CHILD_GROUP_NAME)==0)		asprintf(metadataName,"user_child_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_CHILD_GROUP_ID)==0)			asprintf(metadataName,"user_child_group_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_DEFAULT_GROUP_NAME)==0)		asprintf(metadataName,"user_default_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID)==0)		asprintf(metadataName,"user_default_group_id");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_NEW_GROUP_NAME)==0)			asprintf(metadataName,"user_new_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_NEW_GROUP_PARENT_ID)==0)		asprintf(metadataName,"user_new_group_parents_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_NEW_GROUP_CHILD_ID)==0)		asprintf(metadataName,"user_new_group_child_name");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_CATEGORY_ID)==0)			asprintf(metadataName,"user_security_category_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME)==0)		asprintf(metadataName,"user_security_category_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_CATEGORY_PARENT_NAME)==0)	asprintf(metadataName,"user_parent_security_category_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_CATEGORY_CHILD_NAME)==0)		asprintf(metadataName,"user_child_security_category_name");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_LEVEL_ID)==0)			asprintf(metadataName,"user_security_level_id");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_LEVEL_NAME)==0)			asprintf(metadataName,"user_security_level_name");
		else if (strcmp(OID, OID_SYS_METADATA_CERT_SEC_LEVEL_PRIORITY)==0)		asprintf(metadataName,"user_security_level_priority");

		else if (strcmp(OID, OID_SYS_METADATA_SEC_CAT)==0)				asprintf(metadataName,"doc_security_category_name");
		else if (strcmp(OID, OID_SYS_METADATA_BMD_GROUP)==0)				asprintf(metadataName,"doc_group_name");
		else if (strcmp(OID, OID_SYS_METADATA_BMD_SEC_LEVELS)==0)			asprintf(metadataName,"doc_security_level");

		else if (strcmp(OID, OID_SYS_METADATA_CERT_LOGIN_AS)==0)			asprintf(metadataName,"user_in_behalf_of");
		else if (strcmp(OID, OID_RFC3281_GROUP)==0)					asprintf(metadataName,"request_user_group_name");
		else if (strcmp(OID, OID_RFC3281_CLEARANCE)==0)					asprintf(metadataName,"request_user_clearance");
		else if (strcmp(OID, OID_RFC3281_ROLE)==0)					asprintf(metadataName,"request_user_role_name");

		else if (strcmp(OID, OID_SYS_METADATA_DTG_SYMKEY_HASH)==0)			asprintf(metadataName,"datagram_symkey_hash");
		else if (strcmp(OID, OID_SYS_METADATA_DTG_SYMKEY)==0)				asprintf(metadataName,"datagram_symetric_key");

		else if (strcmp(OID, OID_SYS_METADATA_DTG_FORM)==0)				asprintf(metadataName,"user_form");
		else if (strcmp(OID, OID_SYS_METADATA_ACTION)==0)				asprintf(metadataName,"user_action");

		else if (strcmp(OID, OID_ACTION_METADATA_ID)==0)				asprintf(metadataName,"action_register_id");
		else if (strcmp(OID, OID_ACTION_METADATA_ACTION_LOCATION_ID)==0)		asprintf(metadataName,"action_register_location_id");
		else if (strcmp(OID, OID_ACTION_METADATA_OBJECT_ID)==0)				asprintf(metadataName,"action_register_object_id");
		else if (strcmp(OID, OID_ACTION_METADATA_OBJECT_TYPE)==0)			asprintf(metadataName,"action_register_object_type");
		else if (strcmp(OID, OID_ACTION_METADATA_ACTION)==0)				asprintf(metadataName,"action_register_action");
		else if (strcmp(OID, OID_ACTION_METADATA_USER)==0)				asprintf(metadataName,"action_register_user");
		else if (strcmp(OID, OID_ACTION_METADATA_USER_CLASS)==0)			asprintf(metadataName,"action_register_user_class");
		else if (strcmp(OID, OID_ACTION_METADATA_STATUS	)==0)				asprintf(metadataName,"action_register_status");
		else if (strcmp(OID, OID_ACTION_METADATA_STATUS_MESSAGE)==0)			asprintf(metadataName,"action_register_message");
		else if (strcmp(OID, OID_ACTION_METADATA_DATE)==0)				asprintf(metadataName,"action_register_date");

		else if (strcmp(OID, OID_ACTION_METADATA_DOCUMENT_GET_FIRST)==0)		asprintf(metadataName,"action_register_first_download");
		else if (strcmp(OID, OID_ACTION_METADATA_DOCUMENT_GET_LAST)==0)			asprintf(metadataName,"action_register_last_download");
		else if (strcmp(OID, OID_ACTION_METADATA_AWIZO_SENT_FIRST)==0)			asprintf(metadataName,"action_register_first_awizo");
		else if (strcmp(OID, OID_ACTION_METADATA_AWIZO_SENT_LAST)==0)			asprintf(metadataName,"action_register_last_awizo");

		else if (strcmp(OID, OID_ACTION_METADATA_CERT_CLASS_ID)==0)			asprintf(metadataName,"user_class_id");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_CLASS_NAME)==0)			asprintf(metadataName,"user_class_name");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_PARENT_CLASS_ID)==0)		asprintf(metadataName,"user_class_parent_id");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_PARENT_CLASS_NAME)==0)		asprintf(metadataName,"user_class_parent_name");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_CHILD_CLASS_ID)==0)		asprintf(metadataName,"user_class_child_id");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_CHILD_CLASS_NAME)==0)		asprintf(metadataName,"user_class_child_name");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID)==0)		asprintf(metadataName,"user_class_default_id");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_NAME)==0)		asprintf(metadataName,"user_class_default_name");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID)==0)		asprintf(metadataName,"user_class_chosen_id");
		else if (strcmp(OID, OID_ACTION_METADATA_CERT_CHOSEN_CLASS_NAME)==0)		asprintf(metadataName,"user_class_chosen_name");



		else	{	return LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA;	}
	}
	else
	{
		for (i=0; i<(long)strlen(*metadataName); i++)
		{
			if ((*metadataName)[i]==' ')
			{
				(*metadataName)[i]='_';
			}
		}
	}

	return BMD_OK;
}
