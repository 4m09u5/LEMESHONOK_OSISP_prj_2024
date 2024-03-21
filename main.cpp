#include <iostream>
#include <fstream>
#include "bencode/BencodeParser.h"
#include <sstream>
#include "utils/sha1.h"
#include "network/http.h"
#include "torrent/Peer.h"
#include "bencode/TorrentFile.h"
#include "network/PeerConnection.h"
#include "utils/sha1.h"

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

    TorrentFile metadata("../example.torrent");

    size_t blockSize = 0x4000;

    std::cout << "Piece length: " << metadata.info.piece_length << std::endl;
    std::cout << "Desired block size: " << blockSize << std::endl;
    std::cout << "Blocks per piece: " << metadata.info.piece_length / blockSize << std::endl;

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

    for(const auto& peer : peers) {
        try {
            std::cout << "Connecting to " << peer.getAddr() << " - " << peer.getPort() << std::endl;
            PeerConnection connection(peer.getAddr(), peer.getPort());
            connection.sendHandshake();
            auto handshakeResponse = connection.receiveHandshake();
            std::cout << "Successfully shaked " << peer.getAddr() << " hand!" << std::endl;
            connection.sendInterested();
            auto interestedResponse = connection.receiveMessage();
            std::cout << "I told i am interested!" << std::endl;
/*          std::ofstream file("piece.bin", std::ios::out | std::ios::binary);

            file.write()*/

            std::vector<uint8_t> piece;

            for(size_t i = 0; i < metadata.info.piece_length; i += blockSize) {
                connection.sendRequest(0, i, blockSize);
                auto requestResponse = connection.receiveMessage();
                piece.insert(piece.end(), requestResponse.getPayload().begin() + 8, requestResponse.getPayload().end());
                std::cout << "Successfully got " << std::hex << i << " piece (" << requestResponse.getPayload().size() << ") size" << std::endl;
            }

            SHA1 hash;
            hash.update(piece);


            std::cout << "Desired hash: ";
            for(int i = 0; i < 20; i++) {
                std::cout << std::hex << +(uint8_t)metadata.info.pieces[0][i];
            }
            std::cout << "\nPiece hash: " << hash.final() << std::endl;
            std::cout << "Breakpoint me!" << std::endl;
            break;

        } catch (...) {
            std::cout << "Failed to perform a handshake" << std::endl;
        }
    }



    std::cout << "aboba";
}