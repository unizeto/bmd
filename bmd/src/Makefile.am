OLD_LIBS=libbmdaa libbmdasn1_core libbmdpr

if BUILD_TESTS
TESTS=libbmddialogs/tests  libbmdxades/tests libbmdbase64/tests \
		libbmdconfig/tests libbmdprotocol/tests \
		libbmdpkcs11/tests libbmdpkcs12/tests libbmdX509/tests libbmddvcs/tests \
		libbmdmail/tests  libbmdmime/tests  libbmd/tests libbmdasn1_common/tests \
		libbmdasn1_core/tests libbmddb/tests libbmdpr/tests \
		libbmdts/tests libbmdaa2/tests libbmdcms/tests libbmderr/tests \
		libbmdlob/tests libbmdlog/tests  libbmdpki/tests libbmdsql/tests \
		libbmdxml/tests libbmdcrl/tests libbmdnet/tests \
		libbmdsoap/tests libbmdcurl/tests libbmdzlib/tests libbmdxslt/tests \
		libCppJavaApiWrapper/tests
endif


if BUILD_LIBS
LIB_DIRS = libbmdglobals libbmdxpath libbmderr libbmdconfig libbmdaa2 libbmdasn1_core libbmdasn1_common libbmdks bmd_ks\
   libbmdbase64 libbmdnet libbmdxpath libbmdutils \
   libbmdpkcs11 libbmdpkcs12 libbmdX509 libbmdcms libbmdpki \
   libbmdpr  \
   libbmdts libbmdcrl libbmddb libbmddb/db2 libbmdxml libbmdxades \
   libbmdprotocol libbmdlob  libbmd libbmdmail libbmdmime \
   libbmddvcs libbmdsql libbmdlog libbmdcurl libbmdzlib libbmdxslt libbmdarchpack libCppJavaApiWrapper 
endif

if BUILD_SERVER
SERVER_DIRS=libbmdsoap mail_sender 
endif

if BUILD_GUI_LIBS
#GUILIB_DIRS=libbmddialogs
endif

#if BUILD_CLIENT
#CLIENT_DIRS=klient
#endif

if BUILD_XML
#XML_DIRS=libbmdxml libbmdxpath libbmdxades
#XML_DIRS=libbmdxml libbmdxades
endif

#if BUILD_ESP
#ESP_DIRS=esp_da daexport
#endif

SUBDIRS = $(LIB_DIRS) $(XML_DIRS) $(SERVER_DIRS) $(ESP_DIRS) $(GUILIB_DIRS) $(CLIENT_DIRS) $(TESTS)

