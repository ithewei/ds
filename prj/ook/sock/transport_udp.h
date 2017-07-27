#ifndef __OOK_TRANSPORT_UDP_H__
#define __OOK_TRANSPORT_UDP_H__

#include "transport.h"

namespace ook { namespace socket {

class transport_udp : public transport
{
public:
    transport_udp(unsigned int bufsize = TRANSPORT_BUFFER_MAXLEN);
    virtual ~transport_udp();
	
	void set_interface(const char * intf) { if(intf) _interface = intf; }
	void set_sourceip(const char * sip)   { if(sip)  _sourceip  =  sip; }
	
	// launch udp
	unsigned short udp(const char * laddr,
					   unsigned short lport,
					   const char * raddr = NULL,
					   unsigned short rport = 0,
					   int reuseaddr = -1,
					   int sndbuflen = -1,
					   int rcvbuflen = -1,
					   int printmask = 0);

	// send data which will stop when error occurs
	int snd(const unsigned char * buf,
			unsigned int len,
			const char * addr,
			unsigned short port,
			bool on_sndflag = true,
			bool bIPv6 = false);

	// send data without stop when error occures
	int snd_continue(const unsigned char * buf,
					 unsigned int len,
					 const char * addr,
					 unsigned short port,
					 bool onsnd_callback = false,
					 bool bIPv6 = false);

	// udp's public address
	void public_address(const char * addr) { if(addr) _pubip = addr; }
	void public_address(const std::string & addr) { _pubip = addr; }
	const std::string & public_address() const;

	void public_port(unsigned short port) { _pubport = port; }
	unsigned short public_port() const;

	void sock_err(iftransport_sink * sink, SOCKET sock) {} // to avoid auto close the socket by transport

private:
	std::string _interface;
	std::string _sourceip;
	
	// Local IP's public address, specilly for NAT usage.
	// The method to obtain these varibles is not included
	// within this class. You should set these varibles 
	// before using public_address() and public_port() functions,
	// otherwise they are equal to local_address() and local_port()
	std::string _pubip;		
	unsigned short _pubport;

	virtual int on_snd(const unsigned char * buf,
					   unsigned int len,
					   const char * addr,
					   unsigned short port);
};

}}

#endif
