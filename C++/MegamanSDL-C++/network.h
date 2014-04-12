#ifndef SERVER_H
#define SERVER_H

#define _WIN32_WINNT 0x501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

int setup_server();
int server_send(char *send_string);
int close_server();
int setup_client(char *ipaddress);
int receive_data(char *send_string);
int close_client();

#endif
