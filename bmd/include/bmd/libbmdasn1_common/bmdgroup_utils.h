#include <bmd/libbmdasn1_common/BMDGroup.h>
#include <bmd/libbmdasn1_common/BMDGroups.h>
#include <bmd/libbmdasn1_common/BMDGroupPair.h>
#include <bmd/libbmdasn1_common/BMDGroupPairList.h>

#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#if defined(__cplusplus)
extern "C" {
#endif
/* tworzy zakodowana w der postac client_group_info_t */
int bmdgroups_create(client_group_info_t **client_gi,GenBuf_t **der_client_gi);

/* dekoduje DER grup double struktury client_group_info_t */
int bmdgroups_decode(GenBuf_t *der_client_gi,client_group_info_t **client_gi);

#if defined(__cplusplus)
}
#endif
