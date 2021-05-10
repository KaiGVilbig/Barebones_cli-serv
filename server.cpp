#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <future>
#include <windows.h>

#pragma comment (lib, "ws2_32.lib")

int main()
{
	// Init winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
		return 0;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return 0;
	}

	// Bind an ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(90000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Client socket invalid. Quitting...\n";
		return 0;
	}

	char host[NI_MAXHOST];		// client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buf[4096];

	// Wait for client to send data

	while (true)
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		int bytesRecieved = recv(clientSocket, buf, 4096, 0);
		if (bytesRecieved == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). Quitting...\n";
			break;
		}

		if (bytesRecieved == 0)
		{
			std::cout << "Client disconnected\n";
			break;
		}
		else
		{
			std::cout << "<" << host << "> " << buf << std::endl;
		}

		// Echo message back to client
		send(clientSocket, buf, bytesRecieved + 1, 0);
	}

	// Close the socket
	closesocket(clientSocket);

	// Clean up winsock
	WSACleanup();

	return 0;
}