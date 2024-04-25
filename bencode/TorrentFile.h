//
// Created by dzmitry on 20.02.24.
//

#ifndef BITTORRENT_TORRENTFILE_H
#define BITTORRENT_TORRENTFILE_H

#include <vector>
#include <string>

struct Announce {
    std::string protocol{};
    std::string hostname{};
    std::string port{};
    std::string query{};
};

struct File {
    std::string path{};
    size_t length;
};

struct Info {
    std::vector<std::string> pieces{};
    std::string name{};
    size_t piece_length;
    std::vector<File> files{};
};

struct TorrentFile {
    TorrentFile(const std::string &path);

    Info info{};
    std::string infoHash{};
    Announce announce{};
    std::vector<Announce> announceList{};
};

std::string sizeToString(size_t size);
void printInfo(TorrentFile &metadata);

#endif //BITTORRENT_TORRENTFILE_H
