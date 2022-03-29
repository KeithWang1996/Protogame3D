#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include <stdarg.h>
#include <array>

//Add a command startTCPServer, start TCP Server listening for client connection, port 48000

std::uint16_t NetworkSystem::s_sequenceNumber = 0;

NetworkSystem::NetworkSystem()
	:m_isListening(false)
	, m_timeval{ 00, 00 }
{
	m_clock = new Clock();
	m_reliableTimer = new Timer();
	m_reliableTimer->SetSeconds(m_clock, 0.1);
}

NetworkSystem::~NetworkSystem()
{
	delete m_clock;
	m_clock = nullptr;
	delete m_reliableTimer;
	m_reliableTimer = nullptr;

	m_isListening = false;
	m_timeval = timeval{ 01, 01 };
	if (m_clientSocket != nullptr)
	{
		delete m_clientSocket;
		m_clientSocket = nullptr;
	}
	for (auto it = m_servers.begin(); it != m_servers.end(); ++it)
	{
		delete *it;
	}
	for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		delete *it;
	}

	CloseAllUDPConnectionState();
}

void NetworkSystem::StartUp()
{
	//Initialize winsock
	WSADATA wsaData;
	WORD wVersion MAKEWORD(2, 2);
	int iResult = WSAStartup(wVersion, &wsaData);
	if (iResult != 0)
	{
		g_theConsole->Error(Stringf("Call to WSAStartUp failed %i", WSAGetLastError()));
	}
}

void NetworkSystem::BeginFrame()
{
	//UDP
	ReceiveForAllUDPConnectionState();

	if (!m_servers.empty())
	{
		if (m_clientSocket == nullptr)
		{
			m_clientSocket = m_servers.front()->accept();
		}
		else if (!m_clientSocket->isValid())
		{
			delete m_clientSocket;
			m_clientSocket = m_servers.front()->accept();
		}
		else
		{
			m_clientSocket->setMode(Nonblocking);
			TCPData data = m_clientSocket->receive();
			if (data.getLength() != TCPData::DATA_PENDING && data.getLength() >= 4)
			{
				std::string msg(data.getData(), data.getLength());
				//buffer[data.getLength()] = NULL;
				PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(&msg[0]);
				Packet svrListenMsg;
				svrListenMsg.m_header = *headerPtr;
				svrListenMsg.m_messageText = std::string(&msg[16]);
				//1 for start connection
				//2 for text message
				//3 for end connection
				//4 for start connection packet(client only)
				switch (svrListenMsg.m_header.m_id)
				{
				case 1:
					g_theConsole->PrintString(Rgba8::WHITE, "Game name: " + svrListenMsg.m_messageText);
					//A remote server is connected
					//Send back magic number, ip and port
					g_theEvent->Fire("SendConnectPacketToClient");
					break;
				case 2:
					g_theConsole->PrintString(Rgba8::WHITE, svrListenMsg.m_messageText + " from " + m_clientSocket->GetAddress() + ".");
					//Process message here
					break;
				case 3:
					g_theConsole->PrintString(Rgba8::WHITE, svrListenMsg.m_messageText);
					m_clientSocket->close();
					break;
				default:
					break;
				}
			}
		}
	}
	else if (!m_clients.empty())
	{
		//client loop
		if (m_clientSocket->isDataAvailable())
		{
			TCPData data = m_clientSocket->receive();
			if (data.getLength() != TCPData::DATA_PENDING && data.getLength() >= 4)
			{
				std::string msg(data.getData(), data.getLength());
				//buffer[data.getLength()] = NULL;
				PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(&msg[0]);
				Packet svrListenMsg;
				svrListenMsg.m_header = *headerPtr;
				svrListenMsg.m_messageText = std::string(&msg[16]);
				switch (svrListenMsg.m_header.m_id)
				{
				case 1:
					g_theConsole->PrintString(Rgba8::WHITE, "Game name: " + svrListenMsg.m_messageText);
					break;
				case 2:
					g_theConsole->PrintString(Rgba8::WHITE, svrListenMsg.m_messageText + " from " + m_clientSocket->GetAddress() + ".");
					//Process message here
					break;
				case 3:
					g_theConsole->PrintString(Rgba8::WHITE, "Client disconnecting.");
					break;
				case 4:
				{
					//Send string to client to process
					NamedStrings param;
					param.SetValue("messages", svrListenMsg.m_messageText);
					g_theEvent->Fire("ClientProcessConnectPacket", param);
				}
				break;
				default:
					break;
				}
			}
			else
			{
				StopClient();
			}
		}
	}
	HandleDisconnectedUDPSocket();
}

void NetworkSystem::Update()
{
	if (m_reliableTimer->CheckAndReset())
	{
		for (int i = 0; i < m_connectionStates.size(); ++i)
		{
			m_connectionStates[i]->SendMessageInReliableMap();
		}
	}
}

void NetworkSystem::EndFrame()
{

}

void NetworkSystem::ShutDown()
{
	StopClient();
	StopServer();
	int iResult = WSACleanup();
	if (iResult == SOCKET_ERROR)
	{
		g_theConsole->Error(Stringf("Call to WSACleanUp failed %i", WSAGetLastError()));
	}
	ShutDownAllUDPConnections();
}

void NetworkSystem::StartServer(int port)
{
	if (m_servers.empty())//Only allow one
	{
		m_servers.push_back(new TCPServer(port));
		m_servers.front()->bind();
		m_servers.front()->listen();
	}
}

void NetworkSystem::StartClient(std::string ip, int port)
{
	if (m_clients.empty())//Only allow one
	{
		m_clients.push_back(new TCPClient());
		m_clientSocket = m_clients.front()->connect(ip, (uint16_t)port, Mode::Nonblocking);
		if (m_clientSocket == nullptr)
		{
			delete m_clients.front();
			m_clients.pop_front();
			return;
		}
		m_clientSocket->setMode(Nonblocking);
		//Send Game name
		std::array<char, 256> buffer;
		std::string msg = "Doomstein";
		PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(&buffer[0]);
		headerPtr->m_id = 1;
		headerPtr->m_size = static_cast<uint16_t>(msg.length());
		headerPtr->m_seqNo = s_sequenceNumber++;
		headerPtr->m_magicNumber = 0;
		headerPtr->m_numMsg = 1;
		static uint16_t headerSize = static_cast<uint16_t>(sizeof(PacketHeader));
		for (int i = 0; i < msg.length(); ++i)
		{
			buffer[headerSize + i] = msg[i];
		}
		m_clientSocket->send(&buffer[0], headerSize + headerPtr->m_size);
	}
}

void NetworkSystem::SendTextMessage(std::string msg, std::uint32_t magicNumber, std::uint16_t numMessage, std::uint16_t id)//TO BE UPDATED 
{
	std::array<char, 256> buffer;
	PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(&buffer[0]);
	headerPtr->m_id = id;
	headerPtr->m_seqNo = s_sequenceNumber++;
	headerPtr->m_magicNumber = magicNumber;
	headerPtr->m_numMsg = numMessage;
	headerPtr->m_size = static_cast<uint16_t>(msg.length());
	static uint16_t headerSize = static_cast<uint16_t>(sizeof(PacketHeader));
	for (int i = 0; i < msg.length(); ++i)
	{
		buffer[headerSize + i] = msg[i];
	}
	m_clientSocket->send(&buffer[0], headerSize + headerPtr->m_size);
}

void NetworkSystem::StopServer()
{
	if (!m_servers.empty())
	{
		m_clientSocket->close();
		m_servers.front()->stopListen();
		delete m_servers.front();
		m_servers.pop_front();
	}
}

void NetworkSystem::StopClient()
{
	if (!m_clients.empty())
	{
		std::string msg = "Client disconnecting.";
		std::array<char, 256> buffer;
		PacketHeader* headerPtr = reinterpret_cast<PacketHeader*>(&buffer[0]);
		headerPtr->m_id = 3;
		headerPtr->m_size = static_cast<uint16_t>(msg.length());
		headerPtr->m_seqNo = s_sequenceNumber++;
		headerPtr->m_magicNumber = 0;
		headerPtr->m_numMsg = 1;
		static uint16_t headerSize = static_cast<uint16_t>(sizeof(PacketHeader));
		for (int i = 0; i < msg.length(); ++i)
		{
			buffer[headerSize + i] = msg[i];
		}

		m_clientSocket->send(&buffer[0], headerSize + headerPtr->m_size);
		m_clientSocket->shutdown();
		m_clientSocket->close();
		delete m_clients.front();
		m_clients.pop_front();
	}
}

void NetworkSystem::HandleDisconnectedUDPSocket()
{
	for (int i = 0; i < m_connectionStates.size(); ++i)
	{
		if (m_connectionStates[i]->m_errorWatcher >= 10)
		{
			delete m_connectionStates[i];
			m_connectionStates.erase(m_connectionStates.begin() + i);
			NamedStrings args;
			args.SetValue("index", std::to_string(i));
			g_theEvent->Fire("CloseClient", args);
		}
	}
}

std::string NetworkSystem::GetClientAddress()
{
	return m_clientSocket->GetAddress();
}

void NetworkSystem::CreateUDPConnectionState(int bindPort, int sendToPort, std::string host)
{
	CloseUDPConnectionState(bindPort);
	ConnectionState* connectionState = new ConnectionState();
	connectionState->m_isQuitting = false;

	connectionState->m_UDPSocket = new UDPSocket(host, sendToPort);
	connectionState->m_UDPSocket->bind(bindPort);
	//New off two thread
	connectionState->m_readThread = new std::thread(&ConnectionState::ReadThreadMain, connectionState);
	connectionState->m_writeThread = new std::thread(&ConnectionState::WriteThreadMain, connectionState);
	g_theConsole->PrintString(Rgba8::WHITE, Stringf("Start UDP Socket at %s, bind port: %i, send to port: %i", host.c_str(), bindPort, sendToPort));

	m_connectionStates.push_back(connectionState);
}

void NetworkSystem::CloseUDPConnectionState(int bindPort)
{
	for (int i = 0; i < m_connectionStates.size(); ++i)
	{
		if (m_connectionStates[i]->m_bindPort == bindPort)
		{
			delete m_connectionStates[i];
			m_connectionStates.erase(m_connectionStates.begin() + i);
		}
	}
}

void NetworkSystem::CloseAllUDPConnectionState()
{
	for (int i = 0; i < m_connectionStates.size(); ++i)
	{
		delete m_connectionStates[i];
	}
	m_connectionStates.clear();
}

void NetworkSystem::ReceiveForAllUDPConnectionState()
{
	for (int i = 0; i < m_connectionStates.size(); ++i)
	{
		m_connectionStates[i]->ReceiveFromReceiveQueue();
	}
}

void NetworkSystem::ShutDownAllUDPConnections()
{
	for (int i = 0; i < m_connectionStates.size(); ++i)
	{
		m_connectionStates[i]->m_isQuitting = true;
	}
}

void NetworkSystem::SendUDPMessage(std::string message, int UDPIndex, std::uint32_t magicNumber, std::uint16_t id, std::uint16_t numMsg)
{
	if (m_connectionStates.empty())
	{
		return;
	}
	m_connectionStates[UDPIndex]->SendUDPMessage(message, magicNumber, id, numMsg);
}

void NetworkSystem::SendReliableUDPMessage(std::string message, int UDPIndex, std::uint32_t magicNumber, std::uint16_t numMsg)
{
	if (m_connectionStates.empty())
	{
		return;
	}
	m_connectionStates[UDPIndex]->SendReliableUDPMessage(message, magicNumber, numMsg);
}

std::uint16_t ConnectionState::s_sequenceNumber;

ConnectionState::~ConnectionState()
{
	m_isQuitting = true;
	if (m_UDPSocket)
	{
		g_theConsole->PrintString(Rgba8::WHITE, Stringf("Closing UDP socket..."));
		m_UDPSocket->close();
		delete m_UDPSocket;
		m_UDPSocket = nullptr;

		m_readThread->join();
		delete m_readThread;
		m_readThread = nullptr;

		m_sendQueue.notify_all();
		m_writeThread->join();
		delete m_writeThread;
		m_writeThread = nullptr;
	}
}

void ConnectionState::ReceiveFromReceiveQueue()
{
	if (!m_UDPSocket)
	{
		return;
	}
	//Read message from m_receiveQueue
	UDPMessage message = m_receiveQueue.pop();
	while (std::get<5>(message).size() > 0)
	{
		int length = std::get<2>(message);
		if (length <= 0)
		{
			return;
		}
		std::string data = std::get<5>(message);

		//Handle data differently based on id
		//2 for text message
		//5 for create remote client(server only)
		//6 for create player entities(client only)
		std::uint16_t id = std::get<0>(message);
		std::uint16_t seqNo = std::get<3>(message);
		switch (id)
		{
		case 2:
			if (data != "")
			{
				g_theConsole->PrintString(Rgba8::WHITE, Stringf("Message: %s", data.c_str()));
			}
			break;
		case 5:
			if (data != "")
			{
				g_theConsole->PrintString(Rgba8::WHITE, Stringf("Message: %s", data.c_str()));
				NamedStrings args;
				args.SetValue("id", std::to_string(id));
				std::uint32_t magicNumber = std::get<1>(message);
				args.SetValue("magicNumber", std::to_string(magicNumber));
				args.SetValue("message", std::get<5>(message));
				args.SetValue("port", std::to_string(m_UDPSocket->m_port));//Send to port
				args.SetValue("seqno", std::to_string(seqNo));
				g_theEvent->Fire("ServerProcessUDPMessage", args);
			}
			break;
		case 10: // All player health send from server to client
			if (data != "")
			{
				//g_theConsole->PrintString(Rgba8::WHITE, Stringf("Message: %s", data.c_str()));
				NamedStrings args;
				args.SetValue("id", std::to_string(id));
				std::uint32_t magicNumber = std::get<1>(message);
				args.SetValue("magicNumber", std::to_string(magicNumber));
				args.SetValue("message", std::get<5>(message));
				args.SetValue("port", std::to_string(m_UDPSocket->m_port));//Send to port
				args.SetValue("seqno", std::to_string(seqNo));
				g_theEvent->Fire("ServerProcessUDPMessage", args);
			}
			break;
		case 11:
		{
			//Receive ACK and remove reliable message from map.
			std::uint16_t receivedSeqNo = (std::uint16_t)stoi(data);
			for (int i = 0; i < m_reliableMap.size(); ++i)
			{
				if (std::get<3>(m_reliableMap[i]) == receivedSeqNo)
				{
					m_reliableMap.erase(m_reliableMap.begin() + i);
				}
			}
		}
		break;
		default:
			//Send everything to game and let game decide what to do.
		{
			NamedStrings args;
			args.SetValue("id", std::to_string(id));
			std::uint32_t magicNumber = std::get<1>(message);
			args.SetValue("magicNumber", std::to_string(magicNumber));
			args.SetValue("message", std::get<5>(message));
			args.SetValue("port", std::to_string(m_UDPSocket->m_port));//Send to port
			g_theEvent->Fire("ServerProcessUDPMessage", args);
		}
		break;
		}
		message = m_receiveQueue.pop();
	}
	
}

void ConnectionState::PushToWriteQueue(UDPMessage message)
{
	m_sendQueue.push(message);
}

void ConnectionState::SendUDPMessage(std::string message, std::uint32_t magicNumber, std::uint16_t id, std::uint16_t numMsg)
{
	UDPMessageHeader header;
	header.id = id;
	header.length = static_cast<std::uint16_t>(message.size() + 1);
	header.seqNo = s_sequenceNumber++;
	header.magicNumber = magicNumber;
	header.numMsg = numMsg;
	UDPMessage udpMessage = std::make_tuple(header.id, header.magicNumber, header.length, header.seqNo, header.numMsg, message);
	PushToWriteQueue(udpMessage);
}

void ConnectionState::SendReliableUDPMessage(std::string message, std::uint32_t magicNumber, std::uint16_t numMsg)
{
	UDPMessageHeader header;
	header.id = 10;
	header.length = static_cast<std::uint16_t>(message.size() + 1);
	header.seqNo = s_sequenceNumber++;
	header.magicNumber = magicNumber;
	header.numMsg = numMsg;
	UDPMessage udpMessage = std::make_tuple(header.id, header.magicNumber, header.length, header.seqNo, header.numMsg, message);
	m_reliableMap.push_back(udpMessage);
}

void ConnectionState::SendMessageInReliableMap()
{
	for (int i = 0; i < m_reliableMap.size(); ++i)
	{
		PushToWriteQueue(m_reliableMap[i]);
	}
}

void ConnectionState::ReadThreadMain()
{
	while (!m_isQuitting)
	{
		UDPMessageHeader const* pMsg = nullptr;
		std::string dataStr;
		std::size_t length = 0;

		length = m_UDPSocket->receive();
		if (length == -1)
		{
			m_errorWatcher++;
		}
		else {
			m_errorWatcher = 0;
		}
// 		if (m_errorWatcher > 3)
// 		{
// 			g_theConsole->PrintString(Rgba8::WHITE, Stringf("Error: %i", m_errorWatcher));
// 		}
		dataStr.clear();
		if (length > 0 && length != ~0)
		{
			auto& buffer = m_UDPSocket->receiveBuffer();

			//Copy message header
			pMsg = reinterpret_cast<UDPMessageHeader const*>(&buffer[0]);
			if (pMsg->length > 0)
			{
				dataStr = &buffer[sizeof(UDPMessageHeader)];
				UDPMessage message = std::make_tuple(pMsg->id, pMsg->magicNumber, pMsg->length, pMsg->seqNo, pMsg->numMsg, dataStr);
				m_receiveQueue.push(message);
			}
		}
	}
}

void ConnectionState::WriteThreadMain()
{
	while (!m_isQuitting)
	{
		if (!m_UDPSocket)
		{
			continue;
		}
		UDPMessage message = m_sendQueue.pop();
		while (std::get<5>(message).size() > 0)
		{
			UDPMessageHeader header;
			header.id = std::get<0>(message);
			header.magicNumber = std::get<1>(message);
			header.length = std::get<2>(message);
			header.seqNo = std::get<3>(message);
			header.numMsg = std::get<4>(message);
			auto& buffer = m_UDPSocket->sendBuffer();
			*reinterpret_cast<UDPMessageHeader*>(&buffer[0]) = header;

			//Copy the data into the buffer
			std::memcpy(&(m_UDPSocket->sendBuffer()[sizeof(UDPMessageHeader)]), std::get<5>(message).c_str(), header.length);

			m_UDPSocket->sendBuffer()[sizeof(UDPMessageHeader) + header.length] = NULL;

			m_UDPSocket->send(sizeof(UDPMessageHeader) + header.length + 1);
			message = m_sendQueue.pop();
		}
	}
}