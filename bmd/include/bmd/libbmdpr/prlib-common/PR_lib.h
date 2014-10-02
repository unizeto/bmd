#ifndef _PR_lib_included_
#define _PR_lib_included_


#define LIBBMDPR_SCOPE
#if 0 /* biblioteka linkowana statycznie ,wiec export niepotrzebny*/
#ifdef WIN32
	#ifdef LIBBMDPR_EXPORTS
		#define LIBBMDPR_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDPR_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDPR_SCOPE
		#endif
	#endif
#else
	#include "unistd.h"
	#define LIBBMDPR_SCOPE
#endif	/* ifdef WIN32*/
#endif /* if 0*/

#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/ANY.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/MetaDatas.h> 
#include <bmd/libbmdpr/bmd_resp/asn1clib/Kontrolka.h>
#include <bmd/libbmdpr/bmd_resp/asn1clib/Kontrolki.h>
#include <bmd/libbmdutils/libbmdutils.h>

#endif /* */
