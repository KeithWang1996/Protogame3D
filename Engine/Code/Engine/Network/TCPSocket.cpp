#include "Engine/Network/TCPSocket.hpp"
#include <array>
#include <strstream>
#include <iostream>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"


TCPSocket::TCPSocket(
) : m_mode(Mode::Blocking)
, m_socket(INVALID_SOCKET)
, m_timeval{ 0l, 0l }
, m_bufferSize(256)
, m_receiveSize(0)
, m_bufferPtr(NULL)
{
	FD_ZERO(&m_fdSet);
	m_bufferPtr = new char[m_bufferSize];
}

TCPSocket::TCPSocket(
	SOCKET& socket,
	Mode mode,
	std::size_t bufferSize
) : m_mode(mode)
, m_socket(socket)
, m_timeval{ 0l, 0l }
, m_bufferSize(bufferSize)
, m_receiveSize(0)
, m_bufferPtr(NULL)
{
	FD_ZERO(&m_fdSet);
	m_bufferPtr = new char[m_bufferSize];
}

TCPSocket::TCPSocket(
	const TCPSocket& src
) : m_mode(src.m_mode)
, m_socket(src.m_socket)
, m_fdSet(src.m_fdSet)
, m_timeval(src.m_timeval)
, m_bufferSize(src.m_bufferSize)
, m_receiveSize(0)
, m_bufferPtr(NULL)
{
	if (m_bufferSize > 0)
	{
		m_bufferPtr = new char[m_bufferSize];
	}
}

TCPSocket::~TCPSocket()
{
	m_mode = Mode::Invalid;
	m_socket = INVALID_SOCKET;
	FD_ZERO(&m_fdSet);
	m_bufferSize = 0;
	m_receiveSize = 0;
	delete[] m_bufferPtr;
}

TCPSocket& TCPSocket::operator=(const TCPSocket& src)
{
	if (this != &src)
	{
		m_mode = src.m_mode;
		m_bufferSize = src.m_bufferSize;
		m_receiveSize = src.m_receiveSize;
		m_socket = src.m_socket;
		if (m_bufferSize > 0)
		{
			m_bufferPtr = new char[m_bufferSize];
		}
	}
	return *this;
}

Mode TCPSocket::getMode()
{
	return m_mode;
}

void TCPSocket::setMode(Mode mode)
{
	m_mode = mode;
	if (m_mode != Mode::Invalid)
	{
		u_long winsockmode = 0;
		switch (m_mode)
		{
		case Mode::Blocking: winsockmode = 0; break;
		case Mode::Nonblocking: winsockmode = 1; break;
		}
		int iResult = ioctlsocket(m_socket, FIONBIO, &winsockmode);
		if (iResult == SOCKET_ERROR)
		{
			g_theConsole->Error(Stringf("Call to ioctlsocket on socket failed, error = %i", WSAGetLastError()));
		}
	}

}

std::string TCPSocket::GetAddress()
{
	std::array<char, 64> addressStr;
	DWORD outLen = 63;

	sockaddr clientAddr;
	int addrSize = sizeof(clientAddr);
	int iResult = getpeername(m_socket, &clientAddr, &addrSize);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to getpeername failed %i", WSAGetLastError()));
	}

	iResult = WSAAddressToStringA(&clientAddr, addrSize, NULL, &addressStr[0], &outLen);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to WSAAddressToStringA failed %i", WSAGetLastError()));
	}
	addressStr[outLen] = NULL;
	return std::string(&addressStr[0]);
}

std::string TCPSocket::GetLocalAddress()
{
	std::array<char, 64> addressStr;
	DWORD outLen = 63;

	sockaddr clientAddr;
	int addrSize = sizeof(clientAddr);
	int iResult = getsockname(m_socket, &clientAddr, &addrSize);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to getsockname failed %i", WSAGetLastError()));
	}

	iResult = WSAAddressToStringA(&clientAddr, addrSize, NULL, &addressStr[0], &outLen);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to WSAAddressToStringA failed %i", WSAGetLastError()));
	}
	addressStr[outLen] = NULL;
	return std::string(&addressStr[0]);
}

void TCPSocket::send(char const* dataPtr, size_t length)
{
	int iResult = ::send(m_socket, dataPtr, static_cast<int>(length), 0);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to socket failed %i", WSAGetLastError()));
	}
	else if (iResult < static_cast<int>(length))
	{
		g_theConsole->Error("Not enough bytes sent.");
	}
}

TCPData TCPSocket::receive()
{
	int iResult = ::recv(m_socket, m_bufferPtr, static_cast<int>(m_bufferSize), 0);
	if (iResult == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK && m_mode == Mode::Nonblocking)
		{
			return TCPData{ TCPData::DATA_PENDING, NULL };
		}
		else
		{
			g_theConsole->Error(Stringf("Call to recv failed %i", WSAGetLastError()));
			closesocket(m_socket);
		}
	}
	return TCPData{ static_cast<std::size_t>(iResult), m_bufferPtr };
}

bool TCPSocket::isDataAvailable()
{
	if (m_mode == Mode::Nonblocking)
	{
		FD_ZERO(&m_fdSet);
		FD_SET(m_socket, &m_fdSet);
		int iResult = select(0, &m_fdSet, NULL, NULL, &m_timeval);
		if (iResult == SOCKET_ERROR)
		{
			g_theConsole->Error(Stringf("Call to select failed %i", WSAGetLastError()));
		}
		return FD_ISSET(m_socket, &m_fdSet);
	}
	else
	{
		g_theConsole->Error("Function isDataAvailable is only valid in non-blocking mode");
		return false;
	}
}

void TCPSocket::shutdown()
{
	int iResult = ::shutdown(m_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to shutdown failed %i", WSAGetLastError()));
	}
}

void TCPSocket::close()
{
	int iResult = closesocket(m_socket);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to close failed %i", WSAGetLastError()));
	}
	m_socket = INVALID_SOCKET;
}

void TCPSocket::bind(addrinfo* pAddrOut)
{
	int iResult = ::bind(m_socket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to bind failed %i", WSAGetLastError()));
	}
}

void TCPSocket::listen()
{
	int iResult = ::listen(m_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to listen failed %i", WSAGetLastError()));
	}
}

TCPSocket* TCPSocket::accept()
{
	SOCKET clientSocket = ::accept(m_socket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		g_theConsole->Error(Stringf("Call to accept failed %i", WSAGetLastError()));
	}
	TCPSocket* toreturn = new TCPSocket(clientSocket);
	g_theConsole->PrintString(Rgba8::WHITE, Stringf("Client connect from %s", toreturn->GetAddress().c_str()));
	return toreturn;
}