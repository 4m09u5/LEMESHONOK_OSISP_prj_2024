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
    PeerConnection(const std::string& ip, const std::string& port) : connection(ip, port) {
        this->ip = ip;
        this->port = port;
    }

    bool connect() {
        return connection.connect();
    }

    void changePort(uint16_t port) {
        this->port = port;
        connection.disconnect();
        connection.connect();
    }

    void sendHandshake(std::string infoHash) {
        std::vector<uint8_t> a{0x13, 'B', 'i', 't', 'T', 'o', 'r', 'r', 'e', 'n', 't', ' ', 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x08, 0xe4, 0x05, 0x54, 0xd7, 0x9c, 0xc3, 0x1d, 0x7b, 0x2b, 0xd6, 0x2b, 0x61, 0x9d, 0x44, 0xfd, 0x39, 0xaf, 0xf3, 0x37 ,'-', 'q', 'B', '4', '6', '3', '0', '-', 'k','8','h','j','0','w','g','e','j','6','c','h'};
        //std::vector<uint8_t> a{0x13, 'B', 'i', 't', 'T', 'o', 'r', 'r', 'e', 'n', 't', ' ', 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2e, 0x90, 0x23, 0x31, 0xba, 0xa5, 0xa9, 0x48, 0x44, 0xe0, 0x34, 0x86, 0xae, 0xc2, 0x5f, 0x28, 0xd5, 0x40, 0x3d, 0x67 ,'-', 'q', 'B', '4', '6', '3', '0', '-', 'k','8','h','j','0','w','g','e','j','6','c','h'};

        std::vector<uint8_t> message;

        message.push_back(13);
        for(char c : "BitTorrent protocol")
            message.push_back(c);
        for(int i = 0; i < 8; i++)
            message.push_back(0);
        

        connection.sendData(a);
    }

    void sendKeepAlive() {
        Message keepAlive;
        connection.sendData(keepAlive.getVector());
    }

    void sendChoke() {
        Message choke(0);
        connection.sendData(choke.getVector());
    }

    void sendUnchoke() {
        Message unchoke(1);
        connection.sendData(unchoke.getVector());
    }

    void sendInterested() {
        Message interested(2);
        connection.sendData(interested.getVector());
    }

    void sendNotInterested() {
        Message notInterested(3);
        connection.sendData(notInterested.getVector());
    }

    void sendHave(uint32_t pieceIndex) {
        Message have(4);
        have.addPayload(pieceIndex);
        connection.sendData(have.getVector());
    }

    void sendBitfield(const std::vector<bool>& pieces) {
        Message bitfield(5);
        bitfield.addPayload(pieces);
        connection.sendData(bitfield.getVector());
    }

    void sendRequest(uint32_t index, uint32_t begin, uint32_t length) {
        Message request(6);
        request.addPayload(index);
        request.addPayload(begin);
        request.addPayload(length);
        connection.sendData(request.getVector());
    }

    Message receiveHandshake() {
        Message result;
        std::vector<uint8_t> packet = connection.receivePacket(0);
        if (packet.size() <= 4) {
            result.setPayload({});
            result.setId(0);
            return result;
        }
       result.setId(packet.at(0));
        result.setPayload(std::vector(packet.begin() + 1, packet.end()));

        return result;
    }

    Message receiveMessage() {
        Message result;
        std::vector<uint8_t> packet = connection.receivePacket(0);
        if (packet.empty())
            return {-1};
        result.setId(packet.at(0));

        if (packet.size() == 1) {
            result.setPayload({});
            return result;
        }

        if(result.getId() > 1)
            result.setPayload(std::vector<uint8_t>(packet.begin() + 1, packet.end()));

        return result;
    }
};


#endif //BITTORRENT_PEERCONNECTION_H
