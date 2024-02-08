#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

#pragma comment( lib, "ws2_32.lib" )

using std::vector;

// ポート番号
const unsigned short SERVERPORT = 8888;

// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;

struct UserData {

	std::string name;
	SOCKADDR_IN userID;

};

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

	//60フレーム毎に一回全員に送信する情報を（ここではint）
	int flame = 0;

	//参加してるクライアントのアドレス情報
	vector<UserData> addrList_;

	int num = 0;

	//ここでレイドに参加するクライアントの参加を待つ
	while (true)
	{

		//受信
		char buff[MESSAGELENGTH];

		//fromAddrに情報が保存される
		SOCKADDR_IN fromAddr;
		int fromlen = sizeof(fromAddr);
		ret = recvfrom(sock, buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, &fromlen);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "recvtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		/*for (int i = 0; i < addrList_.size(); i++) {

			if (addrList_.at(i) == fromAddr) {

			}
		}*/

		UserData data;
		data.name = buff;
		data.userID = fromAddr;

		//アドレス情報を保存する
		addrList_.push_back(data);
		std::cout << "現在のユーザー数" << addrList_.size() << std::endl;

		std::cout << "userName = " << buff << std::endl;

		//送信
		char sendChar[MESSAGELENGTH] = "Join OK";


		ret = sendto(sock, sendChar, strlen(sendChar), 0, (struct sockaddr*)&fromAddr, fromlen);
		if (ret != strlen(sendChar))
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "Join OK " << std::endl;

		//一旦2人以上になったら切るexeファイルから実行するとなぜか止まってくれない
		if (addrList_.size() >= 2) {

			break;
		}

	}


	//ここで情報を常に受け取り、一定時間毎にすべてのクライアントに情報を返す
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
		

		//1秒ごとに送信 10047エラーが出る。間違ったプロトコルかソケットが呼び出されたときに出るらしい
		if (flame <= 60) {

			for (int i = 0; i < addrList_.size(); i++) {
			
				char message[MESSAGELENGTH] = "TestSend";

				ret = sendto(sock, message, strlen(message), 0, (struct sockaddr*)&addrList_.at(i), fromlen);
				if (ret != strlen(message))
				{
					std::cout << "sendtoError" << WSAGetLastError() << std::endl;
					return 1;
				}

				std::cout << "sended message " << message << std::endl;
			}

			flame = 0;
		}
		

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