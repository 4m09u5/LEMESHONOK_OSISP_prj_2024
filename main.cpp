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
#include "network/udp.h"
#include "torrent/UDPPeerManager.h"

#include <format>

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
    std::queue<PieceData> tasks;
    std::vector<PieceData> pending;    //TODO pending
    std::vector<PieceData> done;
    std::queue<Peer> peers;
    std::vector<std::thread> threads;
    std::mutex mutex;
public:
    explicit ThreadPool(int workerCount, TorrentFile metadata, PieceManager& pieceManager) {
        for (int i = 0; i < workerCount; ++i) {
            threads.emplace_back([this, &pieceManager, metadata](){
                while(working) {
                    Peer peer("", "");
                    {
                        std::lock_guard guard(mutex);
                        if (peers.empty()) continue;
                        peer = peers.front();
                        peers.pop();
                    }

                    PeerConnection connection(peer.getAddr(), peer.getPort());

                    if (!connection.connect())
                        continue;

                    std::cout << "Connected to " << peer.getAddr() << ":" << peer.getPort() << std::endl;

                    PeerManager peerManager(connection, nullptr, &pieceManager, metadata, "                    ");

                    if(!peerManager.performHandshake())
                        continue;

                    while (working) {
                        PieceData task;
                        {
                            std::lock_guard guard(mutex);
                            if (tasks.empty()) {
                                continue;
                            }
                            task = tasks.front();
                            tasks.pop();
                        }
                        bool successful;

                        for (int i = 0; i < 3; i++) {
                            successful = peerManager.downloadPiece(task);
                            if (successful)
                                break;
                            std::cout << "Failed piece " << task.index << " (" << i << ")." << std::endl;
                        }

                        if (!successful) {
                            std::lock_guard guard(mutex);
                            tasks.push(task);
                            break;
                        } else {
                            std::lock_guard guard(mutex);
                            done.push_back(task);
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

    void download(PieceData piece) {
        std::lock_guard guard(mutex);
        tasks.push(piece);
    }

    void addPeer(Peer peer) {
        std::lock_guard guard(mutex);
        peers.push(peer);
    }

    size_t getDoneCount() {
        std::lock_guard guard(mutex);
        return done.size();
    }
};

std::string sizeToString(size_t size) {
    std::vector<std::string> sizes = {"B", "kB", "MB", "GB", "TB"};

    double result = size;
    int i = 0;

    for (; result >= 1024 && i < sizes.size() - 1; i++)
        result /= 1024;

    return std::format("{:.2f} {}", result, sizes.at(i));
}

void printInfo(TorrentFile &metadata) {
    std::cout << "Label: " << metadata.info.name << std::endl;
    std::cout << "Files: " << std::endl;

    for (auto file : metadata.info.files) {
        std::cout << std::format("\t{} ({})\n", file.path, sizeToString(file.length));
    }

    std::cout << std::endl;
    std::cout << "Announce list: " << std::endl;

    for (auto announce : metadata.announceList) {
        std::cout << std::format("\t{}://{}:{}{}\n", announce.protocol, announce.hostname, announce.port, announce.query);
    }

    std::cout << std::endl;
    std::cout << std::format("Piece length: {} ({})\n", metadata.info.piece_length, sizeToString(metadata.info.piece_length));
    std::cout << "Info hash: " << metadata.infoHash << std::endl;
}

int main(int argc, char **argv) {
    if (argc == 3) {
        if (std::string(argv[1]) == "info") {
            TorrentFile metadata(argv[2]);
            printInfo(metadata);
            return 0;
        }
    }

    TorrentFile metadata("KFP.torrent");

    std::string hash = URLEncode(metadata.infoHash);

/*
    HTTP request(metadata.announce.hostname, metadata.announce.port, metadata.announce.query);
    request.addParameter("info_hash", hash);
    request.addParameter("peer_id", "%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10");
    request.addParameter("port", "44953");
    request.addParameter("uploaded", "0");
    request.addParameter("downloaded", "0");
    request.addParameter("left", "0");

    BencodeParser parser;
    auto response = request.execute();
    std::cout << response.getData();


    auto data = parser.parse(response.getData());
    auto peers = parsePeers(data["peers"].rawValue);*/

    UDPPeerManager pm(metadata.announce.hostname, metadata.announce.port);

    std::vector<Peer> peers;

    try {
        pm.connect();
        peers = pm.getPeers(metadata.infoHash);
    } catch (std::runtime_error &e) {
        std::cout << "UDP failed: " << e.what() << std::endl;

        HTTP request(metadata.announce.hostname, metadata.announce.port, metadata.announce.query);
        request.addParameter("info_hash", hash);
        request.addParameter("peer_id", "%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10%10");
        request.addParameter("port", "44953");
        request.addParameter("uploaded", "0");
        request.addParameter("downloaded", "0");
        request.addParameter("left", "0");

        BencodeParser parser;
        auto response = request.execute();
        std::cout << response.getData();


        auto data = parser.parse(response.getData());
        peers = parsePeers(data["peers"].rawValue);
    }
    std::cout << std::endl;

    for(auto &peer : peers) {
        std::cout << peer.getAddr() << ":" << peer.getPort() << std::endl;
    }

    std::vector<std::string> files;
    std::vector<size_t> sizes;

    for(auto el : metadata.info.files) {
        sizes.push_back(el.length);
        files.push_back(el.path);
    }

    PieceManager pieceManager(metadata, "/home/dzmitry/Desktop/bibubi/");

    ThreadPool threadPool(8, metadata, pieceManager);

    for(const auto& peer : peers) {
        threadPool.addPeer(peer);
    }

    std::cout << "Total pieces: " << pieceManager.getTotalPieces() << std::endl;

    auto pieces = pieceManager.generatePieces();

    for(int i = 0; i < 20; i++) {
        threadPool.download(pieces.at(i));
    }

    std::cout << "Im sleeping" << std::endl;
    while (true) {
        sleep(1);
        std::cout << std::format("{} / {}\n", threadPool.getDoneCount(), pieces.size());
    }
}