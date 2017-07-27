#define tmc_socket_object

#include "socket"

#ifdef WINCE

int socket_sendbuffer_   = -1;
int socket_recvbuffer_   = -1;
int socket_tcpreuseaddr_ = -1;

#elif defined(WIN32)	// system default under WIN32 = 8192

int socket_sendbuffer_   = 1024 * 64;
int socket_recvbuffer_   = 1024 * 64;
int socket_tcpreuseaddr_ = 0;

#else

int socket_sendbuffer_   = -1;
int socket_recvbuffer_   = -1;
int socket_tcpreuseaddr_ = -1;

#endif

int thread_defaultstacksize_ = 0x100000; // 1M
