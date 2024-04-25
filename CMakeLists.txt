cmake_minimum_required(VERSION 3.27)
project(BitTorrent)

set(CMAKE_CXX_STANDARD 20)

add_executable(BitTorrent main.cpp
        bencode/BencodeParser.cpp
        bencode/BencodeParser.h
        utils/sha1.cpp
        utils/sha1.h
        torrent/TorrentClient.cpp
        torrent/TorrentClient.h
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
        Client.cpp
        Client.h
        bencode/TorrentFile.cpp
        bencode/TorrentFile.h
        network/Response.cpp
        network/Response.h
        network/tcp.cpp
        network/tcp.h
        utils/SharedQueue.h
        torrent/PeerManager.cpp
        torrent/PeerManager.h
        torrent/PieceManager.cpp
        torrent/PieceManager.h
        network/udp.cpp
        network/udp.h
        torrent/UDPPeerManager.cpp
        torrent/UDPPeerManager.h
        torrent/PieceData.cpp
        torrent/PieceData.h
        torrent/TorrentThreadPool.cpp
        torrent/TorrentThreadPool.h
        torrent/PeerInfo.cpp
        torrent/PeerInfo.h
)
