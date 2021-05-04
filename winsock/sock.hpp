#ifndef SOCK_H
#define SOCK_H

#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN // prevents including winsock.h (winsock 1.1) by windows.h

#include <cstdio>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h> // always AFTER winsock2

namespace sock {

  int init() {

    WSADATA wsaData;

    int status;

    // Initialize Winsock
    status = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (status) {
      printf("WSAStartup failed: %d\n", status);
      return 1;
    }

    #define DEFAULT_PORT "35000"

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    status = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (status) {
      printf("getaddrinfo failed: %d\n", status);
      WSACleanup();
      return 1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;
    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
      printf("Error at socket(): %ld\n", WSAGetLastError());
      freeaddrinfo(result);
      WSACleanup();
      return 1;
    }

    // Setup the TCP listening socket
    status = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (status == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      freeaddrinfo(result);
      closesocket(ListenSocket);
      WSACleanup();
      return 1;
    }

    freeaddrinfo(result);

    if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
      printf( "Listen failed with error: %ld\n", WSAGetLastError() );
      closesocket(ListenSocket);
      WSACleanup();
      return 1;
    }



  }

}

#endif // SOCK_H
