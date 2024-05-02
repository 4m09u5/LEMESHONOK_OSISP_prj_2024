#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

enum class BencodeType {
    Integer,
    String,
    List,
    Dictionary
};

struct BencodeValue {
    BencodeType type;
    std::string rawValue;
    std::vector<BencodeValue> listValue;
    std::unordered_map<std::string, BencodeValue> dictValue;

    BencodeValue operator[](std::string key);
    BencodeValue operator[](size_t index);
};

class BencodeParser {
    int currentIndex;

    BencodeValue parseValue(const std::string& input);
    BencodeValue parseInteger(const std::string& input);
    BencodeValue parseString(const std::string& input);
    BencodeValue parseList(const std::string& input);
    BencodeValue parseDictionary(const std::string& input);
public:
    BencodeValue parse(const std::string& input);
};

void printBencode(BencodeValue value);