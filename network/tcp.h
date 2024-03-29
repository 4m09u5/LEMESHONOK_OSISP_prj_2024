//
// Created by dzmitry on 3/28/24.
//

#ifndef BITTORRENT_TCP_H
#define BITTORRENT_TCP_H

#include <string>
#include <vector>
#include <cstdint>

class TCP {
    std::string ip, port;
    int sockfd;
    bool connected;

    std::vector<uint8_t> receiveData();
public:
    TCP(std::string ip, std::string port);
    bool connect();
    void disconnect();

    void sendData(std::vector<uint8_t> data);

    std::vector<uint8_t> receivePacket();

    std::vector<uint8_t> receivePacket(uint32_t buffersize);
};


#endif //BITTORRENT_TCP_H
