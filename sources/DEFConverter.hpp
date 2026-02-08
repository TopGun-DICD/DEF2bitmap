#pragma once

#include <fstream>
#include <string>

#include "DEF.hpp"

struct ConverterOptions {
    std::string                 txtFileName,            //< Имя выходного текстового файла
                                bmpFileName;            //< Имя выходного графического файла
    bool                        writeText       = true, //< Нужно генерировать текст?
                                writeBitmap     = true; //< Нужно генерировать картинку?
    std::vector<std::string>    excludeNets;            //< Вектор цепей, которые нужно исключить из записи
    std::vector<uint8_t>        layersToWorkWith;       //< Печень слоёв, на которых происходит обарботка
};

class DEFConverter {
    std::fstream        file;
    DEF                *def     = nullptr;
    uint32_t            gcdX    = 10,
                        gcdY    = 10;
    uint32_t            rows    = 0,
                        cols    = 0;
    uint8_t           **matrix  = nullptr;
    ConverterOptions   *options = nullptr;
public:
    bool writeTo(DEF &_def, ConverterOptions &_options);
private:
    //void findGCDs(uint8_t _layer);

    void initMatrix();
    void uninitMatrix();
};
