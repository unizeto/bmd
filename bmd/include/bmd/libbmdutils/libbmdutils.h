#ifndef _LIBBMDUTILS_INCLUDED_
#define _LIBBMDUTILS_INCLUDED_

#include <bmd/common/bmd-common.h>
#include <bmd/common/bmd-asn1.h>
#include <bmd/common/bmd-openssl.h>
#include <bmd/common/global_types.h>

#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#include <assert.h>

#ifdef WIN32
/* gettimeofday dla Windows - ze strony:
*/
/* http://www.usenet.com/newsgroups/comp.ai.neural-nets/msg01068.html
*/

#define EPOCHFILETIME (116444736000000000i64)

struct timezone {
    int tz_minuteswest; /* minutes W of Greenwich */
    int tz_dsttime;     /* type of dst correction */
};
#endif

#ifdef WIN32
	#ifdef LIBBMDUTILS_EXPORTS
		#define LIBBMDUTILS_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDUTILS_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDUTILS_SCOPE
		#endif
	#endif
#else
	#define LIBBMDUTILS_SCOPE
#endif /* ifdef WIN32
*/

#if defined(__cplusplus)
extern "C" {
#endif
	/* GenBuf */
	LIBBMDUTILS_SCOPE long set_gen_buf_ptr(char **buf, long size, GenBuf_t **output);
	LIBBMDUTILS_SCOPE long set_gen_buf(const char *content, long *length, GenBuf_t ** output);
	LIBBMDUTILS_SCOPE long set_gen_buf2(const char *content, long length, GenBuf_t ** output);
	LIBBMDUTILS_SCOPE long set_empty_gen_buf(	GenBuf_t **dest);
	LIBBMDUTILS_SCOPE void free_gen_buf(GenBuf_t ** bufor);
	LIBBMDUTILS_SCOPE void free_certificate_attributes(	certificate_attributes_t **UserAttributes);
	LIBBMDUTILS_SCOPE long bmd_save_buf(GenBuf_t *buf,char *filename);
	LIBBMDUTILS_SCOPE long bmd_file_move(char *source_path, char *dest_path);
	LIBBMDUTILS_SCOPE long bmd_merge_genbufs(GenBuf_t *gb1, GenBuf_t* gb2, GenBuf_t **output);
	LIBBMDUTILS_SCOPE long remove_dir(char *location);
	LIBBMDUTILS_SCOPE long count_files_in_dir(char *location);
	LIBBMDUTILS_SCOPE long load_translation(	char *translate_file, translate_t **translate, long *translate_count);
	LIBBMDUTILS_SCOPE long bmd_clone_genbuf_content(GenBuf_t *src, GenBuf_t *dest);


	/* ASN.1 */
	LIBBMDUTILS_SCOPE long GenBuf2Octetstring2Any(const GenBuf_t * GenBuf,ANY_t * UserDataANYBuf);
	LIBBMDUTILS_SCOPE long Any2Octetstring2GenBuf(ANY_t * UserDataANYBuf,GenBuf_t * GenBuf);
	LIBBMDUTILS_SCOPE long Any2Octetstring2String(ANY_t * UserDataANYBuf,char **string);
	LIBBMDUTILS_SCOPE long str_of_OID_to_ulong(const char *string,long **oid_table, long *oid_table_size);
	LIBBMDUTILS_SCOPE long IsOidFormat(char *string);

	/* dynamiczne formularze */
	LIBBMDUTILS_SCOPE void free_BMD_attr_value_list(BMD_attr_value ***list);
	LIBBMDUTILS_SCOPE void free_BMD_attr_list(BMD_attr_t *** list);
	LIBBMDUTILS_SCOPE void free_BMD_attr(BMD_attr_t ** attr);
	LIBBMDUTILS_SCOPE long is_send_control(long place, long show);
	LIBBMDUTILS_SCOPE long is_search_control(long place, long show);
	LIBBMDUTILS_SCOPE long is_grid_control(long place, long show);
	LIBBMDUTILS_SCOPE long is_unvisible_used_control(long show);
	LIBBMDUTILS_SCOPE long get_pos(long pos);
	LIBBMDUTILS_SCOPE long get_visible(long show);
	LIBBMDUTILS_SCOPE long BMD_attr_init(BMD_attr_t ** buf);
	LIBBMDUTILS_SCOPE long BMD_attr_duplicate(BMD_attr_t *input,long seqNumber,BMD_attr_t **output,
		                                     long dup_values,long dup_priority,long dup_depValuesList);

	LIBBMDUTILS_SCOPE long duplicate_values(BMD_attr_t * source,BMD_attr_value *** dest);
	LIBBMDUTILS_SCOPE long create_send_kontrolki(BMD_attr_t ** kontrolki,BMD_attr_t ***send_kontrolki);
	LIBBMDUTILS_SCOPE long create_specific_kontrolki(BMD_attr_t **kontrolki, KontrolkiType_t KontrolkiType, BMD_attr_t ***spec_kontrolki);

	/** implementacja tablicy stringow */
	LIBBMDUTILS_SCOPE long bmdStringArray_Create(bmdStringArray_t** newArray);
	LIBBMDUTILS_SCOPE long bmdStringArray_AddItem(bmdStringArray_t* stringArray, char* string);
	LIBBMDUTILS_SCOPE long bmdStringArray_GetItem(bmdStringArray_t* stringArray, long itemIndex, char** string);
	LIBBMDUTILS_SCOPE long bmdStringArray_GetItemPtr(bmdStringArray_t* stringArray, long itemIndex, char** stringPtr);
	LIBBMDUTILS_SCOPE long bmdStringArray_GetItemsCount(bmdStringArray_t* stringArray, long* itemsCount);
	LIBBMDUTILS_SCOPE long bmdStringArray_DestroyList(bmdStringArray_t** stringArray);


	/* CURL */
	LIBBMDUTILS_SCOPE GenBuf_t *get_from_url(char *url);
	LIBBMDUTILS_SCOPE size_t  write_to_gen_buf_callback(void *ptr, long size, long nmemb, void *data);
	LIBBMDUTILS_SCOPE void *myrealloc(void *ptr, long size);

	/* UTILS */
	LIBBMDUTILS_SCOPE long bmdGetSubstring(char *string, long start, long end, char** substring);
	LIBBMDUTILS_SCOPE long DestroyTableOfStrings(char*** table, long stringsCount);
	LIBBMDUTILS_SCOPE long DestroyTableOfBuffers(GenBuf_t*** table, long buffersCount);
	LIBBMDUTILS_SCOPE long GetActualTime(struct tm *actualTime);
	LIBBMDUTILS_SCOPE long bmd_load_binary_content(const char *filename, GenBuf_t **buffer);
	LIBBMDUTILS_SCOPE long bmd_str_replace(char **input,char *keyword,char *replace_with);
	LIBBMDUTILS_SCOPE long bmd_get_files_from_dir(char *dir,char ***files);
	LIBBMDUTILS_SCOPE long IsNumericIdentifier(char* identifier);
#ifdef WIN32
	int strcasecmp(const char *s1, const char *s2);
	int strncasecmp(const char *s1, const char *s2, size_t n);
#endif /*ifdef WIN32*/
/*<OBSOLETE>
	LIBBMDUTILS_SCOPE long wcs2char(const wchar_t *wcs,char **output,long code_page);
	LIBBMDUTILS_SCOPE long char2wcs(const char *src,wchar_t **dest);
</OBSOLETE>
*/
	LIBBMDUTILS_SCOPE long file_not_exists(char * filename);
	LIBBMDUTILS_SCOPE long determine_oid_type_str(	char *oid);
	LIBBMDUTILS_SCOPE char *determine_oid_column_name (char *oid);
	LIBBMDUTILS_SCOPE long metadata_get_value_by_oid(	char *oid, MetaDataBuf_t **metadata, long nof_metadata, GenBuf_t **buf);

	/* UTILS dla SZU */
	LIBBMDUTILS_SCOPE long find_substrings(const GenBuf_t * parsing_buff, const char *pattern,SubstringsGenBuf_t *substrings_genbuf,
						pcre_multiline_t flag,boolean_t find_all);
	LIBBMDUTILS_SCOPE long free_substrings(SubstringsGenBuf_t *substrings_genbuf);

	/*odpowiednik standardowej funkcji C, ktora potrafila sie SEGVaulcic badz dawac glupoty w wyniku*/
	LIBBMDUTILS_SCOPE long bmd_strmerge(char *str1, char *str2, char **dest);
	/*uzyteczne */
	LIBBMDUTILS_SCOPE char* get_file_line(FILE *plik);

	/*<UnicodeSupport.cpp>
*/
	LIBBMDUTILS_SCOPE long ValidateFirstUtf8Character(char *utf8Char, long *bytesCount);

#ifdef WIN32
	LIBBMDUTILS_SCOPE long bmd_load_binary_content_wide(const wchar_t *filename, GenBuf_t **buffer);
	LIBBMDUTILS_SCOPE long bmd_save_buf_wide(GenBuf_t *buf,wchar_t *filename);

	LIBBMDUTILS_SCOPE long gettimeofday(struct timeval *tv, struct timezone *tz);
	LIBBMDUTILS_SCOPE long setenv(const char *name, const char *value, long overwrite);
	LIBBMDUTILS_SCOPE long unsetenv(const char *name);
	/*<UnicodeSupport.cpp>
*/
	LIBBMDUTILS_SCOPE long WindowsStringToUTF8String(wchar_t *windowsString, char **utf8String, long *utf8StringLength);
	LIBBMDUTILS_SCOPE long WindowsStringToCurrentCodePage(const wchar_t *windowsString, char **outputString, long *outputStringLength);
	LIBBMDUTILS_SCOPE long UTF8StringToWindowsString(char *utf8String, wchar_t **windowsString, long *windowsStringLength);
	LIBBMDUTILS_SCOPE long UTF8StringToCurrentCodePage(const char *utf8String, char **outputString, long *outputStringLength);
#endif

	LIBBMDUTILS_SCOPE long GetSortConditions(char *sort, server_request_data_t* req, char ***sort_oids,long *count_oids,
											 char ***sort_orders,long *count_orders);

	/* <bmd_ipc.c> */
#ifndef WIN32
	LIBBMDUTILS_SCOPE long bmdCreateSemaphore(key_t *semaphoreKey, int *semId);
	LIBBMDUTILS_SCOPE long bmdRemoveSemaphore(int *semaphoreId);
	LIBBMDUTILS_SCOPE long bmdGetExistingSemaphore(const key_t semaphoreKey, int* semId);
	LIBBMDUTILS_SCOPE long bmdLockSemaphore(const int semId);
	LIBBMDUTILS_SCOPE long bmdUnlockSemaphore(const int semId);
	/*
	long bmdCreateShmem(int *shmemId, const long sizeInBytes);
	long bmdRemoveShmem(int *shmemId);
	*/
	LIBBMDUTILS_SCOPE long bmdAllocShmem(const long sizeInBytes, void** shmemPtr, int* shmemId);
	LIBBMDUTILS_SCOPE long bmdFreeShmem(void** shmemPtr, int *shmemId);
#endif /* ifndef WIN32 */
	/* </bmd_ipc.c> */

#if defined(__cplusplus)
}
#endif
#endif
