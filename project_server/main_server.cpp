// 프로젝트 설정 최신 C++ 초안의 기능
// ISO C17로 변경
// if, else if 구분 확실히.
#include <winsock2.h> /*소켓 통신에 필요한 함수 들이 선언되어있는 헤더파일 */
#include <iostream>
#include <thread>
#include <conio.h>
//#include <stdlib.h>
#include <windows.h> //zeromemory, cls, sleep 
#include <process.h>  //동기화

#define PORT	5500       /*서버와 클라이언트간에 데이터를 주고 받을 포트번호*/
#define MAX 3 //클라개수
#define PACKET_SIZE 1024
#define MAP_SIZE 5


//입력받을 키 정리.
#define UP	    72
#define DOWN	80
#define RIGHT	77
#define LEFT	75
#define SPACE   32
#define S		115
#define F 102 

#pragma comment (lib, "ws2_32.lib") //ws2_32.lib 라이브러리 링크
#pragma warning(disable:4996)

using namespace std;

/* WSAStartup 함수를 이용해서 윈도우 소켓이 정의되어 있는 WS2_32.DLL 파일을 초기화.  초기화가 성공하면 0을 리턴.
	  형식:int WSAStartup (WORD 윈도우 소켓의 버젼,  LPWSADATA 윈도우 소켓의 구현에 관한 정보를 저장할 포인터 );
*/

WSADATA wsdata;	/*serverAddress 는 서버소켓의 주소정보를 저장 하는 구조체.
				serverAddress 에 필요한 주소 정보를 저장 */
SOCKADDR_IN serverAddress; 	/* 서버 소켓일을 전담하는 SOCKET 변수 선언 */
SOCKET servsoc; 	/* 클라이언트와 데이터를 주고 받는 일을 전담하는 SOCKET 변수 선언 */
SOCKET clisoc[MAX]; //클라 주고받는 소켓


int member = 0;
int map[11][11] = { 0, }; // map[10][10] 에는 안개모드의 값,  0; //0 to 비안개모드, 1 to 안개모드 보여주는 것은 [9][9]까지만
int key[3] = { 0, };
int clientnumber = 0; //클라에게 보내줄 번호

HANDLE hMutex;

void data_send_map(int c_n) {	//atoi itoa 방식
	for (;;)
	{
		char send_integer[MAP_SIZE];
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				ZeroMemory(send_integer, MAP_SIZE);
				_itoa(map[i][j], send_integer, 10);
				send(clisoc[c_n], send_integer, MAP_SIZE, 0);
			}
		}
		Sleep(100); //최적화 
	}
}

void data_send_map3() {		//너무 무분별하게 thread에 반복문 쓰여서 같은 값을 보내는
							//거여서 통합함. data_send_map2의 진화버전 바이트 정렬식
	for (;;)
	{
		int temp = 0;
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				temp = htonl(map[i][j]);
				send(clisoc[0], (char*)&temp, MAP_SIZE, 0); 
				send(clisoc[1], (char*)&temp, MAP_SIZE, 0); 
				send(clisoc[2], (char*)&temp, MAP_SIZE, 0); 
			}
		}
		Sleep(100); //최적화 
	}
}

void data_recv_key(int a) {		
	int temp;
	for (;;)
	{
		recv(clisoc[a], (char*)&temp, sizeof(key), 0);
		key[a] = ntohl(temp);
		Sleep(100); //최적화
	}
}



void clientAcc() {
	for (int i = 0; i < MAX; i++){
		clisoc[i] = accept(servsoc, NULL, NULL);
		if (clisoc[i] == INVALID_SOCKET) {
			closesocket(clisoc[i]);
			closesocket(servsoc);
			WSACleanup();
			return ;
		}

		int ad = 48 + i;
		cout << "Client #" << i << " joined!" << endl;
		send(clisoc[i], (char*)&ad, sizeof(int) + 1, 0);

		ad = htonl(i);
		send(clisoc[i], (char*)&ad, sizeof(int), 0);

		member++;
	}
}

int Ser_Start()		//thread가 클래스에서 사용하기 번거로움..
{
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		/*WS_32.DLL 을 초기화 하는데 실패 했을 경우 에러메시지를 출력하고
			프로그램을 종료 */
		cout << "WS2_32.DLL 을 초기화 하는데 실패했습니다. " << endl;
		return 0;
	}
	cout << "WS_32.DLL 을 초기화 하는데 성공 했습니다 " << endl;

	servsoc = socket(AF_INET, SOCK_STREAM, 0);
	/*ZeroMemory 함수는 해당 메모리를 0으로 초기화 시켜주는 함수.
 serverAddress 배열을 0으로 초기화. */
	ZeroMemory(&serverAddress, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET; /*소켓의 주소 체계를 설정.
	TCP/IP 프로토콜의 경우 인터넷 기반이므로 AF_INET 을 대입.*/

	serverAddress.sin_port = htons(PORT); ///*서버 소켓의 포트를 설정*/ 
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);/*서버의 IP를 설정 */

		 /*bind 함수는 serverAddress 에 대입한 서버소켓의 주소정보를 이용해서
	  servsoc에  IP 와 포트를 부여.  servsoc에 IP 와 port 를 부여 하는데 실패 하면 	SOCKET_ERROR 를 리턴*/
	if (bind(servsoc, (SOCKADDR*)&serverAddress,
		sizeof(serverAddress)) == SOCKET_ERROR)
	{
		cout << "serverSocket에 IP 와 PORT 를 부여 하는데 실패했습니다! " << endl;
		return 0;
	}
	cout << "serverSocket 에 IP 와 Port 를 부여 하는데 성공 했습니다 " << endl;

	/*listen 함수는 클라이언트가 servsoc에 부여한 IP 와 PORT 로 접속 했는지를 감시 하는 함수. 클라이언트가 접속할 때까지 클라이언트 대기
	  형식:  listen(SOCKET 클라이언트의 접속을 기다리는 소켓객체,
	   int 동시에 접속가능한 클라이언트의 수 SOMAXCONN 의 경우 동시 접속 가능한 	클라이언트의 수를 무제한으로 설정) */
	cout << "클라이언트의 접속을 감시 합니다 " << endl;
	listen(servsoc, SOMAXCONN);

	thread(clientAcc).detach();		 //다중클라 받기

	while (true) {
		if (member == MAX) {
			cout << MAX << "명의 플레이어 모두 접속!" << endl;
			break;
		}
	}

	map[1][3] = 1; map[1][4] = 1; map[4][7] = 1; map[4][8] = 1;
	map[2][3] = 1; map[2][4] = 1; map[5][7] = 1; map[5][8] = 1;
	map[6][1] = 1; map[6][2] = 1; map[7][5] = 1; map[7][6] = 1;
	map[7][1] = 1; map[7][2] = 1; map[8][5] = 1; map[8][6] = 1;

	
	map[0][0] = 3;  //player0의 위치
	key[0] = RIGHT;
	map[9][0] = 7;  //player1의 위치
	key[1] = UP;
	map[0][9] = 11; //player2의 위치
	key[2] = LEFT;

	thread(data_send_map3).detach();  //실시간으로 맵값 보내는것

	thread(data_recv_key, 0).detach();
	thread(data_recv_key, 1).detach();
	thread(data_recv_key, 2).detach();	//실시간으로 키보드 값 받는것

	

	return 0;
}

void foggy_key_() { //도중에도 안개모드 비안개모드 
					//선택할 수 있게끔 쓰레드로 돌림.
	for (;;) {

		cin >> map[10][10];

		Sleep(1000);
	}

	return;
}

class Print { //추상클래스
	virtual void print_map() = 0;
};

class depoless : public Print { //상속
private:
	int time;
	int cli_stop[3] = {0, };

public:
	depoless() {  //생성자
		time = 1000;

	}

	~depoless() { //소멸자
	}

	void print_map() override { //상속과 추상클래스
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
		printf("\n");
	}


	void timeaccept() {
		cout << "딜레이를 입력해주세요 ex)100 = 0.1초 1000 = 1초" << endl;
		cin >> time;
	}

	int findplayer_x(int client_num)
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

	int findplayer_y(int client_num)
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

	void move_player2_0() { //특정 시간대에 스태이터스 값 보고 움직이기 쓰레드 분리
		int x, y;
		for (;;) {
			//플레이어0 의 이동
			x = findplayer_x(0);
			y = findplayer_y(0);
			//첫번째 if 벽과 맵이탈 방지
			//두번째 if 플레이어 충돌방지
			if (x == 100 || y == 100) { //플레이어 사망문
				return;
			}

			if (cli_stop[0] == 0) { //스탑했는가 안했는가
				if (key[0] == RIGHT) {
					if (y == 9 || map[x][y + 1] == 1) //벽과 맵이탈 방지
						continue;

					if (map[x][y + 1] >= 7) //플레이어 끼리의 충돌방지
						continue;

					map[x][y] = 0;
					map[x][y + 1] = 6;
				}

				else if (key[0] == LEFT) {
					if (y == 0 || map[x][y - 1] == 1)
						continue;

					if (map[x][y - 1] >= 7)
						continue;

					map[x][y] = 0;
					map[x][y - 1] = 5;

				}

				else if (key[0] == UP) {
					if (x == 0 || map[x - 1][y] == 1)
						continue;
					if (map[x - 1][y] >= 7)
						continue;
					map[x][y] = 0;
					map[x - 1][y] = 3;
				}

				else if (key[0] == DOWN) {
					if (x == 9 || map[x + 1][y] == 1)
						continue;
					if (map[x + 1][y] >= 7)
						continue;
					map[x][y] = 0;
					map[x + 1][y] = 4;
				}

			}

			Sleep(time);
		}
	} 

	void move_player2_1() {
		//플레이어 1의 이동
		int x, y;
		for (;;) {
			x = findplayer_x(1);
			y = findplayer_y(1);
			if (x == 100 || y == 100) { //플레이어 사망문
				return;
			}
			if (cli_stop[1] == 0) {
				if (key[1] == RIGHT) {
					if (y == 9 || map[x][y + 1] == 1)//벽과 맵이탈 방지
						continue;

					if (map[x][y + 1] <= 6 && map[x][y + 1] >= 2)
						continue;

					if (map[x][y + 1] >= 11 && map[x][y + 1] <= 14)
						continue;

					map[x][y] = 0;
					map[x][y + 1] = 10;
				}

				if (key[1] == LEFT) {
					map[x][y] = 9;
					if (y == 0 || map[x][y - 1] == 1)
						continue;

					if (map[x][y - 1] <= 6 && map[x][y - 1] >= 2)
						continue;

					if (map[x][y - 1] >= 11 && map[x][y - 1] <= 14)
						continue;

					map[x][y] = 0;
					map[x][y - 1] = 9;


				}

				if (key[1] == UP) {
					if (x == 0 || map[x - 1][y] == 1)
						continue;

					if (map[x - 1][y] <= 6 && map[x - 1][y] >= 2)
						continue;

					if (map[x - 1][y] >= 11 && map[x - 1][y] <= 14)
						continue;

					map[x][y] = 0;
					map[x - 1][y] = 7;

				}

				if (key[1] == DOWN) {
					map[x][y] = 8;
					if (x == 9 || map[x + 1][y] == 1)
						continue;

					if (map[x + 1][y] <= 6 && map[x + 1][y] >= 2)
						continue;

					if (map[x + 1][y] >= 11 && map[x + 1][y] <= 14)
						continue;

					map[x][y] = 0;
					map[x + 1][y] = 8;
				}
			}
			Sleep(time);
		}
	}

	void move_player2_2() {
		//플레이어2의 이동
		int x, y;

		for (;;) {
			x = findplayer_x(2);
			y = findplayer_y(2);
			if (x == 100 || y == 100) { //플레이어 사망문
				return;
			}

			if (cli_stop[2] == 0) {
				if (key[2] == RIGHT) {
					if (y == 9 || map[x][y + 1] == 1) //벽과 맵이탈 방지
						continue;

					if (map[x][y + 1] >= 3 && map[x][y + 1] <= 6)
						continue;

					if (map[x][y + 1] >= 7 && map[x][y + 1] <= 10)
						continue;

					map[x][y] = 0;
					map[x][y + 1] = 14;
				}

				if (key[2] == LEFT) {
					map[x][y] = 13;
					if (y == 0 || map[x][y - 1] == 1)
						continue;

					if (map[x][y - 1] >= 3 && map[x][y - 1] <= 6)
						continue;

					if (map[x][y - 1] >= 7 && map[x][y - 1] <= 10)
						continue;

					map[x][y] = 0;
					map[x][y - 1] = 13;
				}

				if (key[2] == UP) {
					map[x][y] = 11;
					if (x == 0 || map[x - 1][y] == 1)
						continue;

					if (map[x - 1][y] >= 3 && map[x - 1][y] <= 6)
						continue;

					if (map[x - 1][y] >= 7 && map[x - 1][y] <= 10)
						continue;

					map[x][y] = 0;
					map[x - 1][y] = 11;

				}

				if (key[2] == DOWN) {
					map[x][y] = 12;
					if (x == 9 || map[x + 1][y] == 1)
						continue;

					if (map[x + 1][y] >= 3 && map[x + 1][y] <= 6)
						continue;

					if (map[x + 1][y] >= 7 && map[x + 1][y] <= 10)
						continue;

					map[x][y] = 0;
					map[x + 1][y] = 12;
				}
			}

			Sleep(time);
		}
	}

	void player_status2_0()
	{
		for (;;) {
			int x;
			int y;
			x = findplayer_x(0);
			y = findplayer_y(0);

			if (x == 100 || y == 100) { //플레이어 사망문
				return;
			}

			//우, 좌, 위, 아래
			if (key[0] == RIGHT) {
				map[x][y] = 6;
			}

			if (key[0] == LEFT) {
				map[x][y] = 5;

			}

			if (key[0] == UP) {
				map[x][y] = 3;

			}

			if (key[0] == DOWN) {
				map[x][y] = 4;
			}

			if (key[0] == S) {

				if (map[x][y] == 6) { //스탑을 풀어도 보는 방향으로 가기
					key[0] = RIGHT;
				}
				if (map[x][y] == 5) {
					key[0] = LEFT;
				}
				if (map[x][y] == 3) {
					key[0] = UP;
				}
				if (map[x][y] == 4) {
					key[0] = DOWN;
				}

				if (cli_stop[0] == 0)   //move -> stop
				{
					cli_stop[0] = 1;
				}
				else if (cli_stop[0] == 1) //stop -> move
				{
					cli_stop[0] = 0;
				}
			}
			//공격문
			if (key[0] == SPACE) {
				if (map[x][y] == 6) {
					key[0] = RIGHT;

					if (map[x][y + 1] == 1)
						continue;

					if (map[x][y + 2] == 1)
					{
						map[x][y + 1] = 0;
						continue;
					}

					if (map[x][y + 3] == 1)
					{
						map[x][y + 1] = 0;
						map[x][y + 2] = 0;
						continue;
					}


					map[x][y + 1] = 0;
					map[x][y + 2] = 0;
					map[x][y + 3] = 0;
				}
				if (map[x][y] == 5) {
					key[0] = LEFT;

					if (map[x][y - 1] == 1)
						continue;

					if (map[x][y - 2] == 1) {
						map[x][y - 1] = 0;
						continue;
					}

					if (map[x][y - 3] == 1) {
						map[x][y - 1] = 0;
						map[x][y - 2] = 0;
						continue;
					}

					map[x][y - 1] = 0;
					map[x][y - 2] = 0;
					map[x][y - 3] = 0;
				}
				if (map[x][y] == 3) {
					key[0] = UP;

					if (map[x-1][y] == 1)
						continue;

					if (map[x - 2][y] == 1) {
						map[x - 1][y] = 0;
						continue;
					}

					if (map[x - 3][y] == 1) {
						map[x - 1][y] = 0;
						map[x - 2][y] = 0;
						continue;
					}

					map[x - 1][y] = 0;
					map[x - 2][y] = 0;
					map[x - 3][y] = 0;
				}
				if (map[x][y] == 4) {
					key[0] = DOWN;

					if (map[x + 1][y] == 1)
						continue;

					if (map[x + 2][y] == 1)
					{
						map[x + 1][y] = 0;
						continue;
					}

					if (map[x + 3][y] == 1) {
						map[x + 1][y] = 0;
						map[x + 2][y] = 0;
						continue;
					}
					
					map[x + 1][y] = 0;
					map[x + 2][y] = 0;
					map[x + 3][y] = 0;
				}
			}

			Sleep(time/2);
		}
	}//방향 조절 함수

	void player_status2_1() {
		for (;;) {
			int x, y;
			x = findplayer_x(1);
			y = findplayer_y(1);

			if (x == 100 || y == 100) { //플레이어 사망문
				return;
			}
			//플레이어1의 상태

			if (key[1] == RIGHT) {
				map[x][y] = 10;
			}

			if (key[1] == LEFT) {
				map[x][y] = 9;

			}

			if (key[1] == UP) {
				map[x][y] = 7;

			}

			if (key[1] == DOWN) {
				map[x][y] = 8;
			}

			if (key[1] == S) {
				if (map[x][y] == 10) { //스탑을 풀어도 보는 방향으로 가기
					key[1] = RIGHT;
				}
				if (map[x][y] == 9) {
					key[1] = LEFT;
				}
				if (map[x][y] == 7) {
					key[1] = UP;
				}
				if (map[x][y] == 8) {
					key[1] = DOWN;
				}

				if (cli_stop[1] == 0)   //move -> stop
				{
					cli_stop[1] = 1;
				}
				else if (cli_stop[1] == 1) //stop -> move
				{
					cli_stop[1] = 0;
				}
			}

			if (key[1] == SPACE) {
				if (map[x][y] == 10) {
					key[1] = RIGHT;

					if (map[x][y + 1] == 1)
						continue;

					if (map[x][y + 2] == 1)
					{
						map[x][y + 1] = 0;
						continue;
					}

					if (map[x][y + 3] == 1)
					{
						map[x][y + 1] = 0;
						map[x][y + 2] = 0;
						continue;
					}
					map[x][y + 1] = 0;
					map[x][y + 2] = 0;
					map[x][y + 3] = 0;
				}
				if (map[x][y] == 9) {
					key[1] = LEFT;

					if (map[x][y - 1] == 1)
						continue;

					if (map[x][y - 2] == 1) {
						map[x][y - 1] = 0;
						continue;
					}

					if (map[x][y - 3] == 1) {
						map[x][y - 1] = 0;
						map[x][y - 2] = 0;
						continue;
					}

					map[x][y - 1] = 0;
					map[x][y - 2] = 0;
					map[x][y - 3] = 0;
				}
				if (map[x][y] == 7) {
					key[1] = UP;

					if (map[x - 1][y] == 1)
						continue;

					if (map[x - 2][y] == 1) {
						map[x - 1][y] = 0;
						continue;
					}

					if (map[x - 3][y] == 1) {
						map[x - 1][y] = 0;
						map[x - 2][y] = 0;
						continue;
					}


					map[x - 1][y] = 0;
					map[x - 2][y] = 0;
					map[x - 3][y] = 0;
				}
				if (map[x][y] == 8) {
					key[1] = DOWN;

					if (map[x + 1][y] == 1)
						continue;

					if (map[x + 2][y] == 1)
					{
						map[x + 1][y] = 0;
						continue;
					}

					if (map[x + 3][y] == 1) {
						map[x + 1][y] = 0;
						map[x + 2][y] = 0;
						continue;
					}

					map[x + 1][y] = 0;
					map[x + 2][y] = 0;
					map[x + 3][y] = 0;
				}
			}
			Sleep(time/2);
		}

	}

	void player_status2_2() {
		int x, y;
		for (;;)
		{
			x = findplayer_x(2);
			y = findplayer_y(2);

			if (x == 100 || y == 100) { //플레이어 사망문
				return;
			}
			//플레이어2의 상태

			if (key[2] == RIGHT) {
				map[x][y] = 14;
			}

			if (key[2] == LEFT) {
				map[x][y] = 13;

			}

			if (key[2] == UP) {
				map[x][y] = 11;

			}

			if (key[2] == DOWN) {
				map[x][y] = 12;
			}

			if (key[2] == S) {
				if (map[x][y] == 14) { //스탑을 풀어도 보는 방향으로 가기
					key[2] = RIGHT;
				}
				if (map[x][y] == 13) {
					key[2] = LEFT;
				}
				if (map[x][y] == 11) {
					key[2] = UP;
				}
				if (map[x][y] == 12) {
					key[2] = DOWN;
				}

				if (cli_stop[2] == 0)   //move -> stop
				{
					cli_stop[2] = 1;
				}
				else if (cli_stop[2] == 1) //stop -> move
				{
					cli_stop[2] = 0;
				}
			}

			if (key[2] == SPACE) {
				if (map[x][y] == 14) {
					key[2] = RIGHT;

					if (map[x][y + 1] == 1)
						continue;

					if (map[x][y + 2] == 1)
					{
						map[x][y + 1] = 0;
						continue;
					}

					if (map[x][y + 3] == 1)
					{
						map[x][y + 1] = 0;
						map[x][y + 2] = 0;
						continue;
					}

					map[x][y + 1] = 0;
					map[x][y + 2] = 0;
					map[x][y + 3] = 0;
				}
				if (map[x][y] == 13) {
					key[2] = LEFT;


					if (map[x][y - 1] == 1)
						continue;

					if (map[x][y - 2] == 1) {
						map[x][y - 1] = 0;
						continue;
					}

					if (map[x][y - 3] == 1) {
						map[x][y - 1] = 0;
						map[x][y - 2] = 0;
						continue;
					}


					map[x][y - 1] = 0;
					map[x][y - 2] = 0;
					map[x][y - 3] = 0;
				}
				if (map[x][y] == 11) {
					key[2] = UP;

					if (map[x - 1][y] == 1)
						continue;

					if (map[x - 2][y] == 1) {
						map[x - 1][y] = 0;
						continue;
					}

					if (map[x - 3][y] == 1) {
						map[x - 1][y] = 0;
						map[x - 2][y] = 0;
						continue;
					}



					map[x - 1][y] = 0;
					map[x - 2][y] = 0;
					map[x - 3][y] = 0;
				}
				if (map[x][y] == 12) {
					key[2] = DOWN;

					if (map[x + 1][y] == 1)
						continue;

					if (map[x + 2][y] == 1)
					{
						map[x + 1][y] = 0;
						continue;
					}

					if (map[x + 3][y] == 1) {
						map[x + 1][y] = 0;
						map[x + 2][y] = 0;
						continue;
					}

					map[x + 1][y] = 0;
					map[x + 2][y] = 0;
					map[x + 3][y] = 0;
				}
			}
			Sleep(time/2);
		}
	}



	void Start_Game() {

		timeaccept();

		thread s0([&]() {depoless::player_status2_0(); }); //방향 조절 함수
		s0.detach();
		thread s1([&]() {depoless::player_status2_1(); }); //방향 조절 함수
		s1.detach();
		thread s2([&]() {depoless::player_status2_2(); }); //방향 조절 함수
		s2.detach();

		thread m0([&]() {depoless::move_player2_0(); }); //이동함수
		m0.detach();
		thread m1([&]() {depoless::move_player2_1(); }); //이동함수
		m1.detach();
		thread m2([&]() {depoless::move_player2_2(); }); //이동함수
		m2.detach();

		thread(foggy_key_).detach(); //언제든지 모드선택

		while (1)
		{
			Sleep(time);
			system("cls");
			print_map();
			
		}
	}
	

};

int main()
{
	Ser_Start(); //소켓
	
	class depoless game;

	game.Start_Game(); //게임


	while (1)
	{
		Sleep(1000); //혹시모를 최적화
	}

	return 0;
}