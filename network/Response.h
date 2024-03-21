//
// Created by dzmitry on 27.02.24.
//

#ifndef BITTORRENT_RESPONSE_H
#define BITTORRENT_RESPONSE_H

#include <vector>
#include "message.h"

class Response {
    std::vector<Message> messages;
public:
    Response(std::vector<uint8_t>& data);
};


#endif //BITTORRENT_RESPONSE_H
