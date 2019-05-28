#include "../shared/SocketConnection.h"

#pragma comment(lib,"ws2_32.lib")

void communication() {
	SocketConnection toServer(AF_INET, SOCK_STREAM, 0, "127.0.0.1", 13337);
	toServer.socketConnect();

	std::string message;
	std::cin >> message;
	while (message != "x") {
		char message2[] = "Hello World";

		toServer.socketSend(message2, strlen(message2));
		std::cin >> message;
	}
}

int main() {
	WSADATA data; // this is to save our socket version
	WSAStartup(MAKEWORD(2, 2), &data);

	communication();

	WSACleanup();
	return 0;
}