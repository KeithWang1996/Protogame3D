#pragma once
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

class TCPSocket;

struct PacketHeader {
	std::uint16_t m_id;
	std::uint32_t m_magicNumber;
	std::uint16_t m_seqNo;//TODO: Change seqNo to be specific for each pair of sender and receiver
	std::uint16_t m_size;
	std::uint16_t m_numMsg;
};

struct Packet
{
	PacketHeader m_header;
	std::string m_messageText;
};

class TCPServer
{
public:
	TCPServer(int pPort = 48000);
	~TCPServer();

	void bind();
	void listen();
	TCPSocket* accept();
	void stopListen();
	std::string GetAddress();
private:
	int m_listenPort;
	FD_SET m_listenSet;
	TCPSocket* m_listenTCPSocket = nullptr;
	timeval m_timeval = {00, 00};
};