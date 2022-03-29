#include "Engine/Network/TCPClient.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include <iostream>
#include <strstream>
#include <array>

TCPSocket* TCPClient::connect(
	std::string const& host,
	std::uint16_t port,
	Mode mode
)
{
	m_mode = mode;

	// Resolve the port locally
	struct addrinfo  addrHintsIn;
	struct addrinfo* pAddrOut = NULL;

	ZeroMemory(&addrHintsIn, sizeof(addrHintsIn));
	addrHintsIn.ai_family = AF_INET;
	addrHintsIn.ai_socktype = SOCK_STREAM;
	addrHintsIn.ai_protocol = IPPROTO_TCP;

	// Get the Server IP address and port
	std::ostrstream serverPort;
	serverPort << port << std::ends;
	int iResult = ::getaddrinfo(host.c_str(), serverPort.str(), &addrHintsIn, &pAddrOut);//Maybe add a node name later
	if (iResult != 0)
	{
		g_theConsole->Error(Stringf("Call to getaddrinfo failed %i", WSAGetLastError()));
	}
	// Create the connection socket.
	SOCKET socket = ::socket(pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol);
	if (socket == INVALID_SOCKET)
	{
		g_theConsole->Error(Stringf("Call to socket failed %i", WSAGetLastError()));
	}

	// Connect to the server
	iResult = ::connect(socket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
	freeaddrinfo(pAddrOut);

	if (socket == INVALID_SOCKET)
	{
		g_theConsole->Error("Connection failed.");
		return nullptr;
	}

	// Set blocking mode as needed.
	if (m_mode == Mode::Nonblocking)
	{
		u_long winsockmode = 1;
		iResult = ioctlsocket(socket, FIONBIO, &winsockmode);
		if (iResult == SOCKET_ERROR)
		{
			g_theConsole->Error(Stringf("Call to ioctlsocket failed %i", WSAGetLastError()));
		}
	}
	return new TCPSocket(socket, m_mode);
}
