//
// Created by dzmitry on 3/28/24.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include "tcp.h"

TCP::TCP(std::string ip, std::string port) {
    this->ip = ip;
    this->port = port;
    connected = false;
}

bool TCP::connect() {
    sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        return false;

    hostent *server = gethostbyname(ip.c_str());
    if (server == NULL){
        std::cout << "could Not resolve hostname :(" << std::endl;
        close(sockfd);
        return false;
    }

    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(std::stoi(port));
    bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);

    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,sizeof(timeout));

    if (::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))!= 0)
        return false;

    connected = true;

    return true;
}

void TCP::sendData(std::vector<uint8_t> data) {
    if (!connected)
        return;

    send(sockfd, data.data(), data.size(), 0);
}

void TCP::disconnect() {
    close(sockfd);
}

std::vector<uint8_t> TCP::receiveData() {
    std::vector<uint8_t> result(0x40);

    ssize_t bytesRead = recv(sockfd, result.data(), result.size(), 0);

    if (bytesRead == -1)
        return {};

    result.resize(bytesRead);

    return result;
}


std::vector<uint8_t> TCP::receivePacket(uint32_t packetSize) {
    uint32_t total = 0;
    std::vector<uint8_t> result(packetSize);

    if(!packetSize) {
        ssize_t code = recv(sockfd, &packetSize, sizeof(packetSize), MSG_WAITALL);

        if (code == 0) {
            std::cout << "Pidar niche ne skazal" << std::endl;
            return {};
        }

        if (packetSize == 1953055251) {
            return receiveData();
        }

        packetSize = __builtin_bswap32(packetSize);

        result.resize(packetSize);
    }

    while (total < packetSize) {
        ssize_t bytesRead = recv(sockfd, result.data() + total, packetSize, 0);

        if (bytesRead == -1)
            return {};

        total += bytesRead;
    }

    return result;
}
