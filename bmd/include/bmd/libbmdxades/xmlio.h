#ifndef XADES_XMLIO_H
#define XADES_XMLIO_H

int bmdxml_IO_curl_Match(const char * filename);
int bmdxml_IO_curl_Close(void * context);
int bmdxml_IO_curl_Read(void * context, char * buffer, int len);
void * bmdxml_IO_curl_Open(const char * filename);


#endif

