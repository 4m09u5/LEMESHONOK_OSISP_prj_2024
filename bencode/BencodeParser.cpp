#include <fstream>
#include "BencodeParser.h"

void printBencode(BencodeValue value) {
    if(value.type == BencodeType::String) {
        if(value.rawValue.size() > 100) {
            std::cout << "<string[" << value.rawValue.size() << "]>";
        } else {
            std::cout << '"' << value.rawValue << '"';
        }
        return;
    }

    if(value.type == BencodeType::Integer) {
        std::cout << value.rawValue;
        return;
    }

    if(value.type == BencodeType::Dictionary) {
        std::cout << "{";
        for(auto& el : value.dictValue) {
            std::cout << '"' << el.first << "\" : ";
            printBencode(el.second);
            std::cout << ", ";
        }
        std::cout << "\b\b}";
    }

    if(value.type == BencodeType::List) {
        std::cout << "[";
        for(auto& el : value.listValue) {
            printBencode(el);
            std::cout << ", ";
        }
        std::cout << "\b\b]";
    }
}

