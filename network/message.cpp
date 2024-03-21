//
// Created by dzmitry on 19.02.24.
//

#include <cstdint>
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

void Message::addPayload(const std::vector<bool> &data) {
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

const uint32_t &Message::getLength() const {
    return length;
}

void Message::setLength(const uint32_t &length) {
    Message::length = length;
}

const uint8_t &Message::getId() const {
    return id;
}

void Message::setId(const uint8_t &id) {
    Message::id = id;
}

const std::vector<uint8_t> &Message::getPayload() const {
    return payload;
}

void Message::setPayload(const std::vector<uint8_t> &payload) {
    Message::payload = payload;
}

void Message::addPayload(uint32_t data) {
    length += 4;
    for(int i = 0; i < 4; i++) {
        payload.push_back(static_cast<uint8_t>(data >> (8 * (3 - i))));
    }
}
