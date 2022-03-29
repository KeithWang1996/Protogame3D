#pragma once
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <array>
class UDPSocket
{
public:
	static const int BufferSize = 512;
	using Buffer = std::array<char, BufferSize>;

	UDPSocket(const std::string& host, int port);
	~UDPSocket();

	void bind(int port);
	int send(int length);
	int receive();
	void close();

	Buffer& sendBuffer() { return m_sendBuffer; }
	Buffer& receiveBuffer() { return m_receiveBuffer; }
public:
	std::string m_host;
	int m_port;
protected:
private:
	Buffer m_sendBuffer;
	Buffer m_receiveBuffer;
	sockaddr_in m_toAddress;
	SOCKET m_socket;
};