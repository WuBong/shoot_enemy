// ������Ʈ ���� �ֽ� C++ �ʾ��� ���
// ISO C17�� ����
//cout ���� printf�� �� ������ ����� �Ҷ� printf 
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>

#define PORT	5500 /*������ Ŭ���̾�Ʈ���� �����͸� �ְ� ���� ��Ʈ��ȣ*/
#define PACKET_SIZE 1024
#define MAP_SIZE 5



#pragma comment (lib, "ws2_32.lib") //ws2_32.lib ���̺귯�� ��ũ
#pragma warning(disable:4996)

using namespace std;

WSADATA wsdata;
SOCKET clientSocket; /* ������ ������ Ŭ���̾�Ʈ ���� ���� ����*/
SOCKADDR_IN serverAddress; /* ���������� �ּ������� ���� �ϴ� ����ü ���� ���� */



int map[11][11] = { 0, };
int ClientNumber=0; //�������� ���� ����Ŭ�� ��ȣ
char fromServer[PACKET_SIZE];
int foggy = 0; //0 to ��Ȱ����, 1 to �Ȱ����

void data_send() {
	char buf[PACKET_SIZE];
	for (;;) {
		//send(clientSocket, toServer, strlen(toServer) + 1, 0);
		ZeroMemory(buf, PACKET_SIZE);
		int key = _getch();
		key = htonl(key);
		send(clientSocket, (char*)&key, sizeof(key), 0);
		Sleep(100);
	}
}

void data_recv_map() {
	for (;;)
	{
		char rec_integer[MAP_SIZE];
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				ZeroMemory(rec_integer, MAP_SIZE);
				recv(clientSocket, rec_integer, MAP_SIZE, 0);
				map[i][j] = atoi(rec_integer);
			}
		}
		Sleep(100);
	}

}//atoi itoa ���

void data_recv_map2() {		//���� �� �������� �ް� ����Ʈ�ϴ� �κ��� ��������.
	for (;;)
	{
		int temp = 0;
		
		char rec_integer[MAP_SIZE];
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				ZeroMemory(rec_integer, MAP_SIZE);
				ZeroMemory(&temp, sizeof(temp));
				recv(clientSocket, (char*)&temp, MAP_SIZE, 0);
				map[i][j] = ntohl(temp);
			}
		}

		Sleep(100);
	}

}

void Cli_Start()
{
	/*WS_32.DLL �� �ʱ�ȭ */
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		cout << "WS2_32.DLL �� �ʱ�ȭ �ϴµ� �����߽��ϴ�. " << endl;
		return ;
	}

	cout << "WS_32.DLL �� �ʱ�ȭ �ϴµ� ���� �߽��ϴ� " << endl;
	/*socket �Լ��� �̿��ؼ� clientSocket�� ���� */
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	ZeroMemory(&serverAddress, sizeof(serverAddress));

	/* serverAddress �� �ʿ��� �ּ� ������ ���� �մϴ�.*/
	serverAddress.sin_family = AF_INET; /*������ �ּ� ü�踦 ���� �մϴ�.	TCP/IP ���������� ��� ���ͳ� ����̹Ƿ� AF_INET �� �����մϴ�.*/
	serverAddress.sin_port = htons(PORT); /*���� ������ ��Ʈ�� ���� �մϴ�.  */
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");/*������ IP�� ���� �մϴ�.*/
	/*serverAddress�� ����� ������ ������ �̿��ؼ� ������ �����մϴ�.*/
	if ((connect(clientSocket, (SOCKADDR*)(&serverAddress), sizeof(serverAddress)) != 0))
	{
		cout << "������ �����ϴµ� ���� �߽��ϴ� " << endl;
		return ;
	}
	cout << "������ ���� �߽��ϴ� " << endl;

	int temp = 0;


	recv(clientSocket, fromServer, sizeof(int) + 1, 0);
	cout << "i'm client : " << fromServer << endl;
	recv(clientSocket, (char*)&temp, sizeof(int), 0);
	ClientNumber = ntohl(temp);
	cout << ClientNumber << endl;

	//thread(data_recv_map).detach();
	thread(data_recv_map2).detach();

	thread(data_send).detach();

	return ;
}



class printmap {
private:
	int foggy = 0;

public:
	void map_none_x1() { //�Ȱ���忡�� Ŭ��0���� �ƹ��͵� �Ⱥ��ϋ�
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 3) {
				printf("��");
				}
				else if (map[i][j] == 4) {
				printf("��");
				}
				else if (map[i][j] == 5) {
				printf("��");
				}
				else if (map[i][j] == 6) {
				printf("��");
				}
				else {
					printf("��");
				}
			}
			printf("\n");
		}
	}

	void map_none_x2() { //�Ȱ���忡�� Ŭ��1���� �ƹ��͵� �Ⱥ��ϋ�
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 2) { //���� 
					printf("o");
				}
				//�÷��̾�1 789 10
				else if (map[i][j] == 7) {
					printf("��");
				}
				else if (map[i][j] == 8) {
					printf("��");
				}
				else if (map[i][j] == 9) {
					printf("��");
				}
				else if (map[i][j] == 10) {
					printf("��");
				}
				else {
					printf("��");
				}
			}
			printf("\n");
		}
	}

	void map_none_x3() { //�Ȱ���忡�� Ŭ��2���� �ƹ��͵� �Ⱥ��϶�
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 2) { //���� 
					printf("o");
				}

				//�÷��̾�2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("��");
				}
				else if (map[i][j] == 12) {
					printf("��");
				}
				else if (map[i][j] == 13) {
					printf("��");
				}
				else if (map[i][j] == 14) {
					printf("��");
				}
				else {
					printf("��");
				}
			}
			printf("\n");
		}
	}

	void map_x1_x2() { //�Ȱ���忡�� Ŭ��0���� 1���� ���϶�
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 2) { //���� 
					printf("o");
				}
				//�÷��̾�0 �����¿� 3456  
				else if (map[i][j] == 3) {
					printf("��");
				}
				else if (map[i][j] == 4) {
					printf("��");
				}
				else if (map[i][j] == 5) {
					printf("��");
				}
				else if (map[i][j] == 6) {
					printf("��");
				}
				//�÷��̾�1 789 10
				else if (map[i][j] == 7) {
					printf("��");
				}
				else if (map[i][j] == 8) {
					printf("��");
				}
				else if (map[i][j] == 9) {
					printf("��");
				}
				else if (map[i][j] == 10) {
					printf("��");
				}
				else {
					printf("��");
				}
			}
			printf("\n");
		}
	}

	void map_x1_x3() { //�Ȱ���忡�� Ŭ��0���� Ŭ��2���� ���϶�
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 2) { //���� 
					printf("o");
				}
				//�÷��̾�0 �����¿� 3456  
				else if (map[i][j] == 3) {
					printf("��");
				}
				else if (map[i][j] == 4) {
					printf("��");
				}
				else if (map[i][j] == 5) {
					printf("��");
				}
				else if (map[i][j] == 6) {
					printf("��");
				}

				//�÷��̾�2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("��");
				}
				else if (map[i][j] == 12) {
					printf("��");
				}
				else if (map[i][j] == 13) {
					printf("��");
				}
				else if (map[i][j] == 14) {
					printf("��");
				}
				else {
					printf("��");
				}
			}
			printf("\n");
		}
	}
	
	void map_x2_x3() //�Ȱ���忡�� Ŭ��1���� 2���� ���϶�
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 2) { //���� 
					printf("o");
				}
	
				//�÷��̾�1 789 10
				else if (map[i][j] == 7) {
					printf("��");
				}
				else if (map[i][j] == 8) {
					printf("��");
				}
				else if (map[i][j] == 9) {
					printf("��");
				}
				else if (map[i][j] == 10) {
					printf("��");
				}
				//�÷��̾�2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("��");
				}
				else if (map[i][j] == 12) {
					printf("��");
				}
				else if (map[i][j] == 13) {
					printf("��");
				}
				else if (map[i][j] == 14) {
					printf("��");
				}
				else {
					printf("��");
				}
			}
			printf("\n");
		}
	}

	void map_x1_x2_hori(int client_num) { //�Ȱ���忡�� Ŭ��0���� 1���� ���϶� 
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(client_num);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //��
				printf("��");
			}

			//�÷��̾�0 �����¿� 3456  
			else if (map[x][i] == 3) {
				printf("��");
			}
			else if (map[x][i] == 4) {
				printf("��");
			}
			else if (map[x][i] == 5) {
				printf("��");
			}
			else if (map[x][i] == 6) {
				printf("��");
			}
			//�÷��̾�1 789 10
			else if (map[x][i] == 7) {
				printf("��");
			}
			else if (map[x][i] == 8) {
				printf("��");
			}
			else if (map[x][i] == 9) {
				printf("��");
			}
			else if (map[x][i] == 10) {
				printf("��");
			}
			else {
				printf("  ");
			}
		}
			printf("\n");

			for (int i = 0; i < 10; i++) {
				printf("��");
			}

	} 

	void map_x1_x3_hori(int client_num) { //�Ȱ���忡�� Ŭ��0���� 2���� ���϶�
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(client_num);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //��
				printf("��");
			}

			//�÷��̾�0 �����¿� 3456  
			else if (map[x][i] == 3) {
				printf("��");
			}
			else if (map[x][i] == 4) {
				printf("��");
			}
			else if (map[x][i] == 5) {
				printf("��");
			}
			else if (map[x][i] == 6) {
				printf("��");
			}
			else if (map[x][i] == 11) {
				printf("��");
			}
			else if (map[x][i] == 12) {
				printf("��");
			}
			else if (map[x][i] == 13) {
				printf("��");
			}
			else if (map[x][i] == 14) {
				printf("��");
			}

			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("��");
		}

	}

	void map_x2_x3_hori(int client_num) { //�Ȱ���忡�� Ŭ��1���� 2���� ���϶�
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(client_num);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //��
				printf("��");
			}
			//�÷��̾�1 789 10
			else if (map[x][i] == 7) {
				printf("��");
			}
			else if (map[x][i] == 8) {
				printf("��");
			}
			else if (map[x][i] == 9) {
				printf("��");
			}
			else if (map[x][i] == 10) {
				printf("��");
			}
			//�÷��̾�2
			else if (map[x][i] == 11) {
				printf("��");
			}
			else if (map[x][i] == 12) {
				printf("��");
			}
			else if (map[x][i] == 13) {
				printf("��");
			}
			else if (map[x][i] == 14) {
				printf("��");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("��");
		}

	}

	void map_none_hori_x1() { //�Ȱ���忡�� Ŭ��1���� �ƹ��͵� �Ⱥ��϶�
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(0);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //��
				printf("��");
			}

			//�÷��̾�0 �����¿� 3456  
			else if (map[x][i] == 3) {
				printf("��");
			}
			else if (map[x][i] == 4) {
				printf("��");
			}
			else if (map[x][i] == 5) {
				printf("��");
			}
			else if (map[x][i] == 6) {
				printf("��");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("��");
		}

		printf("\n");

	}

	void map_none_hori_x2() { //�Ȱ���忡�� Ŭ��1���� �ƹ��͵� �Ⱥ��϶�
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(1);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //��
				printf("��");
			}

			//�÷��̾�0 �����¿� 3456  
			else if (map[x][i] == 7) {
				printf("��");
			}
			else if (map[x][i] == 8) {
				printf("��");
			}
			else if (map[x][i] == 9) {
				printf("��");
			}
			else if (map[x][i] == 10) {
				printf("��");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("��");
		}

		printf("\n");

	}

	void map_none_hori_x3() { //�Ȱ���忡�� Ŭ��2���� �ƹ��͵� �Ⱥ��϶�
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(2);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //��
				printf("��");
			}


			else if (map[x][i] == 11) {
				printf("��");
			}
			else if (map[x][i] == 12) {
				printf("��");
			}
			else if (map[x][i] == 13) {
				printf("��");
			}
			else if (map[x][i] == 14) {
				printf("��");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("��");
		}

		printf("\n");

	}

	void basic_print_map() //cout ���� printf�� �� ����.
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("��");
				}
				else if (map[i][j] == 1) { //��
					printf("��");
				}
				else if (map[i][j] == 2) { //���� 
					printf("o");
				}
				//�÷��̾�0 �����¿� 3456  
				else if (map[i][j] == 3) {
					printf("��");
				}
				else if (map[i][j] == 4) {
					printf("��");
				}
				else if (map[i][j] == 5) {
					printf("��");
				}
				else if (map[i][j] == 6) {
					printf("��");
				}
				//�÷��̾�1 789 10
				else if (map[i][j] == 7) {
					printf("��");
				}
				else if (map[i][j] == 8) {
					printf("��");
				}
				else if (map[i][j] == 9) {
					printf("��");
				}
				else if (map[i][j] == 10) {
					printf("��");
				}
				//�÷��̾�2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("��");
				}
				else if (map[i][j] == 12) {
					printf("��");
				}
				else if (map[i][j] == 13) {
					printf("��");
				}
				else if (map[i][j] == 14) {
					printf("��");
				}
			}
			printf("\n");
		}
	}

	int findplayer_x(int client_num) //�ش�Ŭ���� ��ġ�� ã����
	{
		if (client_num == 0) {
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (map[i][j] >= 3 && map[i][j] <= 6)
						return i;
				}
			}
		}

		else if (client_num == 1) {
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (map[i][j] >= 7 && map[i][j] <= 10)
						return i;
				}
			}
		}

		else if (client_num == 2) {
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (map[i][j] >= 11 && map[i][j] <= 14)
						return i;
				}
			}
		}
		return 100; //�ƹ��͵� ��ã���� 100�� ��ȯ �� �÷��̾� ���
	}

	int findplayer_y(int client_num) //�ش�Ŭ���� ��ġ�� ã����
	{
		if (client_num == 0) {
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (map[i][j] >= 3 && map[i][j] <= 6)
						return j;
				}
			}
		}

		else if (client_num == 1) {
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (map[i][j] >= 7 && map[i][j] <= 10)
						return j;
				}
			}
		}

		else if (client_num == 2) {
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (map[i][j] >= 11 && map[i][j] <= 14)
						return j;
				}
			}
		}
		return 100; //�ƹ��͵� ��ã���� 100�� ��ȯ 
	}

	void hori_print_map(int client_num) { //��Ȱ������ ���
		int x;
		printf("\n");
		printf("\n");
			x = findplayer_x(client_num);
			for (int i = 0; i < 10; i++) {
				if (map[x][i] == 0) {
					printf("  ");
				}
				else if (map[x][i] == 1) { //��
					printf("��");
				}

				//�÷��̾�0 �����¿� 3456  
				else if (map[x][i] == 3) {
					printf("��");
				}
				else if (map[x][i] == 4) {
					printf("��");
				}
				else if (map[x][i] == 5) {
					printf("��");
				}
				else if (map[x][i] == 6) {
					printf("��");
				}
				//�÷��̾�1 789 10
				else if (map[x][i] == 7) {
					printf("��");
				}
				else if (map[x][i] == 8) {
					printf("��");
				}
				else if (map[x][i] == 9) {
					printf("��");
				}
				else if (map[x][i] == 10) {
					printf("��");
				}
				//�÷��̾�2 11 12 13 14
				else if (map[x][i] == 11) {
					printf("��");
				}
				else if (map[x][i] == 12) {
					printf("��");
				}
				else if (map[x][i] == 13) {
					printf("��");
				}
				else if (map[x][i] == 14) {
					printf("��");
				}
			}
			printf("\n");

			for (int i = 0; i < 10; i++) {
				printf("��");	
			}

			printf("\n");


	}

	void foggy_map(int client_num) { //�Ȱ������ ���
		int x, y;
		int x2, y2;
		int x3, y3;

		int x1_x2 = 0; //0�̸� �����ȿ� ����, 1�̸� �����ȿ� ����
		int x1_x3 = 0;

		int x2_x1 = 0;
		int x2_x3 = 0;

		int x3_x1 = 0;
		int x3_x2 = 0;

		x = findplayer_x(0);
		y = findplayer_y(0);
		
		x2 = findplayer_x(1);
		y2 = findplayer_y(1);

		x3 = findplayer_x(2);
		y3 = findplayer_y(2);

		if (client_num == 0) {
			if (map[x][y] == 3) { //��
				for (int a = 1; a <= 5; a++) {
					if ((x - a) == x2 && y == y2) {
						x1_x2 = 1;
					}

					if ((x - a) == x3 && y == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //�þ߿� �ƹ��� ������
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //�þ߿� client2�� ������
						map_x1_x2();
						map_none_hori_x1();
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //�þ߿� client3�� ������
						map_x1_x3();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // �Ѵ�������
						basic_print_map();
						map_none_hori_x1();
					}

				
			}

			if (map[x][y] == 4) { //��
				for (int a = 1; a <= 5; a++) {
					if ((x + a) == x2 && y == y2) {
						x1_x2 = 1;
					}

					if ((x + a) == x3 && y == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //�þ߿� �ƹ��� ������
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //�þ߿� client2�� ������
						map_x1_x2();
						map_none_hori_x1();
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //�þ߿� client3�� ������
						map_x1_x3();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // �Ѵ�������
						basic_print_map();
						map_none_hori_x1();
					}
				
			}

			if (map[x][y] == 5) { //��
				for (int a = 1; a <= 5; a++) {
					if ((x) == x2 && (y - a) == y2) {
						x1_x2 = 1;
					}

					if ((x) == x3 && (y - a) == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //�þ߿� �ƹ��� ������
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //�þ߿� client2�� ������
						map_x1_x2();
						map_x1_x2_hori(0);
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //�þ߿� client3�� ������
						map_x1_x3();
						map_x1_x3_hori(0);
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // �Ѵ�������
						basic_print_map();
						hori_print_map(0);

					}
				
			}

			if (map[x][y] == 6) { //��
				for (int a = 1; a <= 5; a++) {
					if ((x) == x2 && (y + a) == y2) {
						x1_x2 = 1;
					}

					if ((x) == x3 && (y + a) == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //�þ߿� �ƹ��� ������
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //�þ߿� client2�� ������
						map_x1_x2();
						map_x1_x2_hori(0);
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //�þ߿� client3�� ������
						map_x1_x3();
						map_x1_x3_hori(0);
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // �Ѵ�������
						basic_print_map();
						hori_print_map(0);
					}
				
			}
		}

		if (client_num == 1) {
			if (map[x2][y2] == 7) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2 - a) == x && y2 == y) {
						x2_x1 = 1;
					}

					if ((x2 - a) == x3 && y2 == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //�þ߿� �ƹ��� ������
					map_none_x2(); 
					map_none_hori_x2();
				}

				if (x2_x1 == 1 && x2_x3 == 0) {
					map_x1_x2();
					map_none_hori_x2();
				}

				if (x2_x1 == 0 && x2_x3 == 1) { 
					map_x2_x3();
					map_none_hori_x2();
				}

				if (x2_x1 == 1 && x2_x3 == 1) { // �Ѵ�������
					basic_print_map();
					map_none_hori_x2();
				}
			}

			if (map[x2][y2] == 8) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2 + a) == x && y2 == y) {
						x2_x1 = 1;
					}

					if ((x2 + a) == x3 && y2 == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //�þ߿� �ƹ��� ������
					map_none_x2();
					map_none_hori_x2();
				}

				if (x2_x1 == 1 && x2_x3 == 0) { 
					map_x1_x2();
					map_none_hori_x2();
				}

				if (x2_x1 == 0 && x2_x3 == 1) { 
					map_x2_x3();
					map_none_hori_x2();
				}

				if (x2_x1 == 1 && x2_x3 == 1) { // �Ѵ�������
					basic_print_map();
					map_none_hori_x2();
				}
			}

			if (map[x2][y2] == 9) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2) == x && (y2-a) == y) {
						x2_x1 = 1;
					}

					if (x2 == x3 && (y2-a) == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //�þ߿� �ƹ��� ������
					map_none_x2();
					map_none_hori_x2();
				}

				if (x2_x1 == 1 && x2_x3 == 0) { 
					map_x1_x2();
					map_x1_x2_hori(1);
				}

				if (x2_x1 == 0 && x2_x3 == 1) { 
					map_x2_x3();
					map_x2_x3_hori(1);
				}

				if (x2_x1 == 1 && x2_x3 == 1) { // �Ѵ�������
					basic_print_map();
					hori_print_map(1);
				}
			}

			if (map[x2][y2] == 10) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2) == x && (y2 + a) == y) {
						x2_x1 = 1;
					}

					if (x2 == x3 && (y2 + a) == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //�þ߿� �ƹ��� ������
					map_none_x2();
					map_none_hori_x2();
				}

				if (x2_x1 == 1 && x2_x3 == 0) { 
					map_x1_x2();
					map_x1_x2_hori(1);
				}

				if (x2_x1 == 0 && x2_x3 == 1) { 
					map_x2_x3();
					map_x2_x3_hori(1);
				}

				if (x2_x1 == 1 && x2_x3 == 1) { // �Ѵ�������
					basic_print_map();
					hori_print_map(1);
				}
			}
		}

		if (client_num == 2) {
			if (map[x3][y3] == 11) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if ((x3-a) == x && y3 == y) {
						x3_x1 = 1;
					}

					if ((x3-a) == x2 && y3 == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //�þ߿� �ƹ��� ������
					map_none_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 1 && x3_x2 == 0) { 
					map_x1_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 0 && x3_x2 == 1) { 
					map_x2_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 1 && x3_x2 == 1) { // �Ѵ�������
					basic_print_map();
					map_none_hori_x3();
				}
			}


			if (map[x3][y3] == 12) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if ((x3 + a) == x && y3 == y) {
						x3_x1 = 1;
					}

					if ((x3 + a) == x2 && y3 == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //�þ߿� �ƹ��� ������
					map_none_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 1 && x3_x2 == 0) { 
					map_x1_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 0 && x3_x2 == 1) { 
					map_x2_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 1 && x3_x2 == 1) { // �Ѵ�������
					basic_print_map();
					map_none_hori_x3();
				}
			}


			if (map[x3][y3] == 13) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if (x3 == x && (y3-a) == y) {
						x3_x1 = 1;
					}

					if (x3 == x2 && (y3-a) == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //�þ߿� �ƹ��� ������
					map_none_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 1 && x3_x2 == 0) { 
					map_x1_x3();
					map_x1_x3_hori(2);
				}

				if (x3_x1 == 0 && x3_x2 == 1) { 
					map_x2_x3();
					map_x2_x3_hori(2);
				}

				if (x3_x1 == 1 && x3_x2 == 1) { // �Ѵ�������
					basic_print_map();
					hori_print_map(2);
				}
			}

			if (map[x3][y3] == 14) // ��
			{
				for (int a = 1; a <= 5; a++) {
					if (x3 == x && (y3 + a) == y) {
						x3_x1 = 1;
					}

					if (x3 == x2 && (y3 + a) == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //�þ߿� �ƹ��� ������
					map_none_x3();
					map_none_hori_x3();
				}

				if (x3_x1 == 1 && x3_x2 == 0) { 
					map_x1_x3();
					map_x1_x3_hori(2);
				}

				if (x3_x1 == 0 && x3_x2 == 1) { 
					map_x2_x3();
					map_x2_x3_hori(2);
				}

				if (x3_x1 == 1 && x3_x2 == 1) { // �Ѵ�������
					basic_print_map();
					hori_print_map(2);
				}
			}
		}

	}
};


int main()
{
	Cli_Start();

	class printmap A;


	while (true)
	{
		if (map[10][10] == 0) { //��Ȱ����
			Sleep(500);
			system("cls");
			A.basic_print_map();
			A.hori_print_map(ClientNumber);
		}

		if (map[10][10] == 1) { //�Ȱ����
			Sleep(500);
			system("cls");
			A.foggy_map(ClientNumber);
		}
	}


	return 0;
}
