#include "../shared/SocketConnection.h"

#pragma comment(lib,"ws2_32.lib")

void encrypt(char *input, char *output) {
	char key[] = "a";
	for (int i = 0; i < strlen(input); ++i) {
		output[i] = input[i] ^ key[i % (sizeof(key)/sizeof(char))];
	}
}

void decrypt(char *input, char *output) {
	encrypt(input, output);
}

void communication() {
	SocketConnection toServer(AF_INET, SOCK_STREAM, 0, "127.0.0.1", 13337);
	toServer.socketConnect();

	std::string message;
	std::cin >> message;
	while (message != "x") {

		char message2[] = "Hello World";
		char *msg_encrypt = new char[strlen(message2)];
		encrypt(message2, msg_encrypt);
		toServer.socketSend(msg_encrypt, strlen(message2));

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