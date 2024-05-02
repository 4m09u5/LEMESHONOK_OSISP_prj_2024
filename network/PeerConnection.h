//
// Created by dzmitry on 19.02.24.
//

#ifndef BITTORRENT_PEERCONNECTION_H
#define BITTORRENT_PEERCONNECTION_H


#include <string>
#include <iostream>
#include "tcp.h"
#include "message.h"
#include "../torrent/Peer.h"

class PeerConnection {
    Peer peer;
    TCP connection;

    size_t downloaded;
    size_t uploaded;
public:
    explicit PeerConnection(const Peer& peer);

    bool connect();

    void sendHandshake(std::string infoHash);
    void sendKeepAlive();
    void sendChoke();
    void sendUnchoke();
    void sendInterested();
    void sendNotInterested();
    void sendHave(uint32_t pieceIndex);
    void sendBitfield(const std::vector<bool>& pieces);
    void sendRequest(uint32_t index, uint32_t begin, uint32_t length);
    Message receiveHandshake();
    Message receiveMessage();
    Peer getPeer();
    size_t getDownloaded();
    size_t getUploaded();
};


#endif //BITTORRENT_PEERCONNECTION_H
