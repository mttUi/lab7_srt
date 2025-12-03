# Лаба №7
Выполнил Балыбердин Егор, 3МО РБД-1

# Очтет
Выводы
UDP (User Datagram Protocol):

Без установки соединения

Нет гарантий доставки и порядка пакетов

Использует SOCK_DGRAM

Методы: sendto(), recvfrom()

TCP (Transmission Control Protocol):

С установкой соединения (handshake)

Гарантированная доставка и порядок пакетов

Использует SOCK_STREAM

Методы: connect(), accept(), listen(), send(), recv()

Основные различия:

UDP быстрее, но ненадежен

TCP медленнее, но надежен

TCP требует listen() и accept() для сервера

UDP работает с датаграммами, TCP - с потоком байтов
