//
// Created by dzmitry on 19.02.24.
//

#ifndef BITTORRENT_PEERCONNECTION_H
#define BITTORRENT_PEERCONNECTION_H


#include <string>
#include "tcp.h"
#include "message.h"

class PeerConnection {
    std::string ip;
    std::string port;
    TCP connection;
public:
    PeerConnection(const std::string& ip, const std::string& port) : connection(ip, std::stoi(port)) {
        this->ip = ip;
        this->port = port;
        connection.connectToServer();
    }

    std::string sendHandshake() {
        connection.sendData("\x13""BitTorrent protocol\x00\x00\x00\x00\x00\x00\x00\x00\x9D\x61\x14\xC0\x3F\xEB\x85\x35\x49\xB5\x02\x3A\x04\xE4\x92\x4F\xA5\xFF\x47\x57\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10");
        return connection.receiveData();
    }

    void sendKeepAlive() {
        Message keepAlive;
        connection.sendData(keepAlive.toString());
    }

    void sendChoke() {
        Message choke(0);
        connection.sendData(choke.toString());
    }

    void sendUnchoke() {
        Message unchoke(1);
        connection.sendData(unchoke.toString());
    }

    void sendInterested() {
        Message interested(2);
        connection.sendData(interested.toString());
        connection.receiveData();
    }

    void sendNotInterested() {
        Message notInterested(3);
        connection.sendData(notInterested.toString());
    }

    std::string sendHave(uint32_t pieceIndex) {
        Message have(4);
        have.addPayload(pieceIndex);
        connection.sendData(have.toString());
        auto result = connection.receiveData();
        return result;
    }
};


#endif //BITTORRENT_PEERCONNECTION_H
