//
// Created by dzmitry on 20.02.24.
//

#include <regex>
#include <fstream>
#include <format>
#include "TorrentFile.h"
#include "BencodeParser.h"
#include "../utils/sha1.h"

Announce parseAnnounce(std::string raw) {
    Announce announce;

    std::regex regex("(.+):\\/\\/([a-zA-Z0-9.-]+):?(\\d+)?([a-zA-Z0-9.\\/]+)?");
    auto match = *std::sregex_iterator(raw.begin(), raw.end(), regex);

    announce.protocol = match[1];
    announce.hostname = match[2];
    announce.port = match[3].matched ? match[3] : std::string("80");
    announce.query = match[4].matched ? match[4] : std::string("");

    return announce;
}


TorrentFile::TorrentFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::stringstream ss;

    if (!file.is_open())
        throw std::runtime_error("Couldn't open file " + path);

    ss << file.rdbuf() << std::flush;
    file.close();

    BencodeParser parser;
    auto raw = parser.parse(ss.str());

    info.name = raw["info"]["name"].rawValue;
    info.piece_length = stoi(raw["info"]["piece length"].rawValue);

    SHA1 hash;
    hash.update(raw["info"].rawValue);
    infoHash = hash.final();

    announce = parseAnnounce(raw["announce"].rawValue);

    const auto& pieces = raw["info"]["pieces"].rawValue;
    for(int i = 0; i < pieces.size(); i += 20)
        info.pieces.emplace_back(pieces.substr(i, 20));

    const auto& files = raw["info"]["files"].listValue;
    for (auto file: files) {
        std::string path;

        for (auto el : file["path"].listValue)
            path += "/" + std::string(el.rawValue);

        info.files.push_back({path, std::stoul(file["length"].rawValue)});
    }

    for(const auto & it : raw["announce-list"][0].listValue) {
        announceList.emplace_back(parseAnnounce(it.rawValue));
    }
}

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