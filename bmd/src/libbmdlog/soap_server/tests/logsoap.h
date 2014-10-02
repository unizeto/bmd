//gsoap ns service location: http://127.0.0.1:9999
//gsoap ns service namespace: ns.xsd


typedef char*	xsd__string;


/************************************************/
/*	datagram z polami dla serwera logów	*/
/************************************************/

struct ns__logDatagram {

    xsd__string		remoteHost;
    xsd__string		remotePort;
    xsd__string		srcFile;
    xsd__string		srcLine;
    xsd__string		srcFunction;
    xsd__string		logOwner;
    xsd__string		service;
    xsd__string		operationType;
    xsd__string		logLevel;
    xsd__string		dateTimeBegin;
    xsd__string		logString;
    xsd__string		logReason;
    xsd__string		logSolution;
    xsd__string		documentSize;
    xsd__string		documentFilename;
    xsd__string		logReferto;
    xsd__string		eventCode;
    xsd__string		logOwnerEventVisible;
    xsd__string		logRefertoEventVisible;

};


int ns__logServerSend(struct ns__logDatagram *logDatagram, long* result);
int ns__verifyLog(long logNode, long* result );
int ns__verifyTree(long signNode, long* result );