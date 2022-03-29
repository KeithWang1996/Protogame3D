#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

TCPServer::TCPServer(int pPort)
	:m_listenPort(pPort)
{
	FD_ZERO(&m_listenSet);
}

TCPServer::~TCPServer()
{
	FD_ZERO(&m_listenSet);
	m_timeval = { 00, 00 };
	//Change later
	delete m_listenTCPSocket;
	m_listenTCPSocket = nullptr;
}

void TCPServer::bind()
{
	if (m_listenTCPSocket != nullptr)
	{
		return;
	}
	struct addrinfo addrHintsIn;
	struct addrinfo* pAddrOut = NULL;

	ZeroMemory(&addrHintsIn, sizeof(addrHintsIn));

	addrHintsIn.ai_family = AF_INET;
	addrHintsIn.ai_socktype = SOCK_STREAM;
	addrHintsIn.ai_protocol = IPPROTO_TCP;
	addrHintsIn.ai_flags = AI_PASSIVE;

	std::string serverPort = std::to_string(m_listenPort);
	int iResult = getaddrinfo(NULL, serverPort.c_str(), &addrHintsIn, &pAddrOut);
	if (iResult != 0)
	{
		g_theConsole->Error(Stringf("Call to getaddrinfo failed %i", iResult));
	}
	 SOCKET listenSocket = socket(pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol);
	 //Maybe check is nullptr
	 m_listenTCPSocket =  new TCPSocket(listenSocket, Nonblocking, 256);//maybe new off later
	if (!m_listenTCPSocket->isValid())
	{
		g_theConsole->Error(Stringf("Call to socket failed %i", WSAGetLastError()));
	}
	m_listenTCPSocket->setMode(Nonblocking);
	m_listenTCPSocket->bind(pAddrOut);
}

void TCPServer::listen()
{
	m_listenTCPSocket->listen();
	g_theConsole->PrintString(Rgba8::WHITE, Stringf("Server listen on port %d", m_listenPort));
}

TCPSocket* TCPServer::accept()
{
	if (m_listenTCPSocket->isDataAvailable())
	{	
		return m_listenTCPSocket->accept();
	}

	return new TCPSocket();
}

void TCPServer::stopListen()
{
	if (m_listenTCPSocket->isValid())
	{
		m_listenTCPSocket->close();
	}
}

std::string TCPServer::GetAddress()
{
	return m_listenTCPSocket->GetLocalAddress();
}