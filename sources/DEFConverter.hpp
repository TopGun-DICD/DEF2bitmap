#pragma once

#include <fstream>
#include <string>

#include "DEF.hpp"

class DEFConverter {
    std::fstream    file;
    DEF            *def     = nullptr;
    uint32_t        gcdX    = 10,
                    gcdY    = 10;
    uint32_t        rows    = 0,
                    cols    = 0;
    uint8_t       **matrix  = nullptr;
public:
    bool writeTo(const std::string &_fileName, DEF &_def, uint8_t _atLayer);
private:
    void findGCDs(uint8_t _layer);
    void initMatrix(uint8_t _layer = 0);
    void uninitMatrix();
};