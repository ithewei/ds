#ifndef __OOK_ERRCODE_H__
#define __OOK_ERRCODE_H__

#define ERR_RECV_LOSSOFPKG			   0x1000
#define ERR_RECV_LENMISMATCH		   0x1001
#define ERR_RECV_OVERFLOW 			   0x1002
#define ERR_RECV_TSREADERR			   0x1003
#define ERR_RECV_UNKNOW				   0x1080

// common err code
#define ERRCODE_NOERR						0
#define ERRCODE_PARAMETERERR			 -101
#define ERRCODE_NULLPOINTER				 -102
#define ERRCODE_MALLOCFAIL				 -103

// access server err code deined, for livehttp and rtsp_gw

#define ACCESS_ERRCODE_NOERR				0

#define ACCESS_ERRCODE_UNKNOWERR		-1000
#define ACCESS_ERRCODE_INNERERR			-1001
#define ACCESS_ERRCODE_UNKNOWAPPTYPE	-1002
#define ACCESS_ERRCODE_CHANNELNOTFOUND	-1003
#define ACCESS_ERRCODE_MEDIANOTFOUND	-1004
#define ACCESS_ERRCODE_STREAMTYPEERR	-1005
#define ACCESS_ERRCODE_UNCOMPRESSERR	-1006
#define ACCESS_ERRCODE_BASE64DECERR		-1007

#define ACCESS_ERRCODE_FORMAT_PDUERR	-2000
#define ACCESS_ERRCODE_FORMAT_URLERR	-2001
#define ACCESS_ERRCODE_FORMAT_PARAMERR	-2002

#define ACCESS_ERRCODE_AUTH_FAIL		-3000
#define ACCESS_ERRCODE_AUTH_OUTOFDATE	-3001
#define ACCESS_ERRCODE_AUTH_REMOTEIPERR	-3002

#define ACCESS_ERRCODE_DECRYPT_AESERR	-4000
#define ACCESS_ERRCODE_DECRYPT_DESERR	-4001

#define ACCESS_ERRCODE_ANALYSE_SRCERR	-5000
#define ACCESS_ERRCODE_ANALYSE_RANGEERR	-5001

#define ACCESS_ERRCODE_MEDIA_UNKNOWFMT	-6000
#define ACCESS_ERRCODE_MEDIA_OPENFAIL	-6001

// IOCP special
#define ERRCODE_IOCP_POSTSTATUSFAIL	   -10001
#define ERRCODE_IOCP_WSASENDFAIL	   -10002

// special
#define ERRCODE_HTTP_UNSUPPORT_KEEPALIVE -20001

#endif
