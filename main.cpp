#include <iostream>
#include <fstream>
#include "bencode/BencodeParser.h"
#include <sstream>
#include "utils/sha1.h"
#include "network/http.h"
#include "torrent/Peer.h"
#include "bencode/TorrentFileParser.h"
#include "network/PeerConnection.h"

void printPeers(std::string peers) {
    for(auto it = peers.begin(); it != peers.end(); it+=6) {
        std::cout << +static_cast<unsigned char>(*it) << "." << +static_cast<unsigned char>(*(it + 1)) << "." <<
                +static_cast<unsigned char>(*(it + 2)) << "." << +static_cast<unsigned char>(*(it + 3)) << ":" <<
                +static_cast<unsigned short>(static_cast<unsigned short>(*(it + 4)) * 256 + static_cast<unsigned char>(*(it + 5))) << std::endl;
    }
}

std::vector<Peer> parsePeers(const std::string& raw) {
    std::vector<Peer> peers;
    for(auto it = raw.begin(); it != raw.end(); it+=6) {
        std::stringstream host, port;
        host << +static_cast<unsigned char>(*it) << "." << +static_cast<unsigned char>(*(it + 1)) << "." <<
            +static_cast<unsigned char>(*(it + 2)) << "." << +static_cast<unsigned char>(*(it + 3));
        port << +static_cast<unsigned short>(static_cast<unsigned short>(*(it + 4)) * 256 + static_cast<unsigned char>(*(it + 5)));
        peers.emplace_back(host.str(), port.str());
    }

    return peers;
}

int main() {
    auto parser = BencodeParser();

    auto metadata = parseTorrentFile("../example.torrent");

    std::string hash = URLEncode(metadata.infoHash);

    HTTP request(metadata.announce.hostname, metadata.announce.port, metadata.announce.query);
    request.addParameter("info_hash", hash);
    request.addParameter("peer_id", "%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10");
    request.addParameter("port", "44953");
    request.addParameter("uploaded", "0");
    request.addParameter("downloaded", "0");
    request.addParameter("left", "0");

    auto response = request.execute();
    std::cout << response.getData();


    auto data = parser.parse(response.getData());
    auto peers = parsePeers(data["peers"].rawValue);

    std::cout << std::endl;
    printPeers(data["peers"].rawValue);
    std::cout << "Connecting to " << peers[0].getAddr() << " - " << peers[0].getPort() << std::endl;
    PeerConnection connection("78.157.236.161", "21499");

    auto handshake = connection.sendHandshake();
    connection.sendInterested();
    auto resp = connection.sendHave(0);
    std::cout << "aboba";
}