#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdlob/libbmdlob.h>

#include <bmd/libbmd/libbmd_internal.h>

long bmd_check_protocol_version(	bmd_info_t * bmd_info,
					long * klntver,
					long * servver,
					long deserialisation_max_memory)							//funkcja weryfikuje wersj�protokolu z wykorzystaniem datagramu NOP
{
long status			= 0;
long serv_prot_ver		= 0;
long clnt_prot_ver		= 0;
bmdDatagram_t *BMDReq		= NULL;
bmdDatagramSet_t *BMDReq_set	= NULL;
bmdDatagramSet_t *BMDResp_set	= NULL;

	if (bmd_info==NULL)
	{	return BMD_ERR_LOGIN_AGAIN;
	}
	status = bmd_datagram_create(BMD_DATAGRAM_TYPE_NOOP,&BMDReq);				//funkcja ustawia w datagramie wersj�uywanego protokou
	if (status != BMD_OK)
	{	return status;
	}
	clnt_prot_ver=BMDReq->protocolVersion;
	if (klntver)
	{	*klntver=clnt_prot_ver;
	}
	status=bmd_datagram_add_to_set(BMDReq,&BMDReq_set);
	if (status != BMD_OK)
	{	PR2_bmdDatagram_free(&BMDReq);
		return status;
	}

	/*Tutaj wystarczy 100 na paczke bo i tak jest jeden datagram tylko.*/
	status=bmd_send_request(bmd_info,BMDReq_set, 100, &BMDResp_set, 1, deserialisation_max_memory);
	if (status==BMD_OK)
	{	serv_prot_ver = BMDResp_set->bmdDatagramSetTable[0]->protocolVersion;
#ifdef _DEBUG
		if (clnt_prot_ver!=serv_prot_ver)										// na ta chwile kazda rozbieznosc w wersji i podwersji traktujemy jako brak mozliwosci wspolpracy
		{	status = BMD_ERR_PROTOCOL;
		}
#endif
		if (servver)
		{	*servver=serv_prot_ver;
		}
		serv_prot_ver /= 100;													// dwa najmlodsze numery wersji nie wplywaja na stabilnosc polaczenia i dzialania serwera i klienta
		clnt_prot_ver /= 100;													// dlatego dwa najmlodsze znaki wersji odrzucamy
		if (clnt_prot_ver!=serv_prot_ver)										// na ta chwile kazda rozbieznosc w wersji i podwersji traktujemy jako brak mozliwosci wspolpracy
			status = VERSION_NUMBER_ERR;
	}
	else
	{																			// serwer moze odpowiedziec bledem - taka sytuacje interpretujemy jako brak mozliwosci wspolpracy z serwerem
		if ( (BMDResp_set) && (BMDResp_set->bmdDatagramSetTable[0]) && (BMDResp_set->bmdDatagramSetTable[0]->protocolVersion) )
		{	serv_prot_ver = BMDResp_set->bmdDatagramSetTable[0]->protocolVersion;
		}
		//status = VERSION_NUMBER_ERR;											// taki blad powinien byl sie pojawic juz w trakcie logowania
		if ( (status!=BMD_ERR_NET_CONNECT) && (status!=BMD_ERR_NET_WRITE) )		// wszelkie bledy inne niz polaczenia traktujemy jako koniecznosc wylogowania (mozna by tez dodac tu BMD_ERR_NET_READ, ale raczej to moze byc takze przy roznicy protokolow)
		{	status=BMD_ERR_LOGIN_AGAIN;
		}
	}
	BMDReq=NULL;
	// to tylko wskaznik - pamiec jest zwalniana podczas zwalniania calego zbioru
	PR2_bmdDatagramSet_free(&BMDReq_set);

	PR2_bmdDatagramSet_free(&BMDResp_set);
	return status;
}
