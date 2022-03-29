#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include <string>
#include <list>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Engine/Core/SynchronizedBlockingQueue.hpp"
#include "Engine/Core/SynchronizedNonBlockingQueue.hpp"

class TCPServer;
class TCPClient;
class TCPSocket;

class UDPSocket;

class Clock;
class Timer;

struct Packet;

struct UDPMessageHeader
{
	std::uint16_t id;
	std::uint32_t magicNumber;
	std::uint16_t length;
	std::uint16_t seqNo;
	std::uint16_t numMsg;
};

using UDPMessage = std::tuple<std::uint16_t, std::uint32_t, std::uint16_t, std::uint16_t, std::uint16_t, std::string>;

class ConnectionState
{
public:
	ConnectionState() = default;
	~ConnectionState();
	//main thread function checks receive queue
	void ReceiveFromReceiveQueue();
	//main thread function pushes to write queue
	void PushToWriteQueue(UDPMessage message);

	//Convert string into a UDPMessage
	void SendUDPMessage(std::string message, std::uint32_t magicNumber = 0, std::uint16_t id = 2, std::uint16_t numMsg = 1);
	void SendReliableUDPMessage(std::string message, std::uint32_t magicNumber = 0, std::uint16_t numMsg = 1);
	void SendMessageInReliableMap();
	//read thread function that read from socket and push to read queue
	void ReadThreadMain();

	//write thread function that get from 
	void WriteThreadMain();
public:
	UDPSocket* m_UDPSocket = nullptr;
	//SNQ for receive
	SynchronizedNonblockingQueue<UDPMessage> m_receiveQueue;
	//SBQ for sending
	SynchronizedBlockingQueue<UDPMessage> m_sendQueue;

	std::vector<UDPMessage> m_reliableMap;

	std::thread* m_readThread = nullptr;
	std::thread* m_writeThread = nullptr;

	bool m_isQuitting = false;

	int m_bindPort = 0;

	static std::uint16_t s_sequenceNumber;

	int m_errorWatcher = 0;

	std::uint32_t m_magicNumber = 0;
};

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();
// 	int GetListenPort() { return m_listenPort; }
// 	void SetListenPort(int listenPort) { m_listenPort = listenPort; }

	bool IsListening() { return m_isListening; }
	void SetIsListening(bool islListening) { m_isListening = islListening; }
	void StartUp();
	void BeginFrame();
	void Update();
	void EndFrame();
	void ShutDown();
	void StartServer(int port);
	void StartClient(std::string ip, int port);
	void SendTextMessage(std::string msg, std::uint32_t magicNumber = 0, std::uint16_t numMessage = 1, std::uint16_t id = 2);
	void StopServer();
	void StopClient();
	void HandleDisconnectedUDPSocket();
	std::string GetClientAddress();

	//Single UDP function
// 	void OpenUDPPort(int bindPort, int sendToPort, std::string host = "127.0.0.1");
// 	void CloseUDPPort(int bindPort);
// 
// 	//main thread function checks receive queue
// 	void ReceiveFromReceiveQueue();
// 	//main thread function pushes to write queue
// 	void PushToWriteQueue(UDPMessage message);
// 
// 	//Convert string into a UDPMessage
// 	void SendUDPMessage(std::string message);
// 
// 	//read thread function that read from socket and push to read queue
// 	void ReadThreadMain();
// 
// 	//write thread function that get from 
// 	void WriteThreadMain();

	//Make Connection state version of all the above UDP functions;
	void CreateUDPConnectionState(int bindPort, int sendToPort, std::string host = "127.0.0.1");
	void CloseUDPConnectionState(int bindPort);
	void CloseAllUDPConnectionState();
	void ReceiveForAllUDPConnectionState();
	void ShutDownAllUDPConnections();
	void SendUDPMessage(std::string message, int UDPIndex, std::uint32_t magicNumber = 0, std::uint16_t id = 2, std::uint16_t numMsg = 1);
	void SendReliableUDPMessage(std::string message, int UDPIndex, std::uint32_t magicNumber = 0, std::uint16_t numMsg = 1);
private:
	
	bool m_isListening;
	
	TCPSocket* m_clientSocket = nullptr;
	timeval m_timeval;
	std::list<TCPServer*> m_servers;
	std::list<TCPClient*> m_clients;

	static std::uint16_t s_sequenceNumber;

	Clock* m_clock;
	Timer* m_reliableTimer;

// 	//UDP
// 	UDPSocket* m_UDPSocket = nullptr;
// 	//SNQ for receive
// 	SynchronizedNonblockingQueue<UDPMessage> m_receiveQueue;
// 	//SBQ for sending
// 	SynchronizedBlockingQueue<UDPMessage> m_sendQueue;
// 
// 	std::thread* m_readThread = nullptr;
// 	std::thread* m_writeThread = nullptr;
// 
// 	bool m_isQuitting = false;

	//Package UDP into connection states
	std::vector<ConnectionState*> m_connectionStates;
};