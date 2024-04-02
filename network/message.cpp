//
// Created by dzmitry on 19.02.24.
//

#include <cstdint>
#include "message.h"

Message::Message(int16_t id) {
    this->id = id;
}

Message::Message() {
    this->id = -1;
}

void Message::addPayload(const std::vector<uint8_t> &data) {
    payload.insert(payload.end(), data.begin(), data.end());
}

void Message::addPayload(const std::vector<bool> &data) {
    payload.insert(payload.end(), data.begin(), data.end());
}

std::vector<uint8_t> Message::getVector() {
    std::vector<uint8_t> data;

    for(int i = 0; i < 4; i++) {
        data.emplace_back(static_cast<uint8_t>((payload.size() + 1) >> (8 * (3 - i))));
    }

    if(id != -1)
        data.emplace_back(id);

    for(auto c : payload) {
        data.emplace_back(c);
    }

    return data;
}

const int16_t &Message::getId() const {
    return id;
}

void Message::setId(const int16_t &data) {
    this->id = data;
}

const std::vector<uint8_t> &Message::getPayload() const {
    return payload;
}

void Message::setPayload(const std::vector<uint8_t> &payload) {
    this->payload = payload;
}

void Message::addPayload(uint32_t data) {
    for(int i = 0; i < 4; i++) {
        payload.push_back(static_cast<uint8_t>(data >> (8 * (3 - i))));
    }
}
