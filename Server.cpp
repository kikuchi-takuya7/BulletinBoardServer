#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment( lib, "ws2_32.lib" )


// ポート番号
const unsigned short SERVERPORT = 8888;

// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;


int main()
{
	int ret;

	// WinSockの初期化	WinSock2.2
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret == SOCKET_ERROR)
	{
		std::cout << "initoError" << WSAGetLastError() << std::endl;
		return 1;
	}
	
	


	// ソケットディスクリプタ
	SOCKET sock;

	// UDPソケット作成
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// ソケット作成に失敗
	if (sock == INVALID_SOCKET)
	{
		std::cout << "socktoError" << WSAGetLastError() << std::endl;
		return 1;
	}

	// ソケットに使用するポート番号を設定
	SOCKADDR_IN bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bindAddr.sin_port = htons(SERVERPORT);

	ret = bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr));
	if (ret == SOCKET_ERROR)
	{
		std::cout << "bindtoError" << WSAGetLastError() << std::endl;
		return 1;
	}

	std::cout << "ok" << std::endl;

	// ループ
	while (true)
	{
		//受信
		char buff[MESSAGELENGTH];
		SOCKADDR_IN fromAddr;
		int fromlen = sizeof(fromAddr);
		ret = recvfrom(sock, buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, &fromlen);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "recvtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "recv message = " << buff << std::endl;
		

		//送信
		ret = sendto(sock, buff, strlen(buff), 0, (struct sockaddr*)&fromAddr, fromlen);
		if (ret != strlen(buff))
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "sended message " << buff << std::endl;

	}


	// ここまでこないけど、ソケット破棄
	ret = closesocket(sock);
	if (ret == SOCKET_ERROR)
	{
		/*
			エラー処理
		*/
	}

	// WinSockの終了処理
	ret = WSACleanup();
	if (ret == SOCKET_ERROR)
	{
		/*
			エラー処理
		*/
	}


	return 0;
}