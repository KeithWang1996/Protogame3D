#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <string>
#include <limits>
#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Use std::max template not macro since it conflicts
// with max function in numeric_limits.
#ifdef max
#undef max
#endif

enum Mode
{
	Invalid,
	Blocking,
	Nonblocking
};

class TCPData
{
public:
	TCPData() : m_length(0), m_dataPtr(NULL) { }
	TCPData(std::size_t length, char* dataPtr) 
		: m_length(length)
		, m_dataPtr(dataPtr)
	{
	}

	~TCPData()
	{
		m_length = 0;
		m_dataPtr = NULL;
	}

	std::size_t getLength() { return m_length; }
	char const* getData() { return m_dataPtr; }

	static const std::size_t DATA_PENDING = std::numeric_limits<std::size_t>::max();

private:
	std::size_t m_length;
	char* m_dataPtr;
};

class TCPSocket
{
public:
	TCPSocket();
	TCPSocket(
		SOCKET& socket,
		Mode mode = Mode::Blocking,
		std::size_t bufferSize = 256
	);
	TCPSocket(const TCPSocket& src);
	~TCPSocket();

	TCPSocket& operator=(const TCPSocket& src);

	std::size_t getBufferSize() { return m_bufferSize; }

	Mode getMode();
	void setMode(Mode mode);

	bool isValid() { return m_socket != INVALID_SOCKET; }

	std::string GetAddress();
	std::string GetLocalAddress();

	void send(
		char const* dataPtr,
		std::size_t length
	);
	TCPData receive();

	bool isDataAvailable();

	void shutdown();
	void close();
	void bind(addrinfo* pAddrOut);
	void listen();
	TCPSocket* accept();//return client socket using accept

protected:

private:
	Mode m_mode;
	SOCKET m_socket;

	FD_SET m_fdSet;
	struct timeval m_timeval;

	std::size_t m_bufferSize;
	std::size_t m_receiveSize;
	char* m_bufferPtr;
};

#endif  // TCP_SOCKET_H
