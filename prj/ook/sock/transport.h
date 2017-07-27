#ifndef __OOK_TRANSPORT_H__
#define __OOK_TRANSPORT_H__

#include "../sys/transaction.h"

#define SOCKETREF int
#define INVALID_SOCKETREF -1

#define TRANSPORT_BUFFER_MAXLEN	1452 // MTU - PPPoEHeader - IPHeader - TCPHeader = 1500 - 8 - 20 - 20 = 1452

namespace ook { namespace socket {

/////////////////////////////////////////////////
// iftransport_sink

class iftransport_sink : public virtual if_unknow
{
public:
	virtual void sock_err (iftransport_sink * sink, SOCKET sock) = 0;
	virtual void sock_data(iftransport_sink * sink, SOCKET sock, unsigned char * buf, unsigned int len) = 0;
};

/////////////////////////////////////////////////
// transport

class transport_tcp;
class transport_udp;

class transport : public ifsock,
				  public iftransport_sink
{
	friend class transport_tcp;
	friend class transport_udp;

public:
	enum sock_state
	{
		sock_closed = -1,
		sock_error,
		sock_working,
	};

    transport(unsigned int bufsize = TRANSPORT_BUFFER_MAXLEN);
    virtual ~transport();

	IFUNKNOW_ADDREF
	IFUNKNOW_DECREF
	IFUNKNOW_QUERYINTERFACE

	unsigned int release();

	void close();

	const np_sockinfo_type & get_sockinfo() const { return _sockinfo; }

	unsigned short get_index() const       { return _index;          }
	SOCKET get_socket() const			   { return _sockinfo.sock;  }
	sock_state get_sockstate() const	   { return _sockstate;      }
	bool isIPv6() const 				   { return _sockinfo.bIPv6; }
	void set_sockref(SOCKETREF ref)		   { _sockref = ref;         }
	
	void transport_ref(unsigned int ref)   { _transportref = ref;    }
	unsigned int transport_ref() const	   { return _transportref;   }

	void transport_flag(unsigned int flag) { _transportflag = flag;  }
	unsigned int transport_flag() const	   { return _transportflag;  }

	void set_transaction(transaction * trans);
	void set_transportsink(iftransport_sink * sink);

	// open dynamic remote address mapping
	void dynamic_remote_address(bool open = true) { _dynamic = open; }

	// local address
	void local_address(const char * addr)		  { if(addr) (std::string &)_sockinfo.lip = addr; }
	void local_address(const std::string & addr)  { (std::string &)_sockinfo.lip = addr; }
	const std::string & local_address() const	  { return _sockinfo.lip; }

	void local_port(unsigned short port)          { (unsigned short &)_sockinfo.lport = port; }
	unsigned short local_port() const	          { return _sockinfo.lport; }

	// remote address
	void remote_address(const char * addr)		  { if(addr) (std::string &)_sockinfo.rip = addr; }
	void remote_address(const std::string & addr) { (std::string &)_sockinfo.rip = addr; }
	const std::string & remote_address() const    { return _sockinfo.rip; }

	void remote_port(unsigned short port)         { (unsigned short &)_sockinfo.rport = port; }
	unsigned short remote_port() const            { return _sockinfo.rport; }

	// payload statistaic
	void init_playload_rcv()                      { _rcvlen = 0;    }
	void init_playload_snd()                      { _sndlen = 0;    }
	unsigned int playload_recv() const            { return _rcvlen; }
	unsigned int playload_send() const            { return _sndlen; }
	void payload_send(unsigned int l)             { _sndlen += l;   }

	///////////////////////////////////////////////////////
	// virtual function inherite from ifsock
	bool _hook_nable(SOCKET sock, transaction * t);
	void _hook_sable(SOCKET sock, transaction * t);
	void _hook_rable(SOCKET sock, transaction * t, unsigned char * buf, unsigned int len, sockaddr_in * raddr);

	///////////////////////////////////////////////////////
	// virtual function inherite from iftransport_sink
	void sock_err (iftransport_sink * sink, SOCKET sock);
	void sock_data(iftransport_sink * sink, SOCKET sock, unsigned char * buf, unsigned int len);

protected:
	void _signin(int printmask = 0);

private:
	IFUNKNOW_DEC
	
	unsigned char * _buff;
	unsigned int _bufsize;

	transaction * _trans;
	iftransport_sink * _sink;

	SOCKETREF _sockref;
	sock_state _sockstate;
	np_sockinfo_type _sockinfo;
	
	bool _dynamic;
	
	unsigned short _index;
	unsigned int _transportref;
	unsigned int _transportflag;
	unsigned int _rcvlen;
	unsigned int _sndlen;
};

}}

#endif
