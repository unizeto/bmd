#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/common/bmd-include.h>
#include <bmd/common/bmd-errors.h>
#include <bmd/libbmderr/libbmderr.h>

unsigned char *spc_base64_encode(unsigned char *twf_input, size_t len, int wrap) 
{
char b64table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789+/";

unsigned char *output           = NULL;
unsigned char *p                = NULL;
unsigned char *input            = NULL;
size_t i                        = 0;
size_t mod                      = len % 3;
size_t toalloc                  = 0;

	if(mod > 0)
	{
		input = malloc( (len + 3) * sizeof(char));
		if(input == NULL)       {       PRINT_ERROR("LIBBMDBASE64ERR Error: %i\n", NO_MEMORY);  return NULL;    }
		memset(input, 0, (len + 3) * sizeof(char));
		memcpy(input, twf_input, len);
	}
	else
	{
		input = twf_input;
	}

	toalloc = (len / 3) * 4 + (3 - mod) % 3 + 1;
	if (wrap)
	{
		toalloc += len / 57;
		if (len % 57) toalloc++;
	}

	if (toalloc < len) return 0;
	p = output = (unsigned char *)calloc(toalloc+1024, sizeof(unsigned char));
	if(p == NULL)		{	PRINT_ERROR("LIBBMDBASE64ERR Error: %i\n", NO_MEMORY);	return NULL;	}
	
	while (i < len - mod)
	{
		*p++ = b64table[input[i++] >> 2];
		*p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
		*p++ = b64table[((input[i] << 2) | (input[i + 1] >> 6)) & 0x3f];
		*p++ = b64table[input[i + 1] & 0x3f];
		i += 2;
		if (wrap && !(i % 57)) *p++ = '\n';
	}
	if (!mod)
	{
		if (wrap && i % 57) *p++ = '\n';
		*p = 0;
		return output;
	}
	else
	{
		*p++ = b64table[input[i++] >> 2];
		*p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
		if (mod == 1)
		{
			*p++ = '=';
			*p++ = '=';
			if (wrap) *p++ = '\n';
			*p = 0;
			free(input); input = NULL;
			return output;
		}
		else
		{
			*p++ = b64table[(input[i] << 2) & 0x3f];
			*p++ = '=';
			if (wrap) *p++ = '\n';
			*p = 0;
			free(input); input = NULL;
			return output;
		}
	}
}

unsigned int raw_base64_decode(unsigned char *in, unsigned char *out,int strict, int *err) 
{
	char b64revtb[256] = {
  -3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*0-15*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*16-31*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, /*32-47*/
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1, /*48-63*/
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /*64-79*/
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /*80-95*/
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /*96-111*/
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /*112-127*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*128-143*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*144-159*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*160-175*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*176-191*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*192-207*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*208-223*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*224-239*/
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  /*240-255*/
};
  unsigned int  result = 0, x;
  unsigned char buf[3], *p = in, pad = 0;

  *err = 0;
  while (!pad) {
    switch ((x = b64revtb[*p++])) {
      case -3: /* NULL TERMINATOR */
        if (((p - 1) - in) % 4) {*err = 1;
		  printf("Null terminator\n");}
        return result;
      case -2: /* PADDING CHARACTER. INVALID HERE */
        if (((p - 1) - in) % 4 < 2) 
		{
			*err = 1;
			printf("Padding character. Invalid here\n");
			return result;
        } 
		else 
			if (((p - 1) - in) % 4 == 2) 
			{
				/* Make sure there's appropriate padding */
				if (*p != '=') 
				{
					*err = 1;
					printf("Bad padding.\n");
					return result;
				}
				buf[2] = 0;
				pad = 2;
				result++;
				break;
			} 
			else 
			{
				pad = 1;
				result += 2;
				break;
			}
			/*return result;*/
      case -1:
        if (strict) {
          *err = 2;
          return result;
        }
        break;
      default:
        switch (((p - 1) - in) % 4) {
          case 0:
            buf[0] = (unsigned char)(x << 2);
            break;
          case 1:
            buf[0] |= (x >> 4);
            buf[1] = (unsigned char)(x << 4);
            break;
          case 2:
            buf[1] |= (x >> 2);
            buf[2] = (unsigned char )(x << 6);
            break;
          case 3:
            buf[2] |= x;
            result += 3;
            for (x = 0;  x < (unsigned int)(3 - pad);  x++) *out++ = buf[x];
            break;
        }
        break;
    }
  }
  for (x = 0;  x < (unsigned int)(3 - pad);  x++) *out++ = buf[x];
  return result;
};

/* If err is non-zero on exit, then there was an incorrect padding error.  We
 * allocate enough space for all circumstances, but when there is padding, or
 * there are characters outside the character set in the string (which we are
 * supposed to ignore), then we end up allocating too much space.  You can
 * realloc() to the correct length if you wish.
 */

unsigned char *spc_base64_decode(unsigned char *buf, size_t *len, int strict,int *err) 
{
  unsigned char *outbuf	= NULL;
  unsigned char *inbuf	= NULL;
  int i, j;

  if(err == NULL)
  {
	return 0;
  }
  if(buf == NULL)
  {
	*err = -4;
	return 0;
  }
  if(len == NULL)
  {
	*err = -5;
	return 0;
  }
  
  /*wywalamy wszystkie entery z bufora wejściowego*/
  inbuf = (unsigned char *) calloc (strlen((char *)buf)+1, sizeof(unsigned char));
	if(!inbuf)
	{
		*err = -3;
		*len = 0;
		return 0;
	}
	for (i = 0, j = 0; buf[i] != '\0'; i++)
	{
		if (buf[i] != '\n')
		{
			inbuf[j] = buf[i];
			j++;
		}
	}
	inbuf[j] = '\0';
  
	outbuf = (unsigned char *)calloc(3 * (strlen((char *)buf) / 4 + 1), sizeof(unsigned char));
  if (!outbuf) {
    *err = -3;
    *len = 0;
    return 0;
  }
  *len = raw_base64_decode(inbuf, outbuf, strict, err);
  free(inbuf);
  if (*err) {
    free(outbuf);
    *len = 0;
    outbuf = 0;
  }
  return outbuf;
}
