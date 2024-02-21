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

// �|�[�g�ԍ�
const unsigned short SERVERPORT = 8888;

// ����M���郁�b�Z�[�W�̍ő�l
const unsigned int MESSAGELENGTH = 1024;

//���肦�Ȃ��l
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
	vector<PlayerInfo> playerList_;

	vector<int> janken_;

	int num = 0;

	//�����Ń��C�h�ɎQ������N���C�A���g�̎Q����҂�
	while (true)
	{

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

		

		//�A�h���X����ۑ�����
		PlayerInfo info;
		info.addr = fromAddr;
		playerList_.push_back(info);
		std::cout << playerList_.size() << std::endl;

		//���M
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

	//������while�̓v���C���[���X�g�̃T�C�Y������
	int pNum = 0;
	while (playerList_.size() < pNum) {


		//���M
		char buff[MESSAGELENGTH] = playerList_.at(pNum).playerName + "����̂���񂯂�̎����͂��Ă�������";
		//SOCKADDR_IN fromAddr;
		int fromlen = sizeof(SOCKADDR_IN);//������sizeof�ɕϐ��̂ق������Ă闝�R����������̂�
		ret = sendto(sock, buff, strlen(buff), 0, (struct sockaddr*)&playerList_.at(pNum), fromlen);
		if (ret != strlen(buff))
		{
			std::cout << "sendtoError" << WSAGetLastError() << std::endl;
			return 1;
		}

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
			janken_.emplace_back(9);
		}
		else if (buff == "���傫") {
			janken_.emplace_back(2);
		}
		else if (buff == "�ρ[") {
			janken_.emplace_back(4);
		}

	}

	std::string ref_ = { "����","����" };

	int tmp = 0;
	int size = janken_.size();

	for (int i = 0; i < size; i++) {
		tmp = tmp | janken_.at(i);
	}

	int sTmp = tmp >> 1;//�Z�p�V�t�g

	int result = sTmp & tmp;

	if (result == 7 || result == 0) {
		cout << "���������I" << endl;
	}
	else {
		for (int i = 0; i < size; i++) {
			tmp = janken_.at(i);
			std::bitset<4> bit(result & tmp);
			int countBit = bit.count();
			cout << janken_.at(i).getName() << "�����" << ref_[countBit] << endl;
			if (countBit == 1) {
				janken_.at(i).CountW();
			}
		}
	}


	//�����ŏ��s��Ԃ�
	while (true)
	{
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