//
// Created by dzmitry on 4/18/24.
//

#include <sstream>
#include "UDPPeerManager.h"

UDPPeerManager::UDPPeerManager(std::string hostname, std::string port) : connection(hostname, port) {
    connection.connect();
}

void UDPPeerManager::connect() {
    std::vector<uint8_t> data{0x00, 0x00, 0x04, 0x17, 0x27, 0x10, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};

    connection.sendData(data);

    auto response = connection.receivePacket();

    connection_id = *(uint64_t*)(response.data() + 8);
}

std::vector<Peer> UDPPeerManager::getPeers(std::string hash) {
    std::vector<Peer> peers;

    std::vector<uint8_t> data{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x22, 0x33, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34};

    (*(uint64_t*)data.data()) = connection_id;

    int i = 0;
    for(uint8_t* ptr = data.data() + 16; i < 40; i += 2) {
        if (tolower(hash[i]) >= 'a')
            (*ptr) += (hash[i] - 'a' + 10) * 0x10;
        else
            (*ptr) += (hash[i] - '0') * 0x10;
        if (tolower(hash[i + 1]) >= 'a')
            (*ptr) += hash[i + 1] - 'a' + 10;
        else
            (*ptr) += hash[i + 1] - '0';
        ptr++;
    }

    connection.sendData(data);

    auto response = connection.receivePacket();

    for(auto it = response.begin() + 20; it != response.end(); it+=6) {
        std::stringstream host, port;
        host << +static_cast<unsigned char>(*it) << "." << +static_cast<unsigned char>(*(it + 1)) << "." <<
             +static_cast<unsigned char>(*(it + 2)) << "." << +static_cast<unsigned char>(*(it + 3));
        port << +static_cast<unsigned short>(static_cast<unsigned short>(*(it + 4)) * 256 + static_cast<unsigned char>(*(it + 5)));
        peers.emplace_back(host.str(), port.str());
    }

    return peers;
}

