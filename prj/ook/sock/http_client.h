#ifndef __OOK_HTTP_CLIENT_H__
#define __OOK_HTTP_CLIENT_H__

#include <stdint.h>
#include <ook/sock/transport_tcp.h>
#include <ook/buf>

/*
	!!! you must create such object by new method
 */
class http_client : public ook::socket::transport_tcp
{
public:
	int tracegrade;
	int HTTP1_1;
	
	enum http_protocol
	{
		e_protocol_standard = 0,
		e_protocol_shoutcast
	};
	
	enum http_access_status
	{
		e_status_unknow,
		e_status_init,
		e_status_ok,
		e_status_writefile,
		e_status_err,
		e_status_over,
		
		e_status_move,
		e_status_contentlength
	};
	
	enum http_errcode
	{
		e_errcode_unknow = -1,
		e_errcode_none,
		e_errcode_move,
		e_errcode_401,
		e_errcode_403,
		e_errcode_404,
		e_errcode_405,
		e_errcode_406,
		e_errcode_408
	};
	
	static const char * errcode_s(int errcode);
	
	http_client(transaction * trans = NULL, int tracegrade = -1);
	http_client(transaction * trans, bool need_resp, unsigned int index, int tracegrade = -1);
	virtual ~http_client();

	int get_status  () const { return _status;   }
	int get_errcode () const { return _errcode;  }
	int get_protocol() const { return _protocol; }
	
	bool is_chunked    () const { return _bchunked;        }
	bool is_downloading() const { return _downloading > 0; }
	
	void append_null()				   { _buf.append_null();   }
	unsigned char * get_buffer() const { return _buf.ptr();    }
	unsigned int    get_length() const { return _buf.length(); }

	void open_download  (bool bopen)         { _downloading = (bopen ? 1 : 0);  }
	void set_filepath   (const char * fpath) { if(fpath) _fpath       = fpath;  }
	void set_clienttype (const char * ctype) { if(ctype) _clienttype  = ctype;  }
	void set_xclienttype(const char * ctype) { if(ctype) _xclienttype = ctype;  }
	void set_predefhost (const char * host)  { if(host)  _predefhost  = host;   }
	void set_extheadline(const char * line)  { if(line)  _extheadline = line;   }
	
	void set_index(unsigned int idx) { _index = idx; }
	
	void set_callback(http_client * cb, int callbackId);
	
	uint64_t get_contlength() const { return _contentlength;  }
	uint64_t get_recvlength() const { return _receivelength;  }
	uint64_t get_rangesize () const { return _totalrangesize; }
	
	void reset();
	
	int waiting(unsigned int timeout = 0); // add @ 2015/05/13
	int signal();  // add @ 2014/06/07
	
	bool request(const char * url, unsigned int timeout, int tracegrade = -1);
	bool partial(const char * url, const char * range, unsigned int timeout, int tracegrade = -1);
	
	bool request(const char * url, const char * content, unsigned int timeout, int tracegrade = -1);
	bool request(const char * url, const char * content_type, const char * content, unsigned int timeout, int tracegrade = -1);

	void query(std::string & rsp, unsigned int timeout = 0, int * errcode = NULL);
		
	void sock_err (ook::socket::iftransport_sink * sink, SOCKET sock);
	void sock_data(ook::socket::iftransport_sink * sink, SOCKET sock, unsigned char * buf, unsigned int len);
	
private:
	transaction * _trans;
	
	FILE * _fp;
	FILE * _fpraw;
	http_client * _cb;
	
	std::string _clienttype;
	std::string _xclienttype;
	std::string _predefhost;
	std::string _extheadline;
	std::string _reqrange;
	
	std::string _fpath;

	unsigned int _index;
	unsigned int _begintick;
	
	uint64_t _contentlength;
	uint64_t _receivelength;
	uint64_t _totalrangesize;
		
	int _status;
	int _errcode;
	int _protocol;
	int _condwait;
	int _downloading;
	int _callbackId;
	
	bool _bchunked;
	bool _brcvresp;
	bool _bextrans;
	
	any_buf _buf;
	
	tmc_mutex_type _mucd;
	tmc_cond_type  _cond;

	void __reset();

	int _request(const char * host, const char * req, const char * content_type, const char * content, int timeout);
	bool _connect(const char * url, const char * content_type, const char * content, unsigned int timeout);
	void __ondownloadstream(unsigned char * buf, unsigned int len);
	
	virtual void on_http_recvstatus(int callbackId, int opt, int param, void * ptr)   {}
	virtual void on_http_recvdate(int callbackId, const unsigned char * buf, int len) {}
};

#endif
