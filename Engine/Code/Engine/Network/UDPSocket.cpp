#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <iostream>

//#define TEST_MODE

#ifdef TEST_MODE
#define LOG_ERROR(...) printf((Stringf(__VA_ARGS__) + std::string("\n")).c_str())
#define LOG_PRINT(...) printf((Stringf(__VA_ARGS__) + std::string("\n")).c_str())
#else
#define LOG_ERROR(...) g_theConsole->Error(Stringf(__VA_ARGS__))
#define LOG_PRINT(...) g_theConsole->PrintString(Rgba8::WHITE, Stringf(__VA_ARGS__))
#endif

UDPSocket::UDPSocket(const std::string& host, int port) : m_socket(INVALID_SOCKET)
{
	m_toAddress.sin_family = AF_INET;
	m_toAddress.sin_port = htons((u_short)port);
	m_toAddress.sin_addr.s_addr = inet_addr(host.c_str());

	m_host = host;
	m_port = port;

	m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		LOG_ERROR("Socket instantiate failed %i", WSAGetLastError());
	}
}

UDPSocket::~UDPSocket()
{
	close();
}

void UDPSocket::bind(int port)
{
	//LOG_ERROR("TTTTTTEEEEEEESSSSSSTTTTTT");
	sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons((u_short)port);
	bindAddr.sin_addr.s_addr = htons(INADDR_ANY);

	int result = ::bind(m_socket, (SOCKADDR*)&bindAddr, sizeof(bindAddr));
	if (result != 0)
	{
		LOG_ERROR("Socket bind failed %i", WSAGetLastError());
	}
}

void UDPSocket::close()
{
	if (m_socket != INVALID_SOCKET)
	{
		int result = closesocket(m_socket);
		if (result == SOCKET_ERROR)
		{
			LOG_ERROR("Socket close failed %i", WSAGetLastError());
		}
		m_socket = INVALID_SOCKET;
	}
}

int UDPSocket::send(int length)
{
	int result = ::sendto(m_socket, &m_sendBuffer[0], static_cast<int>(length), 0,
		reinterpret_cast<SOCKADDR*>(&m_toAddress), sizeof(m_toAddress));
	if (result == SOCKET_ERROR)
	{
		LOG_ERROR("Socket send failed %i", WSAGetLastError());
	}
	return result;
}

int UDPSocket::receive()
{
	sockaddr_in fromAddr;
	int fromLen = sizeof(fromAddr);
	int result = ::recvfrom(m_socket, &m_receiveBuffer[0],
		static_cast<int>(m_receiveBuffer.size()), 0, reinterpret_cast<SOCKADDR*>(&fromAddr), &fromLen);
	if ((result == SOCKET_ERROR || result < 0 ) && WSAGetLastError() != 10054)
	{
		LOG_ERROR("Socket receive failed %i", WSAGetLastError());
	}
	std::string fromAddress = inet_ntoa(fromAddr.sin_addr);
	//unsigned int fromPort = static_cast<unsigned int>(ntohs(fromAddr.sin_port));
	//LOG_PRINT(Stringf("Receive string from %s:%u", fromAddress.c_str(), fromPort).c_str());
	return result;
}