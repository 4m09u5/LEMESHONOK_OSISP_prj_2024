//
// Created by dzmitry on 19.02.24.
//

#include "message.h"

Message::Message(uint8_t id) {
    this->length = 1;
    this->id = id;
}

Message::Message() {
    this->length = 0;
}

void Message::addPayload(const std::vector<uint8_t> &data) {
    payload.insert(payload.end(), data.begin(), data.end());
    length += data.size();
}

std::string Message::toString() {
    std::string data;

    for(int i = 0; i < 4; i++) {
        data += static_cast<uint8_t>(length >> (8 * (3 - i)));
    }

    if(length != 0)
        data += id;

    for(auto c : payload) {
        data += c;
    }

    return data;
}

void Message::addPayload(uint32_t data) {
    for(int i = 0; i < 4; i++) {
        payload.push_back(static_cast<uint8_t>(data >> (8 * (3 - i))));
    }
}
