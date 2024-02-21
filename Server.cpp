#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include<bitset>

#pragma comment( lib, "ws2_32.lib" )

using std::vector;
using std::cout;
using std::endl;
using std::cin;

// ポート番号
const unsigned short SERVERPORT = 8888;

// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;

//ありえない値
const int INF = 9999999;

struct PlayerInfo {
	SOCKADDR_IN addr;
	char playerName[MESSAGELENGTH];
	int janken;

	PlayerInfo() {
		janken = INF;
	}
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


	//参加してるクライアントのアドレス情報
	vector<PlayerInfo> playerList_;

	vector<int> janken_;

	int num = 0;

	//ここでレイドに参加するクライアントの参加を待つ
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

		

		//アドレス情報を保存する
		PlayerInfo info;
		info.addr = fromAddr;
		playerList_.push_back(info);
		std::cout << playerList_.size() << std::endl;

		//送信
		ret = sendto(sock, buff, strlen(buff), 0, (struct sockaddr*)&fromAddr, fromlen);
		if (ret != strlen(buff))
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "Join OK " << buff << std::endl;

		if (playerList_.size() >= 3) {

			break;
		}

	}

	//ここのwhileはプレイヤーリストのサイズ分だけ
	int pNum = 0;
	while (playerList_.size() < pNum) {


		//送信
		char buff[MESSAGELENGTH] = playerList_.at(pNum).playerName + "さんのじゃんけんの手を入力してください";
		//SOCKADDR_IN fromAddr;
		int fromlen = sizeof(SOCKADDR_IN);//ここのsizeofに変数のほうを入れてる理由が何かあるのか
		ret = sendto(sock, buff, strlen(buff), 0, (struct sockaddr*)&playerList_.at(pNum), fromlen);
		if (ret != strlen(buff))
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

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

		if (buff == "ぐー") {
			janken_.emplace_back(9);
		}
		else if (buff == "ちょき") {
			janken_.emplace_back(2);
		}
		else if (buff == "ぱー") {
			janken_.emplace_back(4);
		}

	}

	std::string ref_ = { "負け","勝ち" };

	int tmp = 0;
	int size = janken_.size();

	for (int i = 0; i < size; i++) {
		tmp = tmp | janken_.at(i);
	}

	int sTmp = tmp >> 1;//算術シフト

	int result = sTmp & tmp;

	if (result == 7 || result == 0) {
		cout << "引き分け！" << endl;
	}
	else {
		for (int i = 0; i < size; i++) {
			tmp = janken_.at(i);
			std::bitset<4> bit(result & tmp);
			int countBit = bit.count();
			cout << janken_.at(i).getName() << "さんは" << ref_[countBit] << endl;
			if (countBit == 1) {
				janken_.at(i).CountW();
			}
		}
	}


	//ここで勝敗を返す
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

		for (int i = 0; i < addrList_.size(); i++) {

			ret = sendto(sock, buff, strlen(buff), 0, (struct sockaddr*)&addrList_.at(i), fromlen);
			if (ret != strlen(buff))
			{
				std::cout << "sendtoError" << WSAGetLastError() << std::endl;
				return 1;
			}

			std::cout << "sended message " << buff << std::endl;
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