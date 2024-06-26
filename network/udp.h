//
// Created by dzmitry on 4/11/24.
//

#ifndef BITTORRENT_UDP_H
#define BITTORRENT_UDP_H

#include <string>
#include <vector>
#include <cstdint>

class UDP {
    std::string ip, port;
    int sockfd;

    std::vector<uint8_t> receiveData();
public:
    UDP(std::string ip, std::string port);
    ~UDP();
    bool connect();

    void sendData(std::vector<uint8_t> data);
    std::vector<uint8_t> receivePacket(size_t buffersize = 0x800);
};

#endif //BITTORRENT_UDP_H
