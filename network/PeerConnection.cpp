//
// Created by dzmitry on 19.02.24.
//

#include "PeerConnection.h"

PeerConnection::PeerConnection(const Peer& peer) : connection(peer.getAddr(), peer.getPort()), peer(peer) {
    downloaded = 0;
    uploaded = 0;
}

bool PeerConnection::connect() {
    return connection.connect();
}

void PeerConnection::sendHandshake(std::string infoHash) {
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

    for (char c : std::string("-qB4630-k8hj0wgej6ch"))
        message.push_back(c);

    uploaded += connection.sendData(message);
}

void PeerConnection::sendKeepAlive() {
    Message keepAlive;
    uploaded += connection.sendData(keepAlive.getVector());
}

void PeerConnection::sendChoke() {
    Message choke(0);
    uploaded += connection.sendData(choke.getVector());
}

void PeerConnection::sendUnchoke() {
    Message unchoke(1);
    uploaded += connection.sendData(unchoke.getVector());
}

void PeerConnection::sendInterested() {
    Message interested(2);
    uploaded += connection.sendData(interested.getVector());
}

void PeerConnection::sendNotInterested() {
    Message notInterested(3);
    uploaded += connection.sendData(notInterested.getVector());
}

void PeerConnection::sendHave(uint32_t pieceIndex) {
    Message have(4);
    have.addPayload(pieceIndex);
    uploaded += connection.sendData(have.getVector());
}

void PeerConnection::sendBitfield(const std::vector<bool>& pieces) {
    Message bitfield(5);
    bitfield.addPayload(pieces);
    uploaded += connection.sendData(bitfield.getVector());
}

void PeerConnection::sendRequest(uint32_t index, uint32_t begin, uint32_t length) {
    Message request(6);
    request.addPayload(index);
    request.addPayload(begin);
    request.addPayload(length);
    uploaded += connection.sendData(request.getVector());
}

Message PeerConnection::receiveHandshake() {
    Message result;
    std::vector<uint8_t> packet = connection.receivePacket(0);
    downloaded += packet.size();
    if (packet.size() <= 4) {
        result.setPayload({});
        result.setId(0);
        return result;
    }
    result.setId(packet.at(0));
    result.setPayload(std::vector(packet.begin() + 1, packet.end()));

    return result;
}

Message PeerConnection::receiveMessage() {
    Message result;
    std::vector<uint8_t> packet = connection.receivePacket();
    downloaded += packet.size();

    if (packet.empty()) {
        // Handle empty packet case
        result.setId(-1);
        return result;
    }

    result.setId(packet[0]);

    if (packet.size() == 1) {
        result.setPayload({});
        return result;
    }

    std::vector<uint8_t> payload(packet.begin() + 1, packet.end());
    result.setPayload(payload);

    return result;
}

Peer PeerConnection::getPeer() {
    return peer;
};

size_t PeerConnection::getDownloaded() {
    return downloaded;
}

size_t PeerConnection::getUploaded() {
    return uploaded;
}