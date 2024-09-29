#include <iostream>
#include <winSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>

using namespace std;

#pragma comment (lib , "ws2_32.lib")

bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void InteractWithClient(SOCKET clientSocket, vector<SOCKET>& clients)
{
	//send , recive - client

	//receive

	cout << "client connected" << endl;

	char buffer[4096];

	while (1) {

		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrecvd <= 0)
		{
			cout << "client disconnected " << endl;
			break;
		}

	string message(buffer, bytesrecvd);
	cout << "Message From Client : " << message << endl;

	for (auto client : clients)
	{
		if (client != clientSocket) //this is avoid message echo
		{
			send(client, message.c_str(), message.length(), 0);
		}
		
	}

	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end())
	{
		clients.erase(it);
	}

	closesocket(clientSocket);

}

int main()
{
	if (!Initialize())
	{
		cout << "WINSOCK INITIALIZATION FAILED" << endl;
		return 1;
	}

	cout << "server program" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
	{
		cout << "SOCKET CREATION FAILED" << endl;
		return 1;
	}

	//create address structure//

	int port = 12345;
	sockaddr_in serveraddr;                 //naming//
	serveraddr.sin_family = AF_INET;        //type of address //
	serveraddr.sin_port = htons(port);     //port number//

	//convert ip address (0.0.0.0) to a binary format//

	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
	{
		cout << " SETTING ADDRESS STRUCTURE FAILED " << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind// 

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "BIND FAILED" << endl;
		closesocket(listenSocket);
		WSACleanup;
		return 1;
	}

	//listen//

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "server has started listening on port :" << port << endl;
	vector<SOCKET> clients;

	while (1)
	{
		//accept//
	SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET)
		{
			cout << "INVALID CLIENT SOCKET" << endl;
		};

		//thread
	clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket, std::ref(clients)); 
		t1.detach();
	}
	
	closesocket(listenSocket);


	WSACleanup();

	return 0;
}