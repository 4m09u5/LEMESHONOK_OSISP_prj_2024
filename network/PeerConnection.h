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

    ~PeerConnection() {
        connection.disconnect();
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
        std::vector<uint8_t> message;

        message.push_back(0x13);
        for(char c : "BitTorrent protocol")
            message.push_back(c);
        for(int i = 0; i < 7; i++)
            message.push_back(0);

        for(int i = 0; i < 40; i += 2) {
            uint8_t result = 0;

            if (tolower(infoHash[i]) >= 'a')
                result += (infoHash[i] - 'a' + 10) * 0x10;
            else
                result += (infoHash[i] - '0') * 0x10;

            if (tolower(infoHash[i + 1]) >= 'a')
                result += infoHash[i + 1] - 'a' + 10;
            else
                result += infoHash[i + 1] - '0';

            message.push_back(result);
        }

        for(char c : "-qB4630-k8hj0wgej6ch")
            message.push_back(c);

        connection.sendData(message);
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
        std::vector<uint8_t> packet = connection.receivePacket();
        if (packet.empty())
            return {-1};
        result.setId(packet.at(0));

        if (packet.size() == 1) {
            result.setPayload({});
            return result;
        }

        if (result.getId() > 1) {
            std::vector<uint8_t> payload = {packet.begin() + 1, packet.end()};
            result.setPayload(payload);
        }

        return result;
    }
};


#endif //BITTORRENT_PEERCONNECTION_H
