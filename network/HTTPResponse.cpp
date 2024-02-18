//
// Created by dzmitry on 18.02.24.
//

#include "HTTPResponse.h"

HTTPResponse::HTTPResponse(std::string raw) {
    rawData = raw;
}

std::string HTTPResponse::getData() {
    size_t index = rawData.find("\r\n\r\n");
    if ( index != 0 )
    {
        index += 4;
        // do something with the data
    }
    return rawData.substr(index, rawData.size() - index);
}
