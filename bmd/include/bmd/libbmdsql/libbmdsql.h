#ifndef HAVE_libbmdsql_HEADER_
#define HAVE_libbmdsql_HEADER_


#ifdef WIN32
	#ifdef LIBBMDSQL_EXPORTS
		#define LIBBMDSQL_SCOPE __declspec(dllexport)
	#else
		#define LIBBMDSQL_SCOPE __declspec(dllimport)
	#endif
#else
	#define LIBBMDSQL_SCOPE extern
#endif /* ifdef WIN32*/

#ifdef WIN32
	#define LIBBMDSQL_IMPORT __declspec(dllimport)
#else
	#define LIBBMDSQL_IMPORT extern
#endif /* ifdef WIN32 */


#endif /* ifndef HAVE_libbmdsql_HEADER_ */
