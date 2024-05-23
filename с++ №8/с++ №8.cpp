#define WIN32_LEAN_AND_MEAN // Уменьшение размера заголовочного файла Windows

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; // Структура для хранения данных о версии Winsock
    ADDRINFO hints; // Структура для хранения информации об адресе
    ADDRINFO* addrResult; // Указатель для хранения результата getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для установления соединения
    char recvBuffer[512]; // Буфер для получения данных

    const char* sendBuffer1 = "Hello from client 1"; // Сообщение 1 для отправки серверу
    const char* sendBuffer2 = "Hello from client 2"; // Сообщение 2 для отправки серверу

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup не удалось с результатом: " << result << endl;
        return 1;
    }

    // Подготовка структуры hints
    ZeroMemory(&hints, sizeof(hints)); // Обнуление структуры hints
    hints.ai_family = AF_INET; // Использование IPv4
    hints.ai_socktype = SOCK_STREAM; // Использование TCP
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Получение адресной информации для подключения к серверу
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Завершение работы с Winsock
        return 1;
    }

    // Создание сокета для подключения
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Завершение работы с Winsock
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу" << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        ConnectSocket = INVALID_SOCKET; // Обнуление сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Завершение работы с Winsock
        return 1;
    }

    // Отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась, ошибка: " << result << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Завершение работы с Winsock
        return 1;
    }
    cout << "Отправлено: " << result << " байт" << endl;

    // Отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась, ошибка: " << result << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Завершение работы с Winsock
        return 1;
    }
    cout << "Отправлено: " << result << " байт" << endl;

    // Завершение отправки данных (закрытие отправляющей части сокета)
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Завершение отправки не удалось, ошибка: " << result << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Завершение работы с Winsock
        return 1;
    }

    // Цикл для получения данных от сервера
    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера для получения данных
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных от сервера
        if (result > 0) {
            cout << "Получено " << result << " байт" << endl;
            cout << "Полученные данные: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Соединение закрыто" << endl;
        }
        else {
            cout << "Получение данных не удалось, ошибка: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Закрытие соединения и освобождение ресурсов
    closesocket(ConnectSocket); // Закрытие сокета
    freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
    WSACleanup(); // Завершение работы с Winsock
    return 0;
}
