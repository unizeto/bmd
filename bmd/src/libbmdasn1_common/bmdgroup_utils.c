#include <bmd/libbmdasn1_common/bmdgroup_utils.h>
#include <bmd/libbmderr/libbmderr.h>

int ____bmdgroup_create_bmdGroupPair(	client_group_pair_t *client_single_pair,
							BMDGroupPair_t **group_pair)
{
	(*group_pair)=(BMDGroupPair_t *)malloc(sizeof(BMDGroupPair_t));
	memset(*group_pair,0,sizeof(BMDGroupPair_t));

	BMD_FOK(OCTET_STRING_fromBuf(&((*group_pair)->parentGroup),client_single_pair->parent_group,(int)strlen(client_single_pair->parent_group)));
	BMD_FOK(OCTET_STRING_fromBuf(&((*group_pair)->childGroup),client_single_pair->child_group,(int)strlen(client_single_pair->child_group)));
// 	BMD_FOK(OCTET_STRING_fromBuf(&((*group_pair)->childGroupOID),client_single_pair->child_group_oid,(int)strlen(client_single_pair->child_group_oid)));

	return BMD_OK;
}

int ___bmdgroup_create_bmdGroupPairList(	client_group_pair_t *client_pair,
							BMDGroupPairList_t **group_pair_list)
{
int i					= 0;
int status				= 0;
BMDGroupPair_t *group_pair	= NULL;

	if(client_pair==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	(*group_pair_list)=(BMDGroupPairList_t *)malloc(sizeof(BMDGroupPairList_t));
	memset(*group_pair_list,0,sizeof(BMDGroupPairList_t));

	while(client_pair[i].child_group!=NULL)
	{
		status=____bmdgroup_create_bmdGroupPair(&(client_pair[i]),&group_pair);
		asn_set_add(&((*group_pair_list)->list), group_pair);
		i++;
	}

	return BMD_OK;
}

int __bmdgroup_create_levels(	client_group_pair_t **client_levels,
					BMDGroup__levels_t **levels)
{
int i						= 0;
int status					= 0;
BMDGroupPairList_t *group_pair_list	= NULL;

	if(client_levels)
	{
		(*levels)=(BMDGroup__levels_t *)malloc(sizeof(BMDGroup__levels_t));
		memset(*levels,0,sizeof(BMDGroup__levels_t));

		while(client_levels[i])
		{
			status=___bmdgroup_create_bmdGroupPairList(client_levels[i],&group_pair_list);
			asn_set_add(&((*levels)->list), group_pair_list);
			i++;
		}
	}

	return BMD_OK;
}

int _bmdgroup_create(	client_group_info_t *client_gi,
				BMDGroup_t **bmd_group)
{
/*int status			= 0;*/

	if(client_gi==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	/* bmd_group nie sprawdzamy poniewaz zwalnia sie ja pozniej przy asn_DEF_BMDGroups.free_struct(...) - podpinane sa wskazniki */
	(*bmd_group)=(BMDGroup_t *)malloc(sizeof(BMDGroup_t));
	memset(*bmd_group,0,sizeof(BMDGroup_t));

	BMD_FOK(OCTET_STRING_fromBuf(&((*bmd_group)->groupName),client_gi->group_name,(int)strlen(client_gi->group_name)));
// 	BMD_FOK(OCTET_STRING_fromBuf(&((*bmd_group)->groupNameOID),client_gi->group_name_oid,(int)strlen(client_gi->group_name_oid)));

	if(client_gi->levels)
	{
		__bmdgroup_create_levels(client_gi->levels,&((*bmd_group)->levels));
	}
	else
	{
		(*bmd_group)->levels=NULL;
	}

	return BMD_OK;
}

int bmdgroups_create(	client_group_info_t **client_gi,
				GenBuf_t **der_client_gi)
{
int i				= 0;
int status			= 0;
BMDGroups_t *bmd_groups	= NULL;
BMDGroup_t *bmd_group	= NULL;

	if(client_gi==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(der_client_gi==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*der_client_gi)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	bmd_groups=(BMDGroups_t *)malloc(sizeof(BMDGroups_t));
	memset(bmd_groups,0,sizeof(BMDGroups_t));

	while(client_gi[i])
	{
		status=_bmdgroup_create(client_gi[i],&bmd_group);
		asn_set_add(&(bmd_groups->list), bmd_group);
		i++;
	}

	BMD_FOK(asn1_encode(&asn_DEF_BMDGroups, bmd_groups, NULL, der_client_gi));

	asn_DEF_BMDGroups.free_struct(&asn_DEF_BMDGroups,bmd_groups,0);bmd_groups=NULL;

	return BMD_OK;
}

int OctetString2char(	OCTET_STRING_t *oc,
				char **output)
{
	(*output)=(char *)malloc(oc->size+2);
	memset(*output,0,oc->size+1);
	memmove(*output,oc->buf,oc->size);

	return BMD_OK;
}

int bmdgroups_decode(	GenBuf_t *der_client_gi,
				client_group_info_t **client_gi)
{
BMDGroups_t *bmd_groups			= NULL;
BMDGroup_t *bmd_group			= NULL;
BMDGroupPairList_t *bmd_group_list	= NULL;
BMDGroupPair_t *bmd_group_pair	= NULL;

asn_dec_rval_t rc_code;
int client_gi_count			= 0;
int levels_count				= 0;
int single_level_count			= 0;
int i						= 0;
int j						= 0;
int k						= 0;
/*int status					= 0;*/

	rc_code=ber_decode(0,&asn_DEF_BMDGroups,(void **)&bmd_groups,der_client_gi->buf,der_client_gi->size);
	if(rc_code.code!=RC_OK)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	/* zaalokuj pamiec na strukture client_gi oraz na wszystkie jej skladniki */
	client_gi_count=bmd_groups->list.count;

	(*client_gi)=(client_group_info_t *)malloc(sizeof(client_group_info_t)*(client_gi_count+2));
	memset(*client_gi,0,sizeof(client_group_info_t)*(client_gi_count+1));

	for(i=0;i<client_gi_count;i++)
	{
		bmd_group=bmd_groups->list.array[i];

		if(bmd_group->levels)
		{
			levels_count=bmd_group->levels->list.count;
			(*client_gi)[i].levels=(client_group_pair_t **)malloc(sizeof(client_group_pair_t *)*(levels_count+2));
			memset((*client_gi)[i].levels,0,sizeof(client_group_pair_t *)*(levels_count+1));

			for(j=0;j<levels_count;j++)
			{
				bmd_group_list=bmd_group->levels->list.array[j];
				single_level_count=bmd_group_list->list.count;

				(*client_gi)[i].levels[j]=(client_group_pair_t *)malloc(sizeof(client_group_pair_t)*(single_level_count+2));
				memset((*client_gi)[i].levels[j],0,sizeof(client_group_pair_t)*(single_level_count+1));
			}
		}
		else
		{
			(*client_gi)->levels=NULL;
		}
	}

	/* wpisz wartosci do client_gi */
	for(i=0;i<client_gi_count;i++)
	{
		bmd_group=bmd_groups->list.array[i];

		BMD_FOK(OctetString2char(&(bmd_group->groupName),&( ( (*client_gi)[i] ).group_name ) ));
		BMD_FOK(OctetString2char(&(bmd_group->groupNameOID),&( ( (*client_gi)[i] ).group_name_oid ) ));

		if(bmd_group->levels)
		{
			levels_count=bmd_group->levels->list.count;

			for(j=0;j<levels_count;j++)
			{
				bmd_group_list=bmd_group->levels->list.array[j];
				single_level_count=bmd_group_list->list.count;

				for(k=0;k<single_level_count;k++)
				{
					bmd_group_pair=bmd_group_list->list.array[k];
					BMD_FOK(OctetString2char(&(bmd_group_pair->parentGroup),&( ( (*client_gi)[i] ).levels[j][k].parent_group ) ));
					BMD_FOK(OctetString2char(&(bmd_group_pair->childGroup),&( ( (*client_gi)[i] ).levels[j][k].child_group ) ));
					BMD_FOK(OctetString2char(&(bmd_group_pair->childGroupOID),&( ( (*client_gi)[i] ).levels[j][k].child_group_oid ) ));
				}
			}
		}
		else
		{
			(*client_gi)->levels=NULL;
		}
	}

	asn_DEF_BMDGroups.free_struct(&asn_DEF_BMDGroups,bmd_groups,0);bmd_groups=NULL;

	return BMD_OK;
}

