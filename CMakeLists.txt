cmake_minimum_required(VERSION 3.27)
project(BitTorrent)

set(CMAKE_CXX_STANDARD 17)

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
        bencode/TorrentFileParser.cpp
        bencode/TorrentFileParser.h)
