#ifndef __TCNETWORK_H__
#define __TCNETWORK_H__

#include <TCFoundation/TCObject.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else // WIN32
#ifdef _QT
#define SOCKET int
#endif // _QT
#endif // WIN32

#define MAX_PACKET_SIZE (1 << 20)

#define TCNE_READ_PACKET_SIZE 1
#define TCNE_PACKET_SIZE 2
#define TCNE_READ 3
#define TCNE_WRITE_BEFORE_CONNECT 4
#define TCNE_WRITE 5
#define TCN_MAX_ERROR 5

class TCNetwork : public TCObject
{
	public:
		TCNetwork(void);

		virtual TCByte* getData(int& length);
		/*
		 * Reads data from then network socket, and sets "length" to be the
		 * length of the data read.
		 */
		virtual char* getString(void);
		/*
		 * Reads a string from the network.  Please note that the amount of data
		 * read is based on the size of the packet sent from the other side.
		 * This doesn't necessarily have any association with the length of a
		 * null-terminated string.
		 */
		int getInt(void);
		/*
		 * Reads an integer from the network.  Please note that if the network
		 * packet contained any other data, it is lost.
		 */
		float getFloat(void);
		/*
		 * Reads a float from the network.  Please note that if the network
		 * packet contained any other data, it is lost.
		 */
		virtual void sendString(const char*);
		/*
		 * Sends a null-terminated string across the network.
		 */
		void sendPrintf(const char* fmt, ...);
		/*
		 * Sends a null-terminated string across the network.  The string is
		 * constructed using the full printf format specification in "fmt" and
		 * any an all arguments in the "..." portion.
		 */
		virtual void sendData(int length, const void* data);
		/*
		 * Sends arbitrary data of length "length" across the network.
		 */

		int getErrorNumber(void) { return errorNumber; }
		char* getErrorString(void) { return errorString; }
		virtual void clearError(void);
	protected:
		virtual ~TCNetwork(void);
		virtual void dealloc(void);
		virtual void cleanShutdown(void);
		virtual void closeConnection(void);
		virtual void setErrorString(const char*, int = 0);
		virtual void setErrorNumber(int);

		SOCKET dataSocket;
		int connected;
		int errorNumber;
		char* errorString;

		static class TCNetworkSetupCleanup
		{
		public:
			TCNetworkSetupCleanup(void);
			~TCNetworkSetupCleanup(void);
		} networkSetupCleanup;
};

#endif
