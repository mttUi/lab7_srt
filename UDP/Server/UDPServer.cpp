#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Установка русской локали
    setlocale(LC_ALL, "Russian");

    std::cout << "=== UDP СЕРВЕР ===" << std::endl;

    // 1. Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации WinSock" << std::endl;
        return 1;
    }
    std::cout << "WinSock инициализирован" << std::endl;

    // 2. Создание UDP сокета
    // AF_INET - Internet-домен (IPv4)
    // SOCK_DGRAM - тип сокета для UDP
    // IPPROTO_UDP - протокол UDP
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "UDP сокет создан" << std::endl;

    // 3. Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;           // Семейство адресов IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;   // Принимать соединения на все интерфейсы
    serverAddr.sin_port = htons(8888);         // Порт 8888 (host to network short)

    // 4. Привязка сокета к адресу
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки сокета: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Сервер запущен на порту 8888" << std::endl;
    std::cout << "Ожидание сообщений от клиентов..." << std::endl;
    std::cout << "Для выхода введите 'exit' в клиенте" << std::endl;

    // 5. Память для приема информации
    char buffer[1024];           // Буфер для приема сообщений
    sockaddr_in clientAddr;      // Информация о клиенте
    int clientAddrSize = sizeof(clientAddr);

    // Жизненный цикл сервера
    while (true) {
        // 1) Очистка временной памяти
        memset(buffer, 0, sizeof(buffer));
        memset(&clientAddr, 0, sizeof(clientAddr));

        // 2) Запуск ожидания сообщения
        std::cout << "\nОжидание данных..." << std::endl;

        // 3) Прием сообщения
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
            (sockaddr*)&clientAddr, &clientAddrSize);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Ошибка приема данных: " << WSAGetLastError() << std::endl;
            continue;
        }

        // 4) Чтение сообщения
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

        std::cout << "Получено от " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
        std::cout << "Сообщение: " << buffer << std::endl;
        std::cout << "Размер: " << bytesReceived << " байт" << std::endl;

        // Проверка на команду выхода
        if (strcmp(buffer, "exit") == 0) {
            std::cout << "Получена команда выхода. Завершение работы..." << std::endl;
            break;
        }

        // Отправка подтверждения клиенту
        std::string response = "Сервер получил: ";
        response += buffer;
        sendto(serverSocket, response.c_str(), response.length(), 0,
            (sockaddr*)&clientAddr, clientAddrSize);
        std::cout << "Отправлено подтверждение клиенту" << std::endl;
    }

    // 6. Очистка ресурсов
    closesocket(serverSocket);
    WSACleanup();
    std::cout << "Сервер завершил работу" << std::endl;

    return 0;
}