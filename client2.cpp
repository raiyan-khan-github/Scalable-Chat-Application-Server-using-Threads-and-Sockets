#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <string>


using namespace std;

#pragma comment (lib, "ws2_32.lib")

bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s)
{
	cout << "Enter Your Chat Name : " << endl;
	string name;
	getline(cin, name);
	string message;

	while (1)
	{
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR)
		{
			cout << "Error Sending Message" << endl;
			break;
		}
		if (message == "quit")
		{
			cout << "Stopiing The Application" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}

void ReceiveMsg(SOCKET s)
{
	char buffer[4096];
	int recvlength;
	string msg = "";

	while (1)
	{
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0)
		{
			cout << "Disconnected From Server" << endl;
			break;
		}
		else
		{
			msg = string(buffer, recvlength);
			cout << msg << endl;
		}
	}

	closesocket(s);
	WSACleanup();
}



int main()
{
	if (!Initialize())
	{
		cout << "INITIALIZE WINSOCK FAILED" << endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		cout << "INVALID SOCKET CREATED" << endl;
	}

	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "not able to connect to server" << endl;
		cout << ": " << WSAGetLastError();
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "successfully connected to server" << endl;

	//we'll need 2 threads

	thread senderthread(SendMsg, s);
	thread receiver(ReceiveMsg, s);

	senderthread.join();
	receiver.join();

	return 0;

}