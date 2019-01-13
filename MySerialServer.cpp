//
// Created by newbuntu on 09/01/19.
//
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <streambuf>
#include "MySerialServer.h"
#include "Server.h"

using namespace std;


struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};

void MySerialServer::open(int port, ClientHandler *c) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    bool shouldStop = false;
    struct sockaddr_in serv;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(port);
    serv.sin_family = AF_INET;
    if (bind(s, (sockaddr *)&serv, sizeof(serv)) < 0)	{
        cerr << "Bad!" << endl;
    }
    while(!shouldStop) {
        struct sockaddr_in client;
        socklen_t clilen = sizeof(client);

        timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

        int new_sock = accept(s, (struct sockaddr *) &client, &clilen);
        if (new_sock < 0) {
            if (errno == EWOULDBLOCK) {
                cout << "timeout!" << endl;
                exit(2);
            } else {
                perror("other error");
                exit(3);
            }
        }
        this->sock_fd = new_sock;
        thread start_routine(&MySerialServer::start, this, new_sock, c);
        start_routine.join();
    }
}


void MySerialServer::start(int sockfd, ClientHandler* ch) {
    int new_sock, n;
    ch->handleClient(sockfd);
}

void MySerialServer::stop() {
    close(sock_fd);
}

MySerialServer::MySerialServer() {}
