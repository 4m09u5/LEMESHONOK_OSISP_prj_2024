cmake_minimum_required(VERSION 3.27)
project(BitTorrent)

set(CMAKE_CXX_STANDARD 20)

add_executable(BitTorrent main.cpp
        bencode/BencodeParser.cpp
        bencode/BencodeParser.h
        utils/sha1.cpp
        utils/sha1.h
        network/http.cpp
        network/http.h
        network/HTTPResponse.cpp
        network/HTTPResponse.h
        torrent/Peer.cpp
        torrent/Peer.h
        network/tcp.cpp
        network/tcp.h
        network/PeerConnection.cpp
        network/PeerConnection.h
        network/message.cpp
        network/message.h
        bencode/TorrentFile.cpp
        bencode/TorrentFile.h
        network/tcp.cpp
        network/tcp.h
        torrent/PeerManager.cpp
        torrent/PeerManager.h
        torrent/PieceManager.cpp
        torrent/PieceManager.h
        network/udp.cpp
        network/udp.h
        torrent/Tracker/UDPTracker/UDPTracker.cpp
        torrent/Tracker/UDPTracker/UDPTracker.h
        torrent/PieceData.h
        torrent/TorrentThreadPool.cpp
        torrent/TorrentThreadPool.h
        torrent/PeerInfo.h
        torrent/Tracker/HTTPTracker/HTTPTracker.cpp
        torrent/Tracker/HTTPTracker/HTTPTracker.h
        torrent/Tracker/Tracker.h
)
