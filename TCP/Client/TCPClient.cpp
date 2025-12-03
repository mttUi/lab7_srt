#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << "=== TCP КЛИЕНТ ===" << std::endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации WinSock" << std::endl;
        return 1;
    }

    // Создание TCP сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8889);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка подключения: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Подключение к TCP серверу установлено" << std::endl;
    std::cout << "Введите сообщения (!quit для выхода, !stop для остановки сервера):" << std::endl;

    char buffer[4096];
    char response[4096];

    while (true) {
        std::cout << "> ";
        std::string userInput;
        std::getline(std::cin, userInput);

        if (userInput.empty()) {
            continue;
        }

        // Отправка сообщения
        if (send(clientSocket, userInput.c_str(), userInput.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Ошибка отправки: " << WSAGetLastError() << std::endl;
            break;
        }

        // Проверка на выход
        if (userInput == "!quit") {
            std::cout << "Завершение работы клиента..." << std::endl;
            break;
        }

        // Получение ответа (TCP гарантирует доставку)
        memset(response, 0, sizeof(response));
        int bytesReceived = recv(clientSocket, response, sizeof(response), 0);

        if (bytesReceived > 0) {
            std::cout << "Эхо-ответ сервера: " << response << std::endl;
        }
        else if (bytesReceived == 0) {
            std::cout << "Сервер отключился" << std::endl;
            break;
        }
        else {
            std::cout << "Ошибка приема ответа" << std::endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    std::cout << "TCP клиент завершил работу" << std::endl;
    return 0;
}