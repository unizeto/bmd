#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <errno.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#include <windows.h>
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
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
		return TRUE;
}
#endif

long bmdconf_init(bmd_conf **struktura)
{
	PRINT_INFO("BMDCONFIGINF Initializing bmd configuration\n");
	if (!(struktura != NULL))
	{
		abort();
	}
	(*struktura)=(bmd_conf*)malloc(sizeof(bmd_conf));
	bmdconf_init_dictionary_config(&( (*struktura)->dict ));
	bmdconf_init_dump_config(&( (*struktura)->dump ));
	(*struktura)->tryb=BMDCONF_RESTRICT;
	(*struktura)->file_name=NULL;

	return BMD_OK;
}

long bmdconf_destroy(	bmd_conf **struktura)
{
	PRINT_INFO("BMDCONFIGINF Destroying bmd configuration\n");
	if (!(struktura != NULL))
	{
		PRINT_ERROR("LIBBMDCONFIGERR Empty bmd_conf structure\n");
		abort();
	}

	bmdconf_delete_dictionary_config(&( (*struktura)->dict ));
	bmdconf_delete_dump_config(&( (*struktura)->dump ));
	free( (*struktura)->file_name );
	free(*struktura);
	/*prawidlowa funkcja niszczaca powinna NULLowac wskaznik - nie wiem, czemu wczesniej tego nie zrobilem ...*/
	*struktura=NULL;

	return BMD_OK;
}

long bmdconf_load_file(	const char *file_path,
				bmd_conf *struktura,
				char **error_string)
{
FILE *conf_file = NULL;
char *ret = NULL;

	if(!(file_path != NULL
		&& file_path[0] != '\0'))			{	BMD_FOK(LIBBMDCONFIG_LIBBMDCONFIG_INVALID_CONF_FILE_PATH);	}
	if (!(struktura != NULL))				{	BMD_FOK(LIBBMDCONFIG_LIBBMDCONFIG_EMPTY_CONF_STRUCTURE);	}

	PRINT_INFO("BMDCONFIGINF Loading bmd configuration file %s\n",file_path);
	if (error_string != NULL)
	{
		*error_string=NULL;
	}

	if((conf_file=fopen(file_path,"r"))==NULL)	{	BMD_FOK(LIBBMDCONFIG_LIBBMDCONFIG_OPEN_CONF_FILE_ERR);	}

	struktura->file_name=strdup(file_path);

	ret=bmdconf_read_and_parse_config(conf_file,&(struktura->dump));
	if (ret!=NULL)
	{
		fclose(conf_file);
		if(error_string != NULL)
			*error_string = ret;
		else
		{
			free(ret);
		}
		if( error_string != NULL )
		{
			PRINT_TEST("ERROR: %s\n",(*error_string));
		}
		BMD_FOK(LIBBMDCONFIG_LIBBMDCONFIG_PARSE_CONF_FILE_ERR);
	}

	ret=bmdconf_validate_config(struktura);
	if(ret!=NULL)
	{
		fclose(conf_file);
		if(error_string != NULL)
			*error_string = ret;
		else
		{

			free(ret);
		}
		bmdconf_delete_list(&(struktura->dump)); /*niszczenie dumpa pliku konfiguracyjnego , gdy niepowodzenie walidacji*/
		if( error_string != NULL )
		{
			PRINT_TEST("ERROR: %s\n",(*error_string));
		}

		BMD_FOK(LIBBMDCONFIG_LIBBMDCONFIG_VALIDATE_CONF_FILE_ERR); /*blad przy walidacji pliku konfigracyjnego*/
	}

	if( (fclose(conf_file)) != 0 )			{	BMD_FOK(LIBBMDCONFIG_LIBBMDCONFIG_CLOSE_CONF_FILE_ERR);	}

	/*sukces*/
	return BMD_OK;
}

/*@added long czy_wymagana*/
long bmdconf_add_section2dict(	bmd_conf *struktura,
					const char *section_name,
					const char **registers,
					long czy_wymagana)
{
char *temp;

	if (!(struktura != NULL))					{	abort();	}
	if(!(section_name != NULL && section_name[0] != '\0')){	abort();	}
	if(!(registers != NULL))					{	abort();	}
	PRINT_INFO("BMDCONFIGINF Adding section %s to bmd configuration\n",section_name);

	temp=bmdconf_load_dictionary_config(&(struktura->dict), (char*) section_name, (char**)registers, czy_wymagana);
	if(temp!=NULL)
	{
		PRINT_ERROR("LIBBMDCONFIGERR Error %s\n",temp);
		free(temp);
		abort();
	}

	return BMD_OK;

}

long bmdconf_get_value(	bmd_conf *struktura,
				const char *section_name,
				const char *option_name,
				char **result)
{
	if (struktura == NULL)			{	abort();	}
	if(!(section_name != NULL &&
		section_name[0] != '\0'))	{	abort();	}
	if(!(option_name != NULL &&
		option_name[0] != '\0'))	{	abort();	}
	if(!(result != NULL))			{	abort();	}
	PRINT_INFO("BMDCONFIGINF Getting bmd configuration value of option %s\n",option_name);

	*result=bmdconf_get_value_config(&(struktura->dump),(char *)section_name,(char *)option_name);
	if((*result)==NULL)
	{
		PRINT_TEST("error in getting conf setting: section: %s   option: %s\n", section_name, option_name);
		PRINT_DEBUG("error in getting conf setting: section: %s   option: %s\n", section_name, option_name);
		return LIBBMDCONFIG_LIBBMDCONFIG_GET_CONFIG_VALUE_ERR;
	}

	return BMD_OK;
}

/*wyniku nie trzeba zwalniac - wskaznik bezposrenio do stringa w strukturze bmd_conf*/
const char* bmdconf_get_value_static(	bmd_conf *struktura,
					const char *section_name,
					const char *option_name)
{
const char *temp;

	if(struktura == NULL)
	{
		PRINT_ERROR("LIBBMDCONFIGERR Empty bmd_conf structure.\n");
		return NULL;
	}

	if(section_name == NULL)
	{
		PRINT_ERROR("LIBBMDCONFIGERR Empty section name.\n");
		return NULL;
	}
	if(section_name[0] == '\0')
	{
		PRINT_ERROR("LIBBMDCONFIGERR Empty string in section name.\n");
		return NULL;
	}

	if(option_name == NULL)
	{
		PRINT_ERROR("LIBBMDCONFIGERR Empty option name.\n");
		return NULL;
	}
	if(option_name[0] == '\0')
	{
		PRINT_ERROR("LIBBMDCONFIGERR Empty string in option name.\n");
		return NULL;
	}
	PRINT_INFO("BMDCONFIGINF Getting bmd configuration static value of option %s\n",option_name);
#ifndef WIN32
	/*bmd_assert(struktura != NULL,"WS");
	bmd_assert(section_name != NULL && section_name[0] != '\0',"WS");
	bmd_assert(option_name != NULL && option_name[0] != '\0',"WS");*/
#else
	bmd_assert_ptr(struktura != NULL,"WS");
	bmd_assert_ptr(section_name != NULL && section_name[0] != '\0',"WS");
	bmd_assert_ptr(option_name != NULL && option_name[0] != '\0',"WS");
#endif
	temp=bmdconf_get_value_static_config(&(struktura->dump),(char *)section_name,(char *)option_name);
	/*NULL jesli nie znaleziono, wskaznik do stringa jesli ok.*/
	return temp;
}


/*result - adres longegera*/
long bmdconf_get_value_int(	bmd_conf *struktura,
					const char *section_name,
					const char *option_name,
					long *result)
{
char *temp;

	if (!(struktura != NULL))			{	abort();	}
	if(!(section_name != NULL &&	section_name[0] != '\0'))		{	abort();	}
	if(!(option_name != NULL &&	option_name[0] != '\0'))		{	abort();	}
	if(!(result != NULL))				{	abort();	}

	PRINT_INFO("BMDCONFIGINF Getting bmd configuration long value of option %s\n",option_name);
	*result = -1;
	temp=bmdconf_get_value_config(&(struktura->dump),(char *)section_name,(char *)option_name);
	if(temp==NULL)
	{
		PRINT_TEST("error in getting conf setting: section: %s   option: %s\n", section_name, option_name);
		PRINT_DEBUG("error in getting conf setting: section: %s   option: %s\n", section_name, option_name);
		return LIBBMDCONFIG_LIBBMDCONFIG_GET_CONFIG_VALUE_ERR;
	}
	else
	{
		*result=atoi(temp);
		free(temp);
	}

	return BMD_OK;
}

long bmdconf_set_mode(	bmd_conf* struktura,
				long flag)
{
	PRINT_INFO("BMDCONFIGINF Setting bmd configuration mode\n");
	if (!(struktura != NULL))			{	abort();	}

	struktura->tryb=flag;

	return BMD_OK;
}
