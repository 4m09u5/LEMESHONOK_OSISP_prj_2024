//
// Created by dzmitry on 19.02.24.
//

#ifndef BITTORRENT_MESSAGE_H
#define BITTORRENT_MESSAGE_H


#include <cstdint>
#include <vector>
#include <string>

enum MessageType {
    CHOKE = 0,
    UNCHOKE = 1,
    INTERESTED = 2,
    NOT_INTERESTED = 3,
    HAVE = 4,
    BITFIELD = 5,
    REQUEST = 6,
    PIECE = 7,
    CANCEL = 8,
    PORT = 9,
    HAVE_ALL = 14,
    ALLOWED_FAST = 17,
};

class Message {
    int16_t id;
    std::vector<uint8_t> payload;
public:
    Message(uint8_t id);
    Message();

    void addPayload(const std::vector<uint8_t>& data);
    void addPayload(uint32_t data);
    void addPayload(const std::vector<bool>& data);
    std::vector<uint8_t> getVector();

    const int16_t &getId() const;

    void setId(const int16_t &id);

    const std::vector<uint8_t> &getPayload() const;

    void setPayload(const std::vector<uint8_t> &payload);
};


#endif //BITTORRENT_MESSAGE_H
