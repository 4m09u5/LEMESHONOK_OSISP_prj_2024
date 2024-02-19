//
// Created by dzmitry on 19.02.24.
//

#ifndef BITTORRENT_MESSAGE_H
#define BITTORRENT_MESSAGE_H


#include <cstdint>
#include <vector>
#include <string>

class Message {
    uint32_t length;
    uint8_t id;
    std::vector<uint8_t> payload;
public:
    Message(uint8_t id);
    Message();

    void addPayload(const std::vector<uint8_t>& data);
    void addPayload(uint32_t data);
    std::string toString();
};


#endif //BITTORRENT_MESSAGE_H
