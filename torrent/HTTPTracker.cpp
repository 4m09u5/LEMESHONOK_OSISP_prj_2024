//
// Created by dzmitry on 4/28/24.
//

#include "HTTPTracker.h"
#include "../network/http.h"
#include "../bencode/TorrentFile.h"
#include "../bencode/BencodeParser.h"

std::vector<Peer> HTTPTracker::getPeers(TorrentFile metadata) {
    std::string encodedHash = URLEncode(metadata.infoHash);

    HTTP request(metadata.announce.hostname, metadata.announce.port, metadata.announce.query);
    request.addParameter("info_hash", encodedHash);
    request.addParameter("peer_id", "%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10");
    request.addParameter("port", "44953");
    request.addParameter("uploaded", "0");
    request.addParameter("downloaded", "0");
    request.addParameter("left", "0");

    BencodeParser parser;
    auto response = request.execute();
    std::cout << response.getData();


    auto data = parser.parse(response.getData())["peers"].rawValue;
    std::vector<Peer> peers;

    for(auto it = data.begin(); it != data.end(); it+=6) {
        std::stringstream host, port;
        host << +static_cast<unsigned char>(*it) << "." << +static_cast<unsigned char>(*(it + 1)) << "." <<
             +static_cast<unsigned char>(*(it + 2)) << "." << +static_cast<unsigned char>(*(it + 3));
        port << +static_cast<unsigned short>(static_cast<unsigned short>(*(it + 4)) * 256 + static_cast<unsigned char>(*(it + 5)));
        peers.emplace_back(host.str(), port.str());
    }

    return peers;
}
