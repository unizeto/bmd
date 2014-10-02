//gsoap bmd230 service location: https://127.0.0.1:9999
//gsoap bmd230 service namespace: bmd230.xsd

#import "wsse.h"

typedef int    xsd__int;
typedef char  *xsd__string;

struct xsd__base64Binary {
	unsigned char *__ptr;
	int __size;
};

struct bmd230__myBinaryDataType
{
   unsigned char* __ptr;
   int __size;
   char *id;
   char *type;
   char *options;
};

struct bmd230__LongNumbersList
{
	long*	__ptr;
	int		__size;
};

/****************************************/
/*	pojedyncze wartosci dowodowe	*/
/****************************************/
struct bmd230__TimestampSingle {
	struct xsd__base64Binary *value;
};

struct bmd230__DVCSSingle {
	struct xsd__base64Binary *value;
	struct bmd230__TimestampList *timestamp;
};

struct bmd230__SignatureSingle {
	struct xsd__base64Binary *value;
	struct bmd230__TimestampList *timestamp;
	struct bmd230__DVCSList *dvcs;
};

/****************************************/
/*	listy wartosci dowodowych	*/
/****************************************/
struct bmd230__TimestampList {
	int __size;
	struct bmd230__TimestampSingle *__ptr;
};

struct bmd230__DVCSList {
	int __size;
	struct bmd230__DVCSSingle *__ptr;
};

struct bmd230__SignatureList {
	int __size;
	struct bmd230__SignatureSingle *__ptr;
};

/********************************************************/
/*	struktura ze wszystkimi wartosciami dowodowymi	*/
/********************************************************/
struct bmd230__PkiFileInfo {
	xsd__string filename;
	struct bmd230__TimestampList *timestamp;
	struct bmd230__SignatureList *signature;
};


/************************************************/
/*	plik, metadane pliku, wszystko razem	*/
/************************************************/
struct bmd230__fileInfo {
    struct bmd230__myBinaryDataType *file;
    xsd__string filename;
};

struct bmd230__fileInfoList
{
	struct bmd230__fileInfo *__ptr;
	int __size;
};


struct bmd230__mtdSingleValue
{
	xsd__string mtdOid;
	xsd__string mtdValue;
	xsd__string mtdDesc;
};

struct bmd230__mtdsValues
{
    struct bmd230__mtdSingleValue *__ptr;
    int __size;
};

/******************************************/
/*	plik wraz ze swoimi metadanymi	*/
/******************************************/
struct bmd230__fileComplex {
	struct bmd230__mtdsValues *mtds;
	struct bmd230__fileInfo *file;
};

/******************************************/
/*	lista plikow wraz z ich metadanymi	*/
/******************************************/
struct bmd230__fileComplexList
{
	struct bmd230__fileComplex *__ptr;
	int __size;
};

struct bmd230__idList
{
	int *__ptr;
	int __size;
};

struct bmd230__searchSortInfo
{
	xsd__string sortOid;
	xsd__string sortOrder;
};

struct bmd230__searchSortInfoList
{
	struct bmd230__searchSortInfo *__ptr;
	int __size;
};

struct bmd230__mtdSingleInfo
{
    xsd__string mtdOid;
    xsd__string mtdDesc;
   };

struct bmd230__mtdsInfo
{
    struct bmd230__mtdSingleInfo *__ptr;
    int __size;
};

struct bmd230__searchSingleResult
{
	struct bmd230__mtdsValues *mtds;
	xsd__string filename;
	int long id;
};

struct bmd230__searchResults
{
	struct bmd230__searchSingleResult *__ptr;
	int __size;
};

/***********************************/
/* struktura wyniku historycznosci */
/***********************************/
struct bmd230__historySingleResult
{
	struct bmd230__mtdsValues *mtds;
	xsd__string updateDate;
	xsd__string updateOwner;
	xsd__string updateReason;
};

struct bmd230__historyResults
{
    struct bmd230__historySingleResult *__ptr;
    int __size;
};

/******************************/
/*	struktura timestampa	*/
/******************************/
struct bmd230__bmdTimestamp
{
	struct xsd__base64Binary *timestamp;
	char *genTime;
};

struct bmd230__mtdSearchSingleValue
{
	xsd__string mtdOid;
	xsd__string mtdValue;
	xsd__string mtdDesc;
	xsd__string mtdLogicalOperator;
	int noOfOpenBrackets;
	int noOfCloseBrackets;
	xsd__string mtdAdjustment;
	xsd__string mtdStatement;
};

struct bmd230__mtdsSearchValues
{
    struct bmd230__mtdSearchSingleValue *__ptr;
    int __size;
};


/****************************/
/* informacje o użytkowniku */
/****************************/
struct bmd230__stringList
{
	xsd__string *__ptr;
	int __size;
};

struct bmd230__userNewGroupInfo
{
	xsd__string userNewGroupName;
	struct bmd230__stringList *userNewGroupParentIds;
	struct bmd230__stringList *userNewGroupChildIds;

};

struct bmd230__singleUserInfo
{
	xsd__string userId;
	xsd__string userCertIssuer;
	xsd__string userCertSerial;
	xsd__string userName;
	xsd__string userIdentity;
	xsd__string userAccepted;
	struct bmd230__stringList *userRoleIdList;
	struct bmd230__stringList *userRoleNameList;
	struct bmd230__stringList *userGroupIdList;
	struct bmd230__stringList *userGroupNameList;
	struct bmd230__stringList *userClassIdList;
	struct bmd230__stringList *userClassNameList;
	struct bmd230__stringList *userSecurityNameList;
	xsd__string userDefaultRoleId;
	xsd__string userDefaultRoleName;
	xsd__string userDefaultGroupId;
	xsd__string userDefaultGroupName;
	xsd__string userDefaultClassId;
	xsd__string userDefaultClassName;
	struct bmd230__userNewGroupInfo *userNewGroupInfo;
};

struct bmd230__userListInfo
{
	struct bmd230__singleUserInfo *__ptr;
	int __size;
};

struct bmd230__userRegistrationResult
{
	long userIdentityId;
	long userCertificateId;
};

/**************************/
/* odpowiedz na logowanie */
/**************************/
struct bmd230__loginResponse
{
	struct bmd230__singleUserInfo *userInfo;

	struct bmd230__mtdsInfo *formSearch;
	struct bmd230__mtdsInfo *formSend;
	struct bmd230__mtdsInfo *formHistory;
	struct bmd230__mtdsInfo *formUpdate;
};

/*********************/
/* informacje o roli */
/*********************/
struct bmd230__singleRoleInfo
{
	xsd__string roleId;
	xsd__string roleName;
	struct bmd230__stringList *roleActionList;
	struct bmd230__stringList *roleRightsOids;
	struct bmd230__stringList *roleRightsCodes;
};

struct bmd230__userRoleInfo
{
	struct bmd230__singleRoleInfo *__ptr;
	int __size;
};

/*********************/
/* informacje o grupie */
/*********************/
struct bmd230__singleGroupInfo
{
	xsd__string groupId;
	xsd__string groupName;
	struct bmd230__stringList *groupParentNames;
	struct bmd230__stringList *groupParentIds;
	struct bmd230__stringList *groupChildNames;
	struct bmd230__stringList *groupChildIds;
};

struct bmd230__groupInfo
{
	struct bmd230__singleGroupInfo *__ptr;
	int __size;
};

/*********************/
/* informacje o klasie */
/*********************/
struct bmd230__singleClassInfo
{
	xsd__string classId;
	xsd__string className;
};

	struct bmd230__classInfo
{
	struct bmd230__singleClassInfo *__ptr;
	int __size;
};

/*****************************************/
/* informacje o kategorii bezpieczeństwa */
/*****************************************/
struct bmd230__singleSecCategoryInfo
{
	xsd__string secCategoryName;
	xsd__string secCategoryId;
	struct bmd230__stringList *secCategoryParentNames;
	struct bmd230__stringList *secCategoryChildNames;
};

struct bmd230__secCategoryInfo
{
	struct bmd230__singleSecCategoryInfo *__ptr;
	int __size;
};

/****************************************/
/* informacje o poziomie bezpieczeństwa */
/****************************************/
struct bmd230__singleSecLevelInfo
{
	xsd__string secLevelName;
	xsd__string secLevelId;
	xsd__string secLevelPriority;
};

struct bmd230__secLevelInfo
{
	struct bmd230__singleSecLevelInfo *__ptr;
	int __size;
};

struct bmd230__securityInfo
{
	struct bmd230__secCategoryInfo *secCategoryInfo;
	struct bmd230__secLevelInfo *secLevelInfo;
};

struct bmd230__singleLinkInfo
{
	char* pointingId;
	char* ownerIdentityId;
	char* ownerCertificateId;
	char* ownerGroupId;
	char* linkName;
	char* linkDescription;
	enum grantingType_e granting;
	struct bmd230__DateTime* expirationDate;
};

struct bmd230__linksInfo
{
	struct bmd230__singleLinkInfo *__ptr;
	int __size;
};

struct bmd230__singleCascadeLinkInfo
{
	struct bmd230__singleLinkInfo *linkInfo;
	long correspondingFilesMaxLevel;
	enum visibilityType_t visible;
};

struct bmd230__cascadeLinksInfo
{
	struct bmd230__singleCascadeLinkInfo *__ptr;
	int __size;
};

struct bmd230__manyCascadeLinksElement
{
	struct bmd230__idList* idsList;
	long errorCode;
};


struct bmd230__manyCascadeLinksResult
{
	struct bmd230__manyCascadeLinksElement *__ptr;
	int __size;
};

struct bmd230__deleteCascadeLinksElement
{
	char* fileId;
	char* ownerIdentityId;
	char* ownerCertificateId;
	long correspondingFilesMaxLevel;
	enum visibilityType_t visible;
};

struct bmd230__deleteCascadeLinksList
{
	struct bmd230__deleteCascadeLinksElement *__ptr;
	int __size;	
};

/*************************************************************/
/* Wybrany rodzaj transmisji : nostream - 0, stream( DIME) 1 */
/*************************************************************/
enum trans_t { nostream, stream };


/**************************************/
/* struktura do obsługi strumieniowej */
/* transmisji paczki archiwalnej      */
/**************************************/
struct bmd230__archPackOut
{

    struct xsd__base64Binary *file;
    xsd__string filename;

    char* tmpLocation;
    char* pathDestZip;
    char* currDir;
    int long pos;
};


/* Sterowanie transmisją  : start - 0, iter 1 */
/*************************************************************/

  enum chunkCntr_t { start, iter };



struct bmd230__accessObject
{
	xsd__string id;
	xsd__string ownerGroupId;
	xsd__string fileType;
	xsd__string ownerIdentityId;
	xsd__string creatorIdentityId;
};

struct bmd230__accessObjectList
{
	struct bmd230__accessObject* __ptr;
	int __size;
};

struct bmd230__DateTime
{
	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;
};


struct bmd230__FileVersionInfo
{
	long id;
	xsd__string insertDate;
};

struct bmd230__FileVersionInfoList
{
	struct bmd230__FileVersionInfo* __ptr; 
	int __size;
};

struct bmd230__GetVersionsHistoryResult
{
	struct bmd230__FileVersionInfoList* versionsInfoList;
	long errorCode;
};

struct bmd230__GetVersionsHistoryResultList
{
	struct bmd230__GetVersionsHistoryResult* __ptr;
	int __size;
};


struct bmd230__ActionsHistoryElement
{
	xsd__string objectId;
	xsd__string objectType;
	xsd__string executorIdentityName;
	xsd__string executorClassName;
	xsd__string actionName;
	xsd__string actionDate;
	xsd__string actionStatus;
	xsd__string actionStatusMessage;
};

struct bmd230__ActionsHistoryList
{
	struct bmd230__ActionsHistoryElement* __ptr;
	int __size;
};

enum visibilityType_t { VISIBLE=1, INVISIBLE=2, ALL=4};

enum groupAuthorizationType_e { WITH_DEPENDENT_GROUPS=0, ONLY_LOGIN_GROUP };

enum grantingType_e { GRANTING_FORBIDDEN=0, GRANTING_ALLOWED };

/************************************/
/* Funkcje nie objęte obsługą DIME  */
/************************************/
int bmd230__testConnection(char **result);
int bmd230__bmdInsertFile( 			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, struct bmd230__fileInfo *input, char *transactionId, enum trans_t type, long int *id);
int bmd230__bmdInsertSignedFile(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, struct bmd230__fileInfo *input, struct bmd230__myBinaryDataType *binarySignature, char* signatureType, char *transactionId, enum trans_t type, long int *id);
int bmd230__bmdInsertFileList(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__fileComplexList *input, char *transactionId, enum trans_t type, struct bmd230__idList **ids);
int bmd230__bmdUpdateFileVersion(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char* fileId, struct bmd230__fileInfo *input, enum trans_t type, long* updatedFileId);
int bmd230__bmdDeleteFileByID(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, /*char *ownerRange, */int *result);
int bmd230__bmdDeleteDirByID(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, int *result);
int bmd230__bmdUpdateMetadata(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, struct bmd230__mtdsValues *mtds, char *updateReason, int *result);
int bmd230__bmdUpdateSystemMetadata(struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, struct bmd230__mtdsValues *mtds, long *result);

int bmd230__bmdUpdateDefaultMetadataValue(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, int *result);
int bmd230__bmdAddSignature(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct xsd__base64Binary *signatureInput, long destFileId, long *result);
int bmd230__bmdLogin(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__loginResponse **loginResponse);
int bmd230__bmdSearchFiles(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, struct bmd230__mtdsValues *formMtds, int offset, int limit, char *logical, char *oper, char *range, char *ownerRange, struct bmd230__searchSortInfoList *sortInfo, struct bmd230__searchResults **searchResults);
int bmd230__bmdSearchFilesCSV(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, struct bmd230__mtdsValues *formMtds, int offset, int limit, char *logical, char *oper, char *ownerRange, struct bmd230__searchSortInfoList *sortInfo, struct bmd230__fileInfo **output);
int bmd230__bmdAdvancedSearchFiles(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsSearchValues *mtds, struct bmd230__mtdsValues *formMtds, int offset, int limit, char *range, char *ownerRange, struct bmd230__searchSortInfoList *sortInfo, struct bmd230__searchResults **searchResults);
int bmd230__bmdAdvancedCountFiles(		struct xsd__base64Binary *cert,	char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsSearchValues *mtds, char *ownerRange, long *result);
int bmd230__bmdHistory(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, struct bmd230__historyResults **historyResults);
int bmd230__bmdSearchUnreceived(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, int offset, int limit, struct bmd230__mtdsSearchValues *mtds, struct bmd230__mtdsValues *formMtds, struct bmd230__searchSortInfoList *sortInfo, struct bmd230__searchResults **searchResults);

/*****************************/
/* operacje na uzytkownikach */
/*****************************/
int bmd230__bmdRegisterNewUser(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__singleUserInfo *userAttributes, struct bmd230__userRegistrationResult** result);
int bmd230__bmdAddCertificateToIdentity(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *identityId, struct bmd230__singleUserInfo *userAttributes, int *result);
int bmd230__bmdUpdateUserRights(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *userId, struct bmd230__singleUserInfo *userAttributes, int *result);
int bmd230__bmdGetUsersList(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *userIdentityId, struct bmd230__userListInfo **result);
int bmd230__bmdDeleteUser(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *userId, int *result);
int bmd230__bmdDisableUser(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *userId, int enable, int *result);
int bmd230__bmdDisableIdentity(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *identityId, int disable, int *result);

/**********************/
/* operacje na rolach */
/**********************/
int bmd230__bmdRegisterNewRole(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__singleRoleInfo *roleInfo, int *result);
int bmd230__bmdUpdateRole(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *roleId, struct bmd230__singleRoleInfo *roleInfo, int *result);
int bmd230__bmdGetRolesList(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *roleId, struct bmd230__userRoleInfo **result);
int bmd230__bmdDeleteRole(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *roleId, int *result);

/***********************/
/* operacje na grupach */
/***********************/
int bmd230__bmdRegisterNewGroup(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__singleGroupInfo *groupInfo, int *result);
int bmd230__bmdUpdateGroup(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *groupId, struct bmd230__singleGroupInfo *groupInfo, int *result);
int bmd230__bmdGetGroupsList(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *groupId, struct bmd230__groupInfo **result);
int bmd230__bmdDeleteGroup(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *groupId, int *result);

/***********************/
/* operacje na klasach */
/***********************/
int bmd230__bmdRegisterNewClass(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__singleClassInfo *ClassInfo, int *result);
int bmd230__bmdUpdateClass(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *classId, struct bmd230__singleClassInfo *ClassInfo, int *result);
int bmd230__bmdGetClassList(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *classId, struct bmd230__classInfo **result);
int bmd230__bmdDeleteClass(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *classId, int *result);


/*******************************/
/* operacje na bezpieczeństwie */
/*******************************/
int bmd230__bmdRegisterNewSecurityCategory(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__singleSecCategoryInfo *secCategoryInfo, int *result);
//int bmd230__bmdRegisterNewSecurityLevel(		struct xsd__base64Binary *cert, struct bmd230__singleSecLevelInfo secLevelInfo, int *result);
int bmd230__bmdGetSecurityList(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__securityInfo **result);


/******************************************/
/*	pobieranie danych z bazy danych	*/
/******************************************/
int bmd230__bmdGetFileById(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, enum trans_t type, char* parameter, struct bmd230__fileInfo **output);
int bmd230__bmdGetArchiveFileVersion(struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long currentVersionFileId, long archiveVersionFileId, enum trans_t type, struct bmd230__fileInfo **output);
int bmd230__bmdGetFileByHash(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *hash, struct bmd230__fileInfo **output);
int bmd230__bmdGetFileByIdInZipPack(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, enum trans_t type, struct bmd230__fileInfo **output);
int bmd230__bmdGetFilePropByID(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id,int property,struct bmd230__fileInfo **output);
int bmd230__bmdGetFilePKIById(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id,struct bmd230__PkiFileInfo **output);
int bmd230__bmdGetTimeStamp(		struct bmd230__fileInfo *input,struct bmd230__bmdTimestamp **output);
int bmd230__bmdGetArchPack(                 struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, enum trans_t type, struct bmd230__fileInfo **output );
int bmd230__bmdGetAccessObjects(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int bmdId, enum visibilityType_t visible, struct bmd230__accessObjectList** accessObjects);
int bmd230__bmdGetVersionsHistory(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__LongNumbersList *fileIds, struct bmd230__GetVersionsHistoryResultList** result);
int bmd230__bmdGetActionsHistoryById(struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char* id, struct bmd230__ActionsHistoryList **output);

/************/
/* operacja */
/************/
int bmd230__getInvoicePackById(		struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, long int id, struct bmd230__fileInfo **output);
int bmd230__getHTMLImageById(		struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, long int id, struct bmd230__fileInfo **output);
int bmd230__verifyInvoiceById(		struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, long int id, int forceVerification, int completeVerification, struct bmd230__stringList **result);
int bmd230__verifyInvoiceWithDetailsById(	struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, long int id, int forceVerification, int completeVerification, struct bmd230__fileInfoList **output);
int bmd230__SendAdvice(			struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, long int bmdID, int *returnCode);
int bmd230__SendAdviceWithParameters(	struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, char* bmdID, char *accountId, char *invoiceId, char *userIdentifier, char* adviceTemplateName, char* adviceTitle, int *returnCode);


/*********************/
/* operacja dla PLAY */
/*********************/
int bmd230__getInvoicePack(			char *accountId, char *invoiceId, char *userIdentifier, char *getReason, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, enum trans_t type, char* parameter, struct bmd230__fileInfo **output);
int bmd230__getHTMLImage(			char *accountId, char *invoiceId, char *userIdentifier, char *getReason, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, enum trans_t type, struct bmd230__fileInfo **output);
int bmd230__verifyInvoice(			char *accountId, char *invoiceId, char *userIdentifier, int forceVerification, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, int *result);
int bmd230__verifyInvoiceSimplified(	char *accountId, char *invoiceId, char *userIdentifier, int forceVerification, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, int *result);
int bmd230__verifyInvoiceWithDetails(	char *accountId, char *invoiceId, char *userIdentifier, int forceVerification, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__fileInfo **output);
int bmd230__countInvoice(			char *accountId, char *invoiceId, char *userIdentifier, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId, int *count);
int bmd230__getFile(	char *accountId, char *invoiceId, char *userIdentifier, char *getReason, struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, enum trans_t type, struct bmd230__fileInfo **output);


/************************************/
/*	wstawienie plików w kawałkach	*/
/************************************/
int bmd230__bmdInsertFileChunkStart(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, char *finalfilename, long int totalfilesize,char *transactionId,char **tmpfilename);
int bmd230__bmdInsertFileChunkIter(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__fileInfo *input, char *tmpfilename, char *count, long int *result);

/************************************/
/*	pobieranie plików w kawałkach	*/
/************************************/
int bmd230__bmdGetFileChunkStart( 		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, char **tmpdir);
int bmd230__bmdGetFileChunkIter( 		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *tmpdir, char *count, struct bmd230__fileInfo **output);
int bmd230__bmdGetArchPackInChunk(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, enum chunkCntr_t cntrFlag, struct bmd230__archPackOut *input, struct bmd230__archPackOut **output );

/************************************************************/
/*	rozpoczecie transakcji - pobranie jej identyfikatora	*/
/************************************************************/
int bmd230__bmdStartTransaction( 		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char **transactionId);
int bmd230__bmdStopTransaction(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, char *transactionId, int *result);

/**************************/
/* oepracje na katalogach */
/**************************/
int bmd230__bmdCreateDirectory(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, char *description, char *dirName, char *transactionId, long int *id);
int bmd230__bmdGetDirectoryDetails(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long int id, struct bmd230__fileInfo **output);
int bmd230__bmdCreateLink(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__mtdsValues *mtds, char *description, char *linkName, char *transactionId, /*enum groupAuthorizationType_e groupAuthorization, */enum grantingType_e granting, struct bmd230__DateTime* expirationDate, long *id);
int bmd230__bmdCreateLinks(			struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__linksInfo* linksInfo, char *transactionId, struct bmd230__idList** ids);
int bmd230__bmdCreateCascadeLinks(		struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__singleLinkInfo* linkInfo, char *transactionId, long correspondingFilesMaxLevel, enum visibilityType_t visible, /*enum groupAuthorizationType_e groupAuthorization, */struct bmd230__idList **createdIds);
int bmd230__bmdCreateManyCascadeLinks(	struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__cascadeLinksInfo* cascadeLinksInfo, char *transactionId, struct bmd230__manyCascadeLinksResult **result);
int bmd230__bmdDeleteCascadeLinks(		struct xsd__base64Binary *cert, char* userRoleId, char* userGroupId, char* userClassId, struct bmd230__deleteCascadeLinksList* filesList, struct bmd230__manyCascadeLinksResult** result);

/***********************************/
/* konserwacja wartosci dowodowych */
/***********************************/
int bmd230__bmdConservateTimestamps(	struct xsd__base64Binary *cert, char *roleName, char *groupName, char *userClassId, struct bmd230__mtdsSearchValues *mtds, int *result);

/* usuwanie podmienionych lobow*/
int bmd230__bmdDeleteReplacedLobs(struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, long *deletedLobsCount);
