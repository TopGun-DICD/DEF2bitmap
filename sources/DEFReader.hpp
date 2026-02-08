#pragma once

#include <fstream>
#include <string>

#include "DEF.hpp"

class DEFReader {
    std::fstream    file;
    DEF            *def     = nullptr;
    uint32_t        lineNum = 0;
public:
    bool read(const std::string &_fileName, DEF &_def);
private:
    bool readDesign();
    bool readComponents();
    bool readPins();
    bool readSpecialNets();
    bool readNets();
    bool resolveNetPinCoords();
};