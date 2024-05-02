//
// Created by dzmitry on 4/18/24.
//

#include <sstream>
#include "UDPTracker.h"

UDPTracker::UDPTracker(TorrentFile metadata) : connection(metadata.announce.hostname, metadata.announce.port), metadata(metadata) {
    connection.connect();
    connect();
}

void UDPTracker::connect() {
    ConnectionMessage message;
    message.transactionId = std::rand();

    auto ptr = reinterpret_cast<uint8_t*>(&message);
    auto data = std::vector<uint8_t>(ptr, ptr + sizeof(message));

    connection.sendData(data);

    auto response = connection.receivePacket();

    connectionId = *(uint64_t*)(response.data() + 8);
}

std::vector<Peer> UDPTracker::getPeers() {
    AnnounceMessage message;
    message.connectionId = connectionId;
    message.transactionId = std::rand();
    message.port = 5342;

    auto hash = metadata.infoHash;

    int i = 0;
    for(uint8_t* ptr = message.infohash; i < 40; i += 2) {
        (*ptr) = 0;
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

    auto ptr = reinterpret_cast<uint8_t*>(&message);
    auto data = std::vector<uint8_t>(ptr, ptr + sizeof(message));


    connection.sendData(data);
    auto response = connection.receivePacket();

    std::vector<Peer> peers;

    for (auto it = response.begin() + 20; it != response.end(); it += 6) {
        std::stringstream host, port;
        host << +static_cast<unsigned char>(*it) << "." << +static_cast<unsigned char>(*(it + 1)) << "." <<
             +static_cast<unsigned char>(*(it + 2)) << "." << +static_cast<unsigned char>(*(it + 3));
        port << +static_cast<unsigned short>(static_cast<unsigned short>(*(it + 4)) * 256 +
                                             static_cast<unsigned char>(*(it + 5)));
        peers.emplace_back(host.str(), port.str());
    }

    return peers;
}

Peer UDPTracker::getPeer() {
    while (peers.empty()) {
        peers = getPeers();
    }

    auto result = peers.back();
    peers.pop_back();

    return result;
}

