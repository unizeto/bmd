#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

long bmd_attr_get_oid(	long nr,
			BMD_attr_t **form,
			char **oid)
{
long i		= 0;
long count	= 0;

	PRINT_INFO("LIBBMDINF Getting attribute oid\n");
	if (nr<0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (form==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}


	i=0;
	while(form[i])
	{
		count++;
		i++;
	}

	if(count==0)		{	BMD_FOK(BMD_ERR_FORMAT);	}
	if(nr>count)		{	BMD_FOK(BMD_ERR_OVERFLOW);	}
	if(oid==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*oid)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	(*oid)=(char *)malloc(sizeof(char)*(form[nr]->oid->size+1));
	memset((*oid), 0, sizeof(char)*(form[nr]->oid->size+1));
	memcpy((*oid), form[nr]->oid->buf, sizeof(char)*(form[nr]->oid->size+1));

	return BMD_OK;
}

long bmd_attr_get_description(	long nr,
				BMD_attr_t **form,
				char **description)
{
long i		= 0;
long count	= 0;

	PRINT_INFO("LIBBMDINF Getting attribute description\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(nr<0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(form==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	i=0;
	while(form[i])
	{
		count++;
		i++;
	}

	if(count==0)			{	BMD_FOK(BMD_ERR_FORMAT);	}
	if(nr>count)			{	BMD_FOK(BMD_ERR_OVERFLOW);	}
	if(description==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*description)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(genbuf_to_char(form[nr]->label,description));

	return BMD_OK;
}
