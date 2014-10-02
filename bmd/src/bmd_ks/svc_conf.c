/* wczytywanie konfiguracji uslugi - obecnie nastepujace parametry */
#include <bmd/bmd_ks/bmd_ks.h>

long _svc_conf_get_value(const char *regexp,char *buf,long length,char **value)
{
    pcre *rc=NULL;
    int ovector[6];
    const char *error=NULL;
    int erroroffset;
    char *tmp=NULL;
    long len,re;
    
    if( regexp == NULL )
        return SVC_ERROR_PARAM;
    if( buf == NULL )
        return SVC_ERROR_PARAM;
    if( value == NULL )
        return SVC_ERROR_PARAM;
    if( (*value) != NULL )
        return SVC_ERROR_PARAM;
    
    rc=pcre_compile(regexp,PCRE_MULTILINE,&error,&erroroffset,NULL);
    if( rc == NULL )
        return SVC_ERROR_FAILED;
    
    re = pcre_exec(rc,NULL,(const char *)buf,length,0,0,ovector,6);
    if( re < 0 )
        return SVC_ERROR_FAILED;
    
    len=pcre_get_substring((char *)buf,ovector,re,1,(const char **)&tmp);
    if( ( tmp != NULL ) && ( len > 0 ) )
    {
        (*value)=(char *)malloc(strlen(tmp)+2);
        if( (*value) == NULL )
            return SVC_ERROR_MEM;
        memset(*value,0,strlen(tmp)+1);
        memmove(*value,tmp,strlen(tmp));
    }
    pcre_free_substring(tmp);tmp=NULL;
    pcre_free(rc);rc=NULL;
	        
    return SVC_OK;
}

long svc_load_conf(char * conf_file,svc_conf_t **svc_conf)
{
	#ifdef WIN32
    HANDLE hFile;
    BOOL res;
    DWORD size,nread;
	#else
	long fd;
	long nread;
	long size;
	struct stat file_info;
	#endif
	
    char *buf=NULL;
    long status,i;
    char *tmp=NULL;
    
    if( conf_file == NULL )
        return SVC_ERROR_PARAM;
    if( svc_conf == NULL )
        return SVC_ERROR_PARAM;
    if( (*svc_conf) != NULL )
        return SVC_ERROR_PARAM;

	#ifdef WIN32
    hFile=CreateFileA(conf_file,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if( hFile == INVALID_HANDLE_VALUE )
        return SVC_ERROR_DISK;

    size=GetFileSize(hFile,NULL);
    if( size <= 0 )
        return SVC_ERROR_DISK;
	#else
	memset(&file_info,0,sizeof(struct stat));
	fd=(long)open(conf_file,O_RDONLY);
	if( fd < 0 )
		return SVC_ERROR_DISK;
	status=(long)fstat((int)fd,&file_info);
	if( status != 0 )
		return SVC_ERROR_DISK;
	size=(long)file_info.st_size;
	#endif
    buf=(char *)malloc(size);
    if( buf == NULL )
        return SVC_ERROR_MEM;
    
	#ifdef WIN32
    res=ReadFile(hFile,buf,size,&nread,NULL);
    if( res == 0 )
        return SVC_ERROR_DISK;
	#else
	nread=(long)read(fd,buf,size);
	#endif
    if( nread != size )
        return SVC_ERROR_DISK;
    
    /* tutaj odczytanie konfiguracji z pliku - na razie wartosci sa na sztywno ustawiane */
    (*svc_conf)=(svc_conf_t *)malloc(sizeof(svc_conf_t));
    if( (*svc_conf) == NULL )
        return SVC_ERROR_MEM;
    memset(*svc_conf,0,sizeof(svc_conf_t));
    
    for(i=0; i< (long)size; i++)
    {
        if( buf[i] == '\r' )
            buf[i]='\n';
    }

    status=_svc_conf_get_value("^adr=(.+)$",buf, size,&((*svc_conf)->adr));
    if( status != SVC_OK )
        return SVC_ERROR_FORMAT;
    status=_svc_conf_get_value("^port=(.+)$",buf, size,&tmp);
    if( status == SVC_OK )
        (*svc_conf)->port=atoi(tmp);    
    else
        return SVC_ERROR_FORMAT;
    free0(tmp);
    status=_svc_conf_get_value("^sig_key_id=(.+)$",buf, size,&((*svc_conf)->sig_key_id));
    if( status != SVC_OK )
        return SVC_ERROR_FORMAT;
    status=_svc_conf_get_value("^dec_key_id=(.+)$",buf,size,&((*svc_conf)->dec_key_id));
    if( status != SVC_OK )
        (*svc_conf)->dec_key_id=NULL;
    status=_svc_conf_get_value("^pin=(.+)$",buf,size,&((*svc_conf)->pin));
    if( status != SVC_OK )
        (*svc_conf)->pin=NULL;
    status=_svc_conf_get_value("^pkcs11_lib=(.+)$",buf,size,&((*svc_conf)->pkcs11_lib));
    if( status != SVC_OK )
        return SVC_ERROR_FORMAT;
    status=_svc_conf_get_value("^max_threads=(.+)$",buf,size,&tmp);
    if( status != SVC_OK )
        (*svc_conf)->max_threads=5;
    else
    {
        (*svc_conf)->max_threads=atoi(tmp);
    }
	free0(tmp);
	status=_svc_conf_get_value("^log_level=(.+)$",buf,size,&tmp);
    if( status != SVC_OK )
        return SVC_ERROR_FORMAT;
	if( strcmp(tmp,"DEBUG") == 0 )
		(*svc_conf)->log_level=SVC_LOG_LEVEL_DEBUG;
	else
		if( strcmp(tmp,"NORMAL") == 0 )
			(*svc_conf)->log_level=SVC_LOG_LEVEL_NORMAL;
		else
			if( strcmp(tmp,"NONE") == 0 )
				(*svc_conf)->log_level=SVC_LOG_LEVEL_NONE;
			else
				return SVC_ERROR_FORMAT;
    free0(tmp);
	status=_svc_conf_get_value("^timeout_sec=(.+)$",buf,size,&tmp);
	if( status != SVC_OK )
		return SVC_ERROR_FORMAT;
	(*svc_conf)->timeout_sec=atoi(tmp);
	free0(tmp);
#ifndef WIN32
	status=_svc_conf_get_value("^log_dir=(.+)$",
                                   buf,size,&((*svc_conf)->log_dir));
	if( status != SVC_OK )
		return SVC_ERROR_FORMAT;
#endif
	#ifdef WIN32
    	CloseHandle(hFile);
	#else
	close(fd);
	#endif
    
    return 0;
}
