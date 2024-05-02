#include <iostream>
#include "torrent/Peer.h"
#include "bencode/TorrentFile.h"
#include "torrent/PeerManager.h"
#include "torrent/TorrentThreadPool.h"
#include <format>

int main(int argc, char **argv) {
    if (argc < 2 || argc > 4) {
        std::cout << std::format("Use {} help", argv[0]) << std::endl;
        return 0;
    }

    if (argc == 2) {
        if (std::string(argv[1]) == "help") {
            std::cout << "BitTorrent [command] [...]" << std::endl;
            std::cout << "\thelp - prints this message" << std::endl;
            std::cout << "\tinfo [path to torrent file] - read info from the torrent file" << std::endl;
            std::cout << "\tdownload [path to torrent file] [download directory] - download files to the provided directory" << std::endl;

            return 0;
        } else {
            std::cout << std::format("responseUse {} help", argv[0]) << std::endl;
            return 0;
        }
    }

    if (argc == 3) {
        if (std::string(argv[1]) == "info") {
            TorrentFile metadata(argv[2]);
            printInfo(metadata);
            return 0;
        } else {
            std::cout << std::format("Use {} help", argv[0]) << std::endl;
            return 0;
        }
    }

    if (argc == 4) {
        if (std::string(argv[1]) == "download") {
            ;
        } else {
            std::cout << "Use BitTorrent --help" << std::endl;
            return 0;
        }
    }

    TorrentFile metadata(argv[2]);
    PieceManager pieceManager(metadata, argv[3]);
    std::cout << "Scanning directory..." << std::endl;
    auto [missing, present] = pieceManager.getPieceData();

    TorrentThreadPool threadPool(16, metadata, pieceManager, missing, present);

    while (true) {
        auto statistics = threadPool.getInfo();
        auto done = threadPool.getDoneCount();
        auto total = pieceManager.getTotalPieces();
        auto percentage = (double)done / total * 100;

        system("clear");

        std::cout << std::format("Downloaded: {}/{} ({:.2f}%)\n", done, total, percentage);

        for (auto info : statistics) {
            auto up = sizeToString(info.uploaded);
            auto down = sizeToString(info.downloaded);
            std::cout << std::format("{}:{} - UP: {}; DOWN: {}\n", info.peer.getAddr(), info.peer.getPort(), up, down);
        }

        sleep(1);

        if (!threadPool.isWorking()) {
            system("clear");
            std::cout << "Download completed." << std::endl;
            break;
        }
    }
}