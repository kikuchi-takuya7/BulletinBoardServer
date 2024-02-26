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

// �|�[�g�ԍ�
const unsigned short SERVERPORT = 8888;

// ����M���郁�b�Z�[�W�̍ő�l
const unsigned int MESSAGELENGTH = 1024;

//���肦�Ȃ��l
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

	// WinSock�̏�����	WinSock2.2
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret == SOCKET_ERROR)
	{
		std::cout << "initoError" << WSAGetLastError() << std::endl;
		return 1;
	}

	// �\�P�b�g�f�B�X�N���v�^
	SOCKET sock;

	// UDP�\�P�b�g�쐬
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// �\�P�b�g�쐬�Ɏ��s
	if (sock == INVALID_SOCKET)
	{
		std::cout << "socktoError" << WSAGetLastError() << std::endl;
		return 1;
	}

	// �\�P�b�g�Ɏg�p����|�[�g�ԍ���ݒ�
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

	//�Q�����Ă�N���C�A���g�̃A�h���X���
	vector<UserData> playerList_;

	int num = 0;

	//�����Ń��C�h�ɎQ������N���C�A���g�̎Q����҂�
	while (true)
	{
		//��M
		char buff[MESSAGELENGTH];

		//fromAddr�ɏ�񂪕ۑ������
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

		//�A�h���X����ۑ�����
		playerList_.push_back(data);
		std::cout << "���݂̃��[�U�[��" << playerList_.size() << std::endl;

		std::cout << "userName = " << data.name << std::endl;

		//���M
		char sendChar[MESSAGELENGTH];
		snprintf(sendChar, sizeof(sendChar), ", Join OK", buff);


		ret = sendto(sock, sendChar, strlen(sendChar), 0, (struct sockaddr*)&fromAddr, fromlen);
		if (ret != strlen(sendChar))
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

		std::cout << "Join OK " << std::endl;

		//��U2�l�ȏ�ɂȂ�����؂�exe�t�@�C��������s����ƂȂ����~�܂��Ă���Ȃ�
		if (playerList_.size() >= 2) {

			break;
		}

	}

	//������while�̓v���C���[���X�g�̃T�C�Y������
	int pNum = 0;
	while (playerList_.size() < pNum) {

		//��M
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

		if (buff == "���[") {
			playerList_.at(pNum).janken = ROCK;
		}
		else if (buff == "���傫") {
			playerList_.at(pNum).janken = SCISSORS;
		}
		else if (buff == "�ρ[") {
			playerList_.at(pNum).janken = PAPER;
		}

		if (playerList_.empty()) {
			break;
		}

	}

	std::string ref = { "����","����" };

	int tmp = 0;
	int size = playerList_.size();

	for (int i = 0; i < size; i++) {
		tmp = tmp | playerList_.at(i).janken;
	}

	int sTmp = tmp >> 1;//�Z�p�V�t�g

	int result = sTmp & tmp;

	if (result == 7 || result == 0) {
		cout << "���������I" << endl;
	}
	else {
		for (int i = 0; i < size; i++) {
			tmp = playerList_.at(i).janken;
			std::bitset<4> bit(result & tmp);
			int countBit = bit.count();
			cout << playerList_.at(i).name << "�����" << ref[countBit] << endl;
			if (countBit == 1) {
				playerList_.at(i).isWin = true;//����
			}
			else {
				playerList_.at(i).isWin = false;//����.�O�̂��ߏ�����
			}
		}
	}


	//�v���C���[�S���ɏ��s���
	while (true)
	{
		for (int i = 0; i < playerList_.size(); i++) {
			
			char message[MESSAGELENGTH];

			if (playerList_.at(i).isWin) {
				snprintf(message, sizeof(message), ", ����", playerList_.at(i).name.c_str());
			}
			else {
				snprintf(message, sizeof(message), ", ����", playerList_.at(i).name.c_str());
			}


			int fromlen = sizeof(SOCKADDR_IN);
			ret = sendto(sock, message, strlen(message), 0, (struct sockaddr*)&playerList_.at(i).userID, fromlen);
			if (ret != strlen(message))
			{
				std::cout << "sendtoError" << WSAGetLastError() << std::endl;
				return 1;
			}

			std::cout << "sended message " << message << std::endl;
		}

	}


	// �����܂ł��Ȃ����ǁA�\�P�b�g�j��
	ret = closesocket(sock);
	if (ret == SOCKET_ERROR)
	{
		/*
			�G���[����
		*/
	}

	// WinSock�̏I������
	ret = WSACleanup();
	if (ret == SOCKET_ERROR)
	{
		/*
			�G���[����
		*/
	}


	return 0;
}