//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: MSG_SVC_ERROR_PARAM
//
// MessageText:
//
//  Bad param was supplied to function Additional info %1 Function: %2 Original WIN32 Err Code %3. 
//
#define MSG_SVC_ERROR_PARAM              ((DWORD)0xC0000001L)

//
// MessageId: MSG_SVC_ERROR_DISK
//
// MessageText:
//
//  Unable to read or save file to disk Additional info %1 Function: %2 Original WIN32 Err Code %3.
//
#define MSG_SVC_ERROR_DISK               ((DWORD)0xC0000002L)

//
// MessageId: MSG_SVC_ERROR_MEM
//
// MessageText:
//
//  Insufficient memory to complete operation Additional info %1 Function: %2 Original WIN32 Err Code %3.
//
#define MSG_SVC_ERROR_MEM                ((DWORD)0xC0000003L)

//
// MessageId: MSG_SVC_ERROR_FORMAT
//
// MessageText:
//
//  Supplied data had wrong format Additional info %1 Function: %2 Original WIN32 Err Code %3.
//
#define MSG_SVC_ERROR_FORMAT             ((DWORD)0xC0000004L)

//
// MessageId: MSG_SVC_ERROR_FAILED
//
// MessageText:
//
//  Operation Failed Additional info %1 Function: %2 Original WIN32 Err Code %3.
//
#define MSG_SVC_ERROR_FAILED             ((DWORD)0xC0000005L)

//
// MessageId: MSG_SVC_ERROR_NODATA
//
// MessageText:
//
//  Desired data not found or no data present Additional info %1 Function: %2 Original WIN32 Err Code %3.
//
#define MSG_SVC_ERROR_NODATA             ((DWORD)0xC0000006L)

//
// MessageId: MSG_SVC_ERROR_NET
//
// MessageText:
//
//  Read or write to network subsystem failed Additional info %1 Function: %2 Original WIN32 Err Code %3.
//
#define MSG_SVC_ERROR_NET                ((DWORD)0xC0000007L)

//
// MessageId: MSG_SVC_OK
//
// MessageText:
//
//  Following operation successfully executed %1.
//
#define MSG_SVC_OK                       ((DWORD)0x00000008L)

//
// MessageId: MSG_SVC_SIGN_OK
//
// MessageText:
//
//  Signature operation executed succesfully. Additional info: %1 Function: %2
//
#define MSG_SVC_SIGN_OK                  ((DWORD)0x00000009L)

//
// MessageId: MSG_SVC_DECRYPT_OK
//
// MessageText:
//
//  Decryption operation executed succesfully. Additional info: %1 Function: %2
//
#define MSG_SVC_DECRYPT_OK               ((DWORD)0x0000000AL)

//
// MessageId: MSG_SVC_WARN
//
// MessageText:
//
//  Decryption key wasn't defined in configuration file.
//
#define MSG_SVC_WARN                     ((DWORD)0x8000000BL)

