#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <bitset>

#pragma comment( lib, "ws2_32.lib" )

using std::vector;
using std::cout;
using std::cin;
using std::endl;

// ポート番号
const unsigned short SERVERPORT = 8888;

// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;

//ありえない値
const int INF = 9999999;

struct UserData {

	std::string name;
	SOCKADDR_IN userID;
	int janken;
	bool isWin;

	UserData() {
		name = "NONE";
		janken = INF;
		isWin = false;
	}

};

const int ROCK = 9;
const int PAPER = 4;
const int SCISSORS = 2;
 

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
	vector<UserData> playerList_;

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

		UserData data;
		data.name = buff;
		data.userID = fromAddr;

		//アドレス情報を保存する
		playerList_.push_back(data);
		std::cout << "現在のユーザー数" << playerList_.size() << std::endl;

		std::cout << "userName = " << data.name << std::endl;

		//一旦2人以上になったら切るexeファイルから実行するとなぜか止まってくれない
		if (playerList_.size() >= 3) {
			//送信
			char sendChar[MESSAGELENGTH] = "AllOK";


			ret = sendto(sock, sendChar, sizeof(sendChar), 0, (struct sockaddr*)&fromAddr, fromlen);
			if (ret == SOCKET_ERROR)
			{
				std::cout << "sendtoError" << WSAGetLastError() << std::endl;
				return 1;
			}

			std::cout << "Join OK " << std::endl;
			break;
		}

		//送信
		char sendChar[MESSAGELENGTH];

		std::string tmp = buff;
		tmp = tmp + ", Join OK";
		strcpy_s(sendChar, tmp.size() + 1, tmp.c_str());

		ret = sendto(sock, sendChar, sizeof(sendChar), 0, (struct sockaddr*)&fromAddr, fromlen);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "Join OK " << std::endl;
	}

	//ぐーちょきぱーを出させる指示
	//for (int i = 0; i < playerList_.size(); i++) {

	//	//送信
	//	char sendChar[MESSAGELENGTH] = "ぐーorちょきorぱーを入力して";

	//	SOCKADDR_IN fromAddr;
	//	int fromlen = sizeof(fromAddr);
	//	ret = sendto(sock, sendChar, sizeof(sendChar), 0, (struct sockaddr*)&playerList_.at(i).userID, fromlen);
	//	if (ret == SOCKET_ERROR)
	//	{
	//		std::cout << "sendtoError" << WSAGetLastError() << std::endl;
	//		return 1;
	//	}
	//}
	
	//ここのwhileはプレイヤーリストのサイズ分だけ
	int pNum = 0;
	while (playerList_.size() > pNum) {

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

		if (strcmp(buff,"ぐー") == 0) {
			playerList_.at(pNum).janken = ROCK;
		}
		else if (strcmp(buff, "ちょき") == 0) {
			playerList_.at(pNum).janken = SCISSORS;
		}
		else if (strcmp(buff, "ぱー") == 0) {
			playerList_.at(pNum).janken = PAPER;
		}
		else {
			cout << "判定できなかった";
			return 1;
		}

		pNum++;

	}

	std::string ref[2] = {"負け","勝ち"};

	int tmp = 0;
	int size = playerList_.size();

	for (int i = 0; i < size; i++) {
		tmp = tmp | playerList_.at(i).janken;
	}

	int sTmp = tmp >> 1;//算術シフト

	int result = sTmp & tmp;

	bool isDraw = false;

	if (result == 7 || result == 0) {
		cout << "引き分け！" << endl;
		isDraw = true;
	}
	else {
		for (int i = 0; i < size; i++) {
			tmp = playerList_.at(i).janken;
			std::bitset<4> bit(result & tmp);
			int countBit = bit.count();
			cout << playerList_.at(i).name << "さんは" << ref[countBit] << endl;
			if (countBit == 1) {
				playerList_.at(i).isWin = true;//勝ち

			}
			else {
				playerList_.at(i).isWin = false;//負け
			}
		}
	}


	//プレイヤー全員に勝敗を報告
	for (int i = 0; i < playerList_.size(); i++) {
			
		char message[MESSAGELENGTH];

		//引き分けなら
		if (isDraw = true) {
			std::string tmp = "引き分け";
			strcpy_s(message, tmp.size() + 1, tmp.c_str());
			int fromlen = sizeof(SOCKADDR_IN);
			ret = sendto(sock, message, sizeof(message), 0, (struct sockaddr*)&playerList_.at(i).userID, fromlen);
			if (ret == SOCKET_ERROR)
			{
				std::cout << "sendtoError" << WSAGetLastError() << std::endl;
				return 1;
			}
			std::cout << "sended message " << message << std::endl;

			break;
		}

		//勝ち負け
		if (playerList_.at(i).isWin) {
			std::string tmp = playerList_.at(i).name;
			tmp = tmp + "さんの勝ち";
			strcpy_s(message, tmp.size() + 1, tmp.c_str());
		}
		else {
			std::string tmp = playerList_.at(i).name;
			tmp = tmp + "さんの負け";
			strcpy_s(message, tmp.size() + 1, tmp.c_str());
		}


		int fromlen = sizeof(SOCKADDR_IN);
		ret = sendto(sock, message, sizeof(message), 0, (struct sockaddr*)&playerList_.at(i).userID, fromlen);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "sended message " << message << std::endl;
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