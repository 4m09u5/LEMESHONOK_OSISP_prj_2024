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

    std::vector<uint8_t> sendHandshake() {
        std::vector<uint8_t> a{0x13, 'B', 'i', 't', 'T', 'o', 'r', 'r', 'e', 'n', 't', ' ', 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l', 0x0, 0x0, 0x0, 0x00, 0x00, 0x10, 0x00, 0x05, 0x9D, 0x61, 0x14, 0xC0, 0x3F, 0xEB, 0x85, 0x35, 0x49, 0xB5, 0x02, 0x3A, 0x04, 0xE4, 0x92, 0x4F, 0xA5, 0xFF, 0x47, 0x57, '-', 'q', 'B', '4', '6', '3', '0', '-', 'k','8','h','j','0','w','g','e','j','6','c','h'};
        connection.sendData(a);

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
    }

    void sendNotInterested() {
        Message notInterested(3);
        connection.sendData(notInterested.toString());
    }

    void sendHave(uint32_t pieceIndex) {
        Message have(4);
        have.addPayload(pieceIndex);
        connection.sendData(have.toString());
    }

    void sendBitfield(const std::vector<bool>& pieces) {
        Message bitfield(5);
        bitfield.addPayload(pieces);
        connection.sendData(bitfield.toString());
    }

    void sendRequest(uint32_t index, uint32_t begin, uint32_t length) {
        Message request(6);
        request.addPayload(index);
        request.addPayload(begin);
        request.addPayload(length);
        connection.sendData(request.toString());
    }

    Message receiveHandshake() {
        Message result;
        std::vector<uint8_t> packet = connection.receiveData();
        if (packet.size() <= 4) {
            result.setLength(0);
            result.setPayload({});
            result.setId(0);
            return result;
        }
        result.setLength((packet.at(0) << 24) + (packet.at(1) << 16) + (packet.at(2) << 8) + packet.at(3));
        result.setId(packet.at(4));
        result.setPayload(std::vector(packet.begin() + 5, packet.end()));

        return result;
    }

    Message receiveMessage() {
        Message result;
        std::vector<uint8_t> packet = connection.receivePacket();
        if (packet.size() <= 4) {
            result.setLength(0);
            result.setPayload({});
            result.setId(0);
            return result;
        }
        result.setLength((packet.at(0) << 24) + (packet.at(1) << 16) + (packet.at(2) << 8) + packet.at(3));
        result.setId(packet.at(4));
        result.setPayload(std::vector(packet.begin() + 5, packet.end()));

        return result;
    }
};


#endif //BITTORRENT_PEERCONNECTION_H
