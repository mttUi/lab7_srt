#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Установка русской локали
    setlocale(LC_ALL, "Russian");

    std::cout << "=== UDP КЛИЕНТ ===" << std::endl;

    // 1. Инициализация WinSock (аналогично серверу)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации WinSock" << std::endl;
        return 1;
    }

    // 2. Создание UDP сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);  // Порт сервера

    // Преобразование IP-адреса из текста в бинарный формат
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1) {
        std::cerr << "Ошибка преобразования IP-адреса" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Подключение к серверу 127.0.0.1:8888" << std::endl;
    std::cout << "Введите сообщения (для выхода введите 'exit'):" << std::endl;

    char buffer[1024];
    char responseBuffer[1024];

    // Жизненный цикл клиента
    while (true) {
        // 1) очищаем сообщение
        memset(buffer, 0, sizeof(buffer));
        memset(responseBuffer, 0, sizeof(responseBuffer));

        // 2) считываем новое сообщение из консоли
        std::cout << "\n> ";
        std::string userInput;
        std::getline(std::cin, userInput);

        if (userInput.empty()) {
            continue;
        }

        // Проверка на команду выхода
        if (userInput == "exit") {
            std::cout << "Завершение работы..." << std::endl;

            // Отправляем команду выхода серверу
            sendto(clientSocket, "exit", 4, 0,
                (sockaddr*)&serverAddr, sizeof(serverAddr));
            break;
        }

        // 3) отправляем данные
        int sendResult = sendto(clientSocket, userInput.c_str(),
            userInput.length(), 0,
            (sockaddr*)&serverAddr, sizeof(serverAddr));

        // 4) проверяем удалось ли отправить
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Ошибка отправки: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Сообщение отправлено (" << sendResult << " байт)" << std::endl;

        // Ожидание ответа от сервера
        sockaddr_in fromAddr;
        int fromAddrSize = sizeof(fromAddr);

        int bytesReceived = recvfrom(clientSocket, responseBuffer,
            sizeof(responseBuffer), 0,
            (sockaddr*)&fromAddr, &fromAddrSize);

        if (bytesReceived > 0) {
            std::cout << "Ответ сервера: " << responseBuffer << std::endl;
        }
        else if (bytesReceived == 0) {
            std::cout << "Сервер отключился" << std::endl;
            break;
        }
        else {
            std::cout << "Ошибка приема ответа" << std::endl;
        }
    }

    // Очистка ресурсов
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Клиент завершил работу" << std::endl;

    return 0;
}