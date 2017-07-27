#ifndef __OOK_TRANSPORT_TCP_H__
#define __OOK_TRANSPORT_TCP_H__

#include "transport.h"

#define TCP_SEND_TIMEOUT_NOBLOCK  0
#define TCP_SEND_TIMEOUT_INFINITE -1000

namespace ook { namespace socket {

class transport_tcp : public transport
{
public:
	transport_tcp(unsigned int bufsize = TRANSPORT_BUFFER_MAXLEN);
    virtual ~transport_tcp();

	bool has_asyncsend() const    			{ return (_sockinfo.contxt != NULL); }
	void set_listenbacklog(int n) 			{ _listenbacklog = n;                }
	void set_interface(const char * intf) 	{ if(intf) _interface = intf;        }
	
	// listening
	unsigned short listen(const char * laddr,
						  unsigned short lport,
						  int keepalive = -1,
						  int sndbuflen = -1,
						  int rcvbuflen = -1,
			              int reuseaddr = -1);

	// launch tcp, for forwarding connect
    bool tcp(const char * rip,
			 unsigned short rport,
			 int timeout   = -1,
			 int sndbuflen = -1,
			 int rcvbuflen = -1,
			 int keepalive = -1,
			 int printmask = 0);

	// new function
	bool re_connect();

	// launch tcp, for incoming connect
	bool tcp(const np_sockinfo_type & nsock);

	bool set_tcpnodelay(bool b);
	unsigned int tcp_dispcount(bool rst);

	// send data
	virtual int snd(const unsigned char * buf,
					unsigned int len,
					int timeout = -1,
				    int * perr = NULL);

	////////////////////////////////////////////
	// virtual function inherite from ifsock
	bool _hook_nable(SOCKET sock, transaction * t);
	void _hook_sable(SOCKET sock, transaction * t);

private:
	int _timeout;
	int _listenbacklog;
	int _asynsendcount;
	unsigned int _sndcount;
	
	bool _bnodelay;
	bool _bsignal;
	
	std::string _interface;
	
	tmc_mutex_type _mucd;
	tmc_cond_type  _cond;
		
	// new virtual function
	virtual bool _new_tcp(const np_sockinfo_type & nsock, transaction * t);
};

}}

#endif
