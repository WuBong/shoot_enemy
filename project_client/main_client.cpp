// 프로젝트 설정 최신 C++ 초안의 기능
// ISO C17로 변경
//cout 보다 printf가 더 빠르니 맵출력 할때 printf 
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>

#define PORT	5500 /*서버와 클라이언트간에 데이터를 주고 받을 포트번호*/
#define PACKET_SIZE 1024
#define MAP_SIZE 5



#pragma comment (lib, "ws2_32.lib") //ws2_32.lib 라이브러리 링크
#pragma warning(disable:4996)

using namespace std;

WSADATA wsdata;
SOCKET clientSocket; /* 서버에 접속할 클라이언트 소켓 변수 선언*/
SOCKADDR_IN serverAddress; /* 서버소켓의 주소정보를 저장 하는 구조체 변수 선언 */



int map[11][11] = { 0, };
int ClientNumber=0; //서버에서 받을 지정클라 번호
char fromServer[PACKET_SIZE];
int foggy = 0; //0 to 비안개모드, 1 to 안개모드

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

}//atoi itoa 방식

void data_recv_map2() {		//수정 맵 정수값을 받고 프린트하는 부분을 만들어야함.
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
	/*WS_32.DLL 을 초기화 */
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		cout << "WS2_32.DLL 을 초기화 하는데 실패했습니다. " << endl;
		return ;
	}

	cout << "WS_32.DLL 을 초기화 하는데 성공 했습니다 " << endl;
	/*socket 함수를 이용해서 clientSocket을 생성 */
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	ZeroMemory(&serverAddress, sizeof(serverAddress));

	/* serverAddress 에 필요한 주소 정보를 저장 합니다.*/
	serverAddress.sin_family = AF_INET; /*소켓의 주소 체계를 설정 합니다.	TCP/IP 프로토콜의 경우 인터넷 기반이므로 AF_INET 을 대입합니다.*/
	serverAddress.sin_port = htons(PORT); /*서버 소켓의 포트를 설정 합니다.  */
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");/*서버의 IP를 설정 합니다.*/
	/*serverAddress에 저장된 서버의 정보를 이용해서 서버에 접속합니다.*/
	if ((connect(clientSocket, (SOCKADDR*)(&serverAddress), sizeof(serverAddress)) != 0))
	{
		cout << "서버에 접속하는데 실패 했습니다 " << endl;
		return ;
	}
	cout << "서버에 접속 했습니다 " << endl;

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
	void map_none_x1() { //안개모드에서 클라0번이 아무것도 안보일떄
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 3) {
				printf("▲");
				}
				else if (map[i][j] == 4) {
				printf("▼");
				}
				else if (map[i][j] == 5) {
				printf("◀");
				}
				else if (map[i][j] == 6) {
				printf("▶");
				}
				else {
					printf("□");
				}
			}
			printf("\n");
		}
	}

	void map_none_x2() { //안개모드에서 클라1번이 아무것도 안보일떄
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 2) { //공격 
					printf("o");
				}
				//플레이어1 789 10
				else if (map[i][j] == 7) {
					printf("▲");
				}
				else if (map[i][j] == 8) {
					printf("▼");
				}
				else if (map[i][j] == 9) {
					printf("◀");
				}
				else if (map[i][j] == 10) {
					printf("▶");
				}
				else {
					printf("□");
				}
			}
			printf("\n");
		}
	}

	void map_none_x3() { //안개모드에서 클라2번이 아무것도 안보일때
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 2) { //공격 
					printf("o");
				}

				//플레이어2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("▲");
				}
				else if (map[i][j] == 12) {
					printf("▼");
				}
				else if (map[i][j] == 13) {
					printf("◀");
				}
				else if (map[i][j] == 14) {
					printf("▶");
				}
				else {
					printf("□");
				}
			}
			printf("\n");
		}
	}

	void map_x1_x2() { //안개모드에서 클라0번과 1번이 보일때
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 2) { //공격 
					printf("o");
				}
				//플레이어0 상하좌우 3456  
				else if (map[i][j] == 3) {
					printf("▲");
				}
				else if (map[i][j] == 4) {
					printf("▼");
				}
				else if (map[i][j] == 5) {
					printf("◀");
				}
				else if (map[i][j] == 6) {
					printf("▶");
				}
				//플레이어1 789 10
				else if (map[i][j] == 7) {
					printf("▲");
				}
				else if (map[i][j] == 8) {
					printf("▼");
				}
				else if (map[i][j] == 9) {
					printf("◀");
				}
				else if (map[i][j] == 10) {
					printf("▶");
				}
				else {
					printf("□");
				}
			}
			printf("\n");
		}
	}

	void map_x1_x3() { //안개모드에서 클라0번과 클라2번이 보일때
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 2) { //공격 
					printf("o");
				}
				//플레이어0 상하좌우 3456  
				else if (map[i][j] == 3) {
					printf("▲");
				}
				else if (map[i][j] == 4) {
					printf("▼");
				}
				else if (map[i][j] == 5) {
					printf("◀");
				}
				else if (map[i][j] == 6) {
					printf("▶");
				}

				//플레이어2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("▲");
				}
				else if (map[i][j] == 12) {
					printf("▼");
				}
				else if (map[i][j] == 13) {
					printf("◀");
				}
				else if (map[i][j] == 14) {
					printf("▶");
				}
				else {
					printf("□");
				}
			}
			printf("\n");
		}
	}
	
	void map_x2_x3() //안개모드에서 클라1번과 2번이 보일때
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 2) { //공격 
					printf("o");
				}
	
				//플레이어1 789 10
				else if (map[i][j] == 7) {
					printf("▲");
				}
				else if (map[i][j] == 8) {
					printf("▼");
				}
				else if (map[i][j] == 9) {
					printf("◀");
				}
				else if (map[i][j] == 10) {
					printf("▶");
				}
				//플레이어2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("▲");
				}
				else if (map[i][j] == 12) {
					printf("▼");
				}
				else if (map[i][j] == 13) {
					printf("◀");
				}
				else if (map[i][j] == 14) {
					printf("▶");
				}
				else {
					printf("□");
				}
			}
			printf("\n");
		}
	}

	void map_x1_x2_hori(int client_num) { //안개모드에서 클라0번과 1번이 보일때 
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(client_num);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //벽
				printf("■");
			}

			//플레이어0 상하좌우 3456  
			else if (map[x][i] == 3) {
				printf("▲");
			}
			else if (map[x][i] == 4) {
				printf("▼");
			}
			else if (map[x][i] == 5) {
				printf("◀");
			}
			else if (map[x][i] == 6) {
				printf("▶");
			}
			//플레이어1 789 10
			else if (map[x][i] == 7) {
				printf("▲");
			}
			else if (map[x][i] == 8) {
				printf("▼");
			}
			else if (map[x][i] == 9) {
				printf("◀");
			}
			else if (map[x][i] == 10) {
				printf("▶");
			}
			else {
				printf("  ");
			}
		}
			printf("\n");

			for (int i = 0; i < 10; i++) {
				printf("□");
			}

	} 

	void map_x1_x3_hori(int client_num) { //안개모드에서 클라0번과 2번이 보일때
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(client_num);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //벽
				printf("■");
			}

			//플레이어0 상하좌우 3456  
			else if (map[x][i] == 3) {
				printf("▲");
			}
			else if (map[x][i] == 4) {
				printf("▼");
			}
			else if (map[x][i] == 5) {
				printf("◀");
			}
			else if (map[x][i] == 6) {
				printf("▶");
			}
			else if (map[x][i] == 11) {
				printf("▲");
			}
			else if (map[x][i] == 12) {
				printf("▼");
			}
			else if (map[x][i] == 13) {
				printf("◀");
			}
			else if (map[x][i] == 14) {
				printf("▶");
			}

			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("□");
		}

	}

	void map_x2_x3_hori(int client_num) { //안개모드에서 클라1번과 2번이 보일때
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(client_num);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //벽
				printf("■");
			}
			//플레이어1 789 10
			else if (map[x][i] == 7) {
				printf("▲");
			}
			else if (map[x][i] == 8) {
				printf("▼");
			}
			else if (map[x][i] == 9) {
				printf("◀");
			}
			else if (map[x][i] == 10) {
				printf("▶");
			}
			//플레이어2
			else if (map[x][i] == 11) {
				printf("▲");
			}
			else if (map[x][i] == 12) {
				printf("▼");
			}
			else if (map[x][i] == 13) {
				printf("◀");
			}
			else if (map[x][i] == 14) {
				printf("▶");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("□");
		}

	}

	void map_none_hori_x1() { //안개모드에서 클라1번이 아무것도 안보일때
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(0);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //벽
				printf("■");
			}

			//플레이어0 상하좌우 3456  
			else if (map[x][i] == 3) {
				printf("▲");
			}
			else if (map[x][i] == 4) {
				printf("▼");
			}
			else if (map[x][i] == 5) {
				printf("◀");
			}
			else if (map[x][i] == 6) {
				printf("▶");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("□");
		}

		printf("\n");

	}

	void map_none_hori_x2() { //안개모드에서 클라1번이 아무것도 안보일때
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(1);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //벽
				printf("■");
			}

			//플레이어0 상하좌우 3456  
			else if (map[x][i] == 7) {
				printf("▲");
			}
			else if (map[x][i] == 8) {
				printf("▼");
			}
			else if (map[x][i] == 9) {
				printf("◀");
			}
			else if (map[x][i] == 10) {
				printf("▶");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("□");
		}

		printf("\n");

	}

	void map_none_hori_x3() { //안개모드에서 클라2번이 아무것도 안보일때
		int x;
		printf("\n");
		printf("\n");
		x = findplayer_x(2);
		for (int i = 0; i < 10; i++) {
			if (map[x][i] == 0) {
				printf("  ");
			}
			else if (map[x][i] == 1) { //벽
				printf("■");
			}


			else if (map[x][i] == 11) {
				printf("▲");
			}
			else if (map[x][i] == 12) {
				printf("▼");
			}
			else if (map[x][i] == 13) {
				printf("◀");
			}
			else if (map[x][i] == 14) {
				printf("▶");
			}
			else {
				printf("  ");
			}
		}
		printf("\n");

		for (int i = 0; i < 10; i++) {
			printf("□");
		}

		printf("\n");

	}

	void basic_print_map() //cout 보다 printf가 더 빠름.
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (map[i][j] == 0) {
					printf("□");
				}
				else if (map[i][j] == 1) { //벽
					printf("■");
				}
				else if (map[i][j] == 2) { //공격 
					printf("o");
				}
				//플레이어0 상하좌우 3456  
				else if (map[i][j] == 3) {
					printf("▲");
				}
				else if (map[i][j] == 4) {
					printf("▼");
				}
				else if (map[i][j] == 5) {
					printf("◀");
				}
				else if (map[i][j] == 6) {
					printf("▶");
				}
				//플레이어1 789 10
				else if (map[i][j] == 7) {
					printf("▲");
				}
				else if (map[i][j] == 8) {
					printf("▼");
				}
				else if (map[i][j] == 9) {
					printf("◀");
				}
				else if (map[i][j] == 10) {
					printf("▶");
				}
				//플레이어2 11 12 13 14
				else if (map[i][j] == 11) {
					printf("▲");
				}
				else if (map[i][j] == 12) {
					printf("▼");
				}
				else if (map[i][j] == 13) {
					printf("◀");
				}
				else if (map[i][j] == 14) {
					printf("▶");
				}
			}
			printf("\n");
		}
	}

	int findplayer_x(int client_num) //해당클라의 위치를 찾아줌
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
		return 100; //아무것도 못찾으면 100을 반환 즉 플레이어 사망
	}

	int findplayer_y(int client_num) //해당클라의 위치를 찾아줌
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
		return 100; //아무것도 못찾으면 100을 반환 
	}

	void hori_print_map(int client_num) { //비안개모드의 출력
		int x;
		printf("\n");
		printf("\n");
			x = findplayer_x(client_num);
			for (int i = 0; i < 10; i++) {
				if (map[x][i] == 0) {
					printf("  ");
				}
				else if (map[x][i] == 1) { //벽
					printf("■");
				}

				//플레이어0 상하좌우 3456  
				else if (map[x][i] == 3) {
					printf("▲");
				}
				else if (map[x][i] == 4) {
					printf("▼");
				}
				else if (map[x][i] == 5) {
					printf("◀");
				}
				else if (map[x][i] == 6) {
					printf("▶");
				}
				//플레이어1 789 10
				else if (map[x][i] == 7) {
					printf("▲");
				}
				else if (map[x][i] == 8) {
					printf("▼");
				}
				else if (map[x][i] == 9) {
					printf("◀");
				}
				else if (map[x][i] == 10) {
					printf("▶");
				}
				//플레이어2 11 12 13 14
				else if (map[x][i] == 11) {
					printf("▲");
				}
				else if (map[x][i] == 12) {
					printf("▼");
				}
				else if (map[x][i] == 13) {
					printf("◀");
				}
				else if (map[x][i] == 14) {
					printf("▶");
				}
			}
			printf("\n");

			for (int i = 0; i < 10; i++) {
				printf("□");	
			}

			printf("\n");


	}

	void foggy_map(int client_num) { //안개모드의 출력
		int x, y;
		int x2, y2;
		int x3, y3;

		int x1_x2 = 0; //0이면 범위안에 없음, 1이면 범위안에 있음
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
			if (map[x][y] == 3) { //상
				for (int a = 1; a <= 5; a++) {
					if ((x - a) == x2 && y == y2) {
						x1_x2 = 1;
					}

					if ((x - a) == x3 && y == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //시야에 아무도 없을때
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //시야에 client2가 있을때
						map_x1_x2();
						map_none_hori_x1();
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //시야에 client3가 있을때
						map_x1_x3();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // 둘다있을때
						basic_print_map();
						map_none_hori_x1();
					}

				
			}

			if (map[x][y] == 4) { //하
				for (int a = 1; a <= 5; a++) {
					if ((x + a) == x2 && y == y2) {
						x1_x2 = 1;
					}

					if ((x + a) == x3 && y == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //시야에 아무도 없을때
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //시야에 client2가 있을때
						map_x1_x2();
						map_none_hori_x1();
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //시야에 client3가 있을때
						map_x1_x3();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // 둘다있을때
						basic_print_map();
						map_none_hori_x1();
					}
				
			}

			if (map[x][y] == 5) { //좌
				for (int a = 1; a <= 5; a++) {
					if ((x) == x2 && (y - a) == y2) {
						x1_x2 = 1;
					}

					if ((x) == x3 && (y - a) == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //시야에 아무도 없을때
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //시야에 client2가 있을때
						map_x1_x2();
						map_x1_x2_hori(0);
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //시야에 client3가 있을때
						map_x1_x3();
						map_x1_x3_hori(0);
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // 둘다있을때
						basic_print_map();
						hori_print_map(0);

					}
				
			}

			if (map[x][y] == 6) { //우
				for (int a = 1; a <= 5; a++) {
					if ((x) == x2 && (y + a) == y2) {
						x1_x2 = 1;
					}

					if ((x) == x3 && (y + a) == y3) {
						x1_x3 = 1;
					}
				}

					if (x1_x2 == 0 && x1_x3 == 0) { //시야에 아무도 없을때
						map_none_x1();
						map_none_hori_x1();
					}

					if (x1_x2 == 1 && x1_x3 == 0) { //시야에 client2가 있을때
						map_x1_x2();
						map_x1_x2_hori(0);
					}

					if (x1_x2 == 0 && x1_x3 == 1) { //시야에 client3가 있을때
						map_x1_x3();
						map_x1_x3_hori(0);
					}

					if (x1_x2 == 1 && x1_x3 == 1) { // 둘다있을때
						basic_print_map();
						hori_print_map(0);
					}
				
			}
		}

		if (client_num == 1) {
			if (map[x2][y2] == 7) // 상
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2 - a) == x && y2 == y) {
						x2_x1 = 1;
					}

					if ((x2 - a) == x3 && y2 == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //시야에 아무도 없을때
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

				if (x2_x1 == 1 && x2_x3 == 1) { // 둘다있을때
					basic_print_map();
					map_none_hori_x2();
				}
			}

			if (map[x2][y2] == 8) // 하
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2 + a) == x && y2 == y) {
						x2_x1 = 1;
					}

					if ((x2 + a) == x3 && y2 == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //시야에 아무도 없을때
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

				if (x2_x1 == 1 && x2_x3 == 1) { // 둘다있을때
					basic_print_map();
					map_none_hori_x2();
				}
			}

			if (map[x2][y2] == 9) // 좌
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2) == x && (y2-a) == y) {
						x2_x1 = 1;
					}

					if (x2 == x3 && (y2-a) == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //시야에 아무도 없을때
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

				if (x2_x1 == 1 && x2_x3 == 1) { // 둘다있을때
					basic_print_map();
					hori_print_map(1);
				}
			}

			if (map[x2][y2] == 10) // 우
			{
				for (int a = 1; a <= 5; a++) {
					if ((x2) == x && (y2 + a) == y) {
						x2_x1 = 1;
					}

					if (x2 == x3 && (y2 + a) == y3) {
						x2_x3 = 1;
					}
				}

				if (x2_x1 == 0 && x2_x3 == 0) { //시야에 아무도 없을때
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

				if (x2_x1 == 1 && x2_x3 == 1) { // 둘다있을때
					basic_print_map();
					hori_print_map(1);
				}
			}
		}

		if (client_num == 2) {
			if (map[x3][y3] == 11) // 상
			{
				for (int a = 1; a <= 5; a++) {
					if ((x3-a) == x && y3 == y) {
						x3_x1 = 1;
					}

					if ((x3-a) == x2 && y3 == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //시야에 아무도 없을때
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

				if (x3_x1 == 1 && x3_x2 == 1) { // 둘다있을때
					basic_print_map();
					map_none_hori_x3();
				}
			}


			if (map[x3][y3] == 12) // 하
			{
				for (int a = 1; a <= 5; a++) {
					if ((x3 + a) == x && y3 == y) {
						x3_x1 = 1;
					}

					if ((x3 + a) == x2 && y3 == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //시야에 아무도 없을때
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

				if (x3_x1 == 1 && x3_x2 == 1) { // 둘다있을때
					basic_print_map();
					map_none_hori_x3();
				}
			}


			if (map[x3][y3] == 13) // 좌
			{
				for (int a = 1; a <= 5; a++) {
					if (x3 == x && (y3-a) == y) {
						x3_x1 = 1;
					}

					if (x3 == x2 && (y3-a) == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //시야에 아무도 없을때
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

				if (x3_x1 == 1 && x3_x2 == 1) { // 둘다있을때
					basic_print_map();
					hori_print_map(2);
				}
			}

			if (map[x3][y3] == 14) // 우
			{
				for (int a = 1; a <= 5; a++) {
					if (x3 == x && (y3 + a) == y) {
						x3_x1 = 1;
					}

					if (x3 == x2 && (y3 + a) == y2) {
						x3_x2 = 1;
					}
				}

				if (x3_x1 == 0 && x3_x2 == 0) { //시야에 아무도 없을때
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

				if (x3_x1 == 1 && x3_x2 == 1) { // 둘다있을때
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
		if (map[10][10] == 0) { //비안개모드
			Sleep(500);
			system("cls");
			A.basic_print_map();
			A.hori_print_map(ClientNumber);
		}

		if (map[10][10] == 1) { //안개모드
			Sleep(500);
			system("cls");
			A.foggy_map(ClientNumber);
		}
	}


	return 0;
}
