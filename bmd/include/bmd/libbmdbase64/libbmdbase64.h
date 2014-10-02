#ifndef _LIBBMDBASE64_INCLUDED_
#define _LIBBMDBASE64_INCLUDED_

#include <sys/types.h>
#include <bmd/common/bmd-os_specific.h>
#include <bmd/common/bmd-include.h>

#ifdef WIN32
	#ifdef LIBBMDBASE64_EXPORTS
		#define LIBBMDBASE64_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDBASE64_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDBASE64_SCOPE
		#endif
	#endif
#else
	#define LIBBMDBASE64_SCOPE
#endif /* ifdef WIN32 */


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

typedef enum
{
	step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
	base64_encodestep step;
	char result;
	int stepcount;
} base64_encodestate;

LIBBMDBASE64_SCOPE void base64_init_decodestate(base64_decodestate* state_in);
LIBBMDBASE64_SCOPE int  base64_decode_value(char value_in);
LIBBMDBASE64_SCOPE int  base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);
LIBBMDBASE64_SCOPE void base64_init_encodestate(base64_encodestate* state_in);
LIBBMDBASE64_SCOPE char base64_encode_value(char value_in);
LIBBMDBASE64_SCOPE int  base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in);
LIBBMDBASE64_SCOPE int  base64_encode_blockend(char* code_out, base64_encodestate* state_in);

LIBBMDBASE64_SCOPE unsigned char *spc_base64_encode(unsigned char *input, size_t len, int wrap);
LIBBMDBASE64_SCOPE unsigned int raw_base64_decode(unsigned char *in, unsigned char *out,int strict, int *err);
/**
 * \param[in] buf Bufor wejściowy zakodowany do base64.
 * \param[in] len Ilość zdekodowanych bajtów.
 * \param[in] strict Czy dopuszczalne są białe znaki (1), czy nie (0)?
 * \param[out] err Tutaj zapisany zostanie ewentualny kod błędu.
 *
 * \return Funkcja zwraca wskaźnik na zdekodowany bufor.
 * \note Funkcja dokonuje automatycznej alokacji wymaganego miejsca pod zdekodowany bufor.
 * */
LIBBMDBASE64_SCOPE unsigned char *spc_base64_decode(unsigned char *buf, size_t *len, int strict,int *err);

#if defined(__cplusplus)
}
#endif
#endif
