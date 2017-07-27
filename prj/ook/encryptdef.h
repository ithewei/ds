#ifndef __OOK_ENCRYPT_DEF_H__
#define __OOK_ENCRYPT_DEF_H__

#define ENCRYPT_METHOD_NO			0
#define ENCRYPT_METHOD_AES_STD		0x1000

#define ENCRYPT_METHOD_UNITENDCRYPT 0x1600
#define ENCRYPT_METHOD_HW_MTU_CRYPT 0x1601

struct encrypt_hw_mtu_crypt_s
{
	std::string ikey;
	std::string cbuf;
	std::string sdp_video_extra;
};

struct encrypt_unitend_crypt_s
{
	bool wfinit;
	void * encptr;
	unsigned char * encbuf;
};

#endif
