#pragma once
#pragma commment(lib, "ws2_32.lib")

#define PORT 8080

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <vector> //vector
#include <utility> //pair
#include <thread>

typedef std::pair<CLIENT, std::string> pii;

class CLIENT {
public:
	SOCKET client;
	SOCKADDR_IN addr = { 0 };
	int clientsize;
}; std::vector<pii> Client;