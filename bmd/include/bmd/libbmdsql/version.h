#ifndef _VERSION_H_INCLUDED_
#define _VERSION_H_INCLUDED_

long versionInsertSymKeyIntoDatabase(void* hDB, bmdDatagram_t* bmdJSRequest, char** symKeyLobId);
long versionInsertToCryptoObjectsHistory(void* hDB, char* cryptoObjectId);
long versionUpdateCryptoObjectSystemMetadata(void* hDB, char* cryptoObjectId, char* fileSize, char* raster, char* symkey, char* hashValueHexString);
long versionLinkProofsToCryptoObjectsHistoryEntry(void *hDB, char* cryptoObjectId);



#endif /*_VERSION_H_INCLUDED_*/
