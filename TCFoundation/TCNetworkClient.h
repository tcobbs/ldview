#ifndef __TCNETWORKCLIENT_H__
#define __TCNETWORKCLIENT_H__

#include <TCFoundation/TCNetwork.h>

#ifdef WIN32
#else // WIN32
#if defined (_QT) || defined (__APPLE__)
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
#endif // _QT || __APPLE__
#endif // WIN32

#define TCNCE_SET_LINGER TCN_MAX_ERROR + 1
#define TCNCE_HOSTNAME_LOOKUP TCN_MAX_ERROR + 2
#define TCNCE_NO_PORT TCN_MAX_ERROR + 3
#define TCNCE_CONNECT TCN_MAX_ERROR + 4
#define TCNCE_CONNECTION_REFUSED TCN_MAX_ERROR + 5
#define TCNCE_SET_NODELAY TCN_MAX_ERROR + 6
#define TCNC_MAX_ERROR TCNCE_SET_NODELAY

class TCNetworkClient : public TCNetwork
{
	public:
		TCNetworkClient(void);
		TCNetworkClient(int);
		TCNetworkClient(int, char*);

		virtual int openConnection(void);
		/*
		 * Opens a network connection to serverHost on port "port".
		 * There must be a Server running there on that port.
		 */
		virtual void closeConnection(void);
		virtual void setServerHost(const char*);
		virtual char* getServerHost(void);
		int isConnected(void) {return connected != 0;}
		/*
		 * Tells you if you are connected.  This can only be verified after
		 * having written data on a connection that is closed, or before the
		 * connection has been opened at all.
		 */
	protected:
		virtual ~TCNetworkClient(void);
		virtual void dealloc(void);
		virtual int setupSocket(void);
		virtual void setLinger(void);
		virtual void setTcpNoDelay(void);
		virtual void setErrorNumber(int);

		int port;
		sockaddr_in portOut;
		char* serverHost;
};

#endif
