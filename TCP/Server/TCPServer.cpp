#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <chrono>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << "=== TCP СЕРВЕР ===" << std::endl;

    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации WinSock" << std::endl;
        return 1;
    }

    // Создание TCP сокета (SOCK_STREAM вместо SOCK_DGRAM)
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Настройка адреса
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8889);  // Другой порт для TCP

    // Привязка
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Прослушивание (listen для TCP)
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Ошибка прослушивания: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "TCP сервер запущен на порту 8889" << std::endl;
    std::cout << "Условия выхода: после 5 минут работы или по команде !stop" << std::endl;

    auto startTime = std::chrono::steady_clock::now();
    const auto TIMEOUT = std::chrono::minutes(5);

    while (true) {
        // Проверка таймаута (условие выхода 1)
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime > TIMEOUT) {
            std::cout << "Время работы истекло (5 минут). Завершение..." << std::endl;
            break;
        }

        std::cout << "\nОжидание подключения клиента..." << std::endl;

        // Принятие подключения
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Ошибка принятия подключения: " << WSAGetLastError() << std::endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "Клиент подключен: " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

        char buffer[4096];
        bool clientConnected = true;

        while (clientConnected) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytesReceived <= 0) {
                std::cout << "Клиент отключился" << std::endl;
                break;
            }

            std::cout << "Получено: " << buffer << " (" << bytesReceived << " байт)" << std::endl;

            // Проверка на спецсимвол для выхода (условие выхода 2)
            if (strcmp(buffer, "!stop") == 0) {
                std::cout << "Получена команда остановки сервера" << std::endl;
                clientConnected = false;
            }

            // Эхо-ответ (TCP гарантирует доставку)
            send(clientSocket, buffer, bytesReceived, 0);
        }

        closesocket(clientSocket);
        if (strcmp(buffer, "!stop") == 0) {
            break;
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    std::cout << "TCP сервер завершил работу" << std::endl;
    return 0;
}