#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

using std::cin;
using std::cout;
using std::endl;
using std::string;

#pragma comment( lib, "ws2_32.lib" )

// ポート番号
const unsigned short SERVERPORT = 8888;

// 送受信するメッセージの最大値
const unsigned int MESSAGELENGTH = 1024;

const string IP = "192.168.42.32";

int main()
{

    int ret;
    // WinSockの初期化	WinSock2.2
    WSADATA wsaData;
    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // if (ret == SOCKET_ERROR)
    if (ret == SOCKET_ERROR)
    {
        std::cout << "InitoError" << WSAGetLastError() << std::endl;
        return 1;
    }

    // ソケットディスクリプタ
    SOCKET sock;

    // UDPソケット作成
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 接続先サーバのIPアドレスを入力させる
    string serverIpAddress;
    cout << "Input Server IPv4 address :";
    cin >> serverIpAddress;
    
    //サーバーからAllOKが来るまでプレイヤーの受付を行う。
    int playerNum = 0;
    while (true)
    {

        // 接続先サーバのソケットアドレス情報格納
        SOCKADDR_IN toAddr;
        const int tolen = sizeof(toAddr);
        memset(&toAddr, 0, sizeof(toAddr));
        toAddr.sin_family = AF_INET;
        toAddr.sin_port = htons(SERVERPORT);
        inet_pton(AF_INET, serverIpAddress.c_str(), &toAddr.sin_addr.s_addr);

        char message[MESSAGELENGTH];	// サーバ側とサイズを合わせること
        SOCKADDR_IN fromAddr;
        int fromlen = sizeof(fromAddr);
        cout << "ユーザー名の入力:";
        cin >> message;
        ret = sendto(sock, message, sizeof(message), 0, (SOCKADDR*)&toAddr, tolen);
        if (ret == SOCKET_ERROR)
        {
            std::cout << "SendtoError" << WSAGetLastError() << std::endl;
            return 1;
        }
        ret = recvfrom(sock, message, sizeof(message), 0, (SOCKADDR*)&fromAddr, &fromlen);

        if (ret == SOCKET_ERROR)
        {
            std::cout << "ResvtoError" << WSAGetLastError() << std::endl;
            return 1;
        }
        else
        {
            std::cout << message << std::endl;
        }

        if (strcmp(message,"AllOK") == 0) {
            break;
        }

        playerNum++;

    }

    //ぐーちょきぱーの指示をもらう
    //{
    //    char message[MESSAGELENGTH];	// サーバ側とサイズを合わせること
    //    SOCKADDR_IN fromAddr;
    //    int fromlen = sizeof(fromAddr);
    //    ret = recvfrom(sock, message, sizeof(message), 0, (SOCKADDR*)&fromAddr, &fromlen);

    //    if (ret == SOCKET_ERROR)
    //    {

    //    }
    //    else
    //    {
    //        std::cout << message << std::endl;
    //    }
    //}

    //出す手を入力
    for(int i=0;i<playerNum;i++)
    {
        // 接続先サーバのソケットアドレス情報格納
        SOCKADDR_IN toAddr;
        const int tolen = sizeof(toAddr);
        memset(&toAddr, 0, sizeof(toAddr));
        toAddr.sin_family = AF_INET;
        toAddr.sin_port = htons(SERVERPORT);
        inet_pton(AF_INET, serverIpAddress.c_str(), &toAddr.sin_addr.s_addr);

        char message[MESSAGELENGTH];	// サーバ側とサイズを合わせること
        SOCKADDR_IN fromAddr;
        int fromlen = sizeof(fromAddr);
        cout << "プレイヤー順に出す手の入力:";
        cin >> message;
        ret = sendto(sock, message, sizeof(message), 0, (SOCKADDR*)&toAddr, tolen);
        if (ret == SOCKET_ERROR)
        {
            std::cout << "SendtoError" << WSAGetLastError() << std::endl;
            return 1;
        }

    }

    //勝敗を聞く
    for (int i = 0; i < playerNum; i++) {


        char message[MESSAGELENGTH];	// サーバ側とサイズを合わせること
        SOCKADDR_IN fromAddr;
        int fromlen = sizeof(fromAddr);
        ret = recvfrom(sock, message, sizeof(message), 0, (SOCKADDR*)&fromAddr, &fromlen);

        if (ret == SOCKET_ERROR)
        {
            std::cout << "initoError" << WSAGetLastError() << std::endl;
            return 1;
        }
        else
        {
            std::cout << message << std::endl;
        }

        if (strcmp(message, "引き分け") == 0) {
            break;
        }

    }

    ret = closesocket(sock);
    if (ret == SOCKET_ERROR)
    {
        std::cout << "ClosetoError" << WSAGetLastError() << std::endl;
        return 1;
    }

    // WinSockの終了処理
    ret = WSACleanup();
    //if (ret == SOCKET_ERROR)
    if (ret != 0)
    {
        std::cout << "CleantoError" << WSAGetLastError() << std::endl;
        return 1;
    }

    return 0;
}