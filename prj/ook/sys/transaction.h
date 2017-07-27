#ifndef __OOK_TRANSACTION_H__
#define __OOK_TRANSACTION_H__

#include "../socket"
#include "../c++/if_unknow"

#include <vector>

#if defined(WIN32) && !defined(WINCE)
#define TRANSACTION_IOCP
#elif defined(EPOLL_INCLUDED)
#define TRANSACTION_EPOLL
#endif

#define SOCKET_FLAG_EPOLL	 0x01
#define SOCKET_FLAG_EPOLLET	 0x02
#define SOCKET_FLAG_EPOLLOUT 0x04
#define SOCKET_FLAG_IOCP	 0x10

class transaction;

/////////////////////////////////////////
// ifsock
class ifsock : public virtual if_unknow
{
public:
	virtual bool _hook_nable(SOCKET sock, transaction * t) = 0;
	virtual void _hook_sable(SOCKET sock, transaction * t) = 0;
	virtual void _hook_rable(SOCKET sock, transaction * t, unsigned char * buf, unsigned int len, sockaddr_in * raddr) = 0;
};

/////////////////////////////////////////
// iftrans
class iftrans : public virtual if_unknow
{
public:
	virtual void on_transact(int param1, int param2, void * p) = 0;
};

/////////////////////////////////////////
// transaction

#ifdef TRANSACTION_IOCP

typedef struct _PER_HANDLE_DATA
{
	SOCKET sock;
	HANDLE hIOCP;
	
	PPER_IO_DATA recvIOData;
	
	unsigned int flag;
	
	#define HANDLEFLAG_WSARECVFROM  0x01
	
	sockaddr_in raddr; // for WSARecvFrom
	int addrsize;

} PER_HANDLE_DATA, * PPER_HANDLE_DATA;

#endif

class transaction : public if_unknow
{
public:
	transaction(int thprio = THREAD_PRIORITY_NORMAL, 
				bool openselect = true, 
				int tracegrade  = -1, 
				int stacksize   = -1);
	virtual ~transaction();

	IFUNKNOW_IMP

#ifdef TRANSACTION_IOCP
	static int IOCP_tcp_send(const unsigned char * buf, 
							 unsigned int len,
							 PPER_HANDLE_DATA pPerHandle);
#endif
	static void release_extrap(void * extra);
	
	void open_detecting() { _detecting   = true; }
	void open_product  () { _openproduct = true; }
	void self_release  () { _selfrelease = true; }
	
	void max_incomings_persecond(unsigned int m) { _incomingspersecond = m; }
	void max_readings_persecond (unsigned int m) { _readingspersecond  = m; }
	
	void launch();
	void close();
	
	void open_asynmode();
	
	// IOCP
	void open_IOCP();
	void stop_IOCP();
	
	// EPOLL
	void open_EPOLL(int maxpds = -1, int maxevents = -1);
	void stop_EPOLL();

	///////////////////////////////////
	// socket signin/out
		
	void signin (const np_sockinfo_type & sockinfo, 
				 ifsock * ifcb, 
				 unsigned char ** buff, 
				 unsigned int bufsize,
				 int printmask = 0);
	void signout(np_sockinfo_type & sockinfo,
				 int printmask = 0);
	
	///////////////////////////////////
	// asyn produce
	
	void trans_in (iftrans * ifptr);
	void trans_out(iftrans * ifptr);
			 
	void produce(iftrans * ifptr,
				 int param1,
				 int param2,
				 void * p = NULL);

private:
	IFUNKNOW_DEC
	
	bool _asynmode;	 	// asyn release ifsock mode
	bool _detecting; 	// detect select state
	bool _openproduct; 	// product function
	bool _selfrelease; 	// self release me

	bool _thruning;
	
	int _thprio;
	int _stacksize;
	int _tracegrade;
	
	unsigned short _index;
	
	unsigned int _rejectnum;
	unsigned int _incomings;
	unsigned int _incomingspersecond;
	
	unsigned int _rejectrd;
	unsigned int _readings;
	unsigned int _readingspersecond;
	
	int _epfd;
	int _fdmax;
	fd_set _fdset;

	tmc_mutex_type _mu;
	tmc_mutex_type _muprod;

	struct ifsock_s
	{
		ifsock * ifcb;
		bool blistener;
		bool bfdset;
		bool bIPv6;
	};
	typedef std::map<SOCKET, ifsock_s > __socks;
	__socks _socks;
	
	typedef std::vector<ifsock * > __asyns;
	__asyns _asyns;
	
	///////////////////////
	// produce
	typedef std::map<iftrans *, int > __trans;
	__trans _trans;

	struct s_payload
	{
		iftrans * ifptr;
		void    * p;
		int		  param1;
		int		  param2;
	};
	typedef std::vector<s_payload > __payload;
	__payload _payload;
	__payload _pld;

	typedef tmc_thread_type<transaction > thread;
	thread * _th;

	void _run(int * thstatus, void * arg);

#ifdef TRANSACTION_IOCP
	bool _bIOCPrunning;
	HANDLE _hIOCP;
	
	thread * _ioth;
	
	void _iorun(int * thstatus, void * arg);
	
	int _IOCPCreatePort(const np_sockinfo_type & sockinfo, unsigned char ** buff, unsigned int bufsize);
#endif
};

#endif
