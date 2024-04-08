#include <iostream>
#include <fstream>
#include "bencode/BencodeParser.h"
#include <sstream>
#include <functional>
#include <thread>
#include "utils/sha1.h"
#include "network/http.h"
#include "torrent/Peer.h"
#include "bencode/TorrentFile.h"
#include "network/PeerConnection.h"
#include "utils/sha1.h"
#include "torrent/PeerManager.h"

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

class ThreadPool {
    bool working = true;
    std::queue<size_t> tasks;
    std::queue<Peer> peers;
    std::vector<std::thread> threads;
    std::mutex mutex;
public:
    explicit ThreadPool(int workerCount, TorrentFile metadata, PieceManager pieceManager) {
        for (int i = 0; i < workerCount; ++i) {
            threads.emplace_back([this, &pieceManager, &metadata](){

                while(working) {
                    Peer peer("", "");
                    {
                        std::lock_guard guard(mutex);
                        if (peers.empty()) continue;
                        peer = peers.front();
                        peers.pop();
                    }

                    PeerConnection connection(peer.getAddr(), peer.getPort());

                    if (!connection.connect()) {
                        std::cout << peer.getAddr() << ":" << peer.getPort() << " fucked up!" << std::endl;
                        continue;
                    }

                    std::cout << "Connected!" << std::endl;

                    PeerManager peerManager(connection, nullptr, pieceManager, metadata, "                    ");

                    while (working) {
                        size_t task;
                        {
                            std::lock_guard guard(mutex);
                            if (tasks.empty()) continue;
                            task = tasks.front();
                            tasks.pop();
                        }
                        if(!peerManager.downloadByPieceId(task)) {
                            std::lock_guard guard(mutex);
                            tasks.push(task);
                        }
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        working = false;
        for (auto &t : threads) t.join();
    }

    void download(size_t task) {
        std::lock_guard guard(mutex);
        tasks.push(task);
    }

    void addPeer(Peer peer) {
        std::lock_guard guard(mutex);
        peers.push(peer);
    }
};

int main() {
    auto parser = BencodeParser();

    TorrentFile metadata("example.torrent");

    size_t blockSize = 0x4000;

    std::cout << "Piece length: " << metadata.info.piece_length << std::endl;
    std::cout << "Desired block size: " << blockSize << std::endl;
    std::cout << "Blocks per piece: " << metadata.info.piece_length / blockSize << std::endl;
    std::cout << "Info hash: " << metadata.infoHash << std::endl;

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

    std::vector<std::string> files;
    std::vector<size_t> sizes;

    for(auto el : metadata.info.files) {
        sizes.push_back(el.length);
        files.push_back(el.path);
    }

    PieceManager pieceManager(metadata, "/home/dzmitry/Desktop/download/");

    ThreadPool threadPool(8, metadata, pieceManager);

    for(const auto& peer : peers) {
        threadPool.addPeer(peer);
    }

    for(int i = 0; i < 100; i++) {
        threadPool.download(i);
    }

    std::cout << "Im sleeping" << std::endl;
    sleep(100000);


    std::cout << "aboba";
}