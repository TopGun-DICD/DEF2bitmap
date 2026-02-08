#include <iostream>

#include "DEF.hpp"
#include "DEFReader.hpp"
#include "DEFConverter.hpp"

int main(int argc, char *argv[]) {
    DEF             def;
    DEFReader       readerDEF;
    DEFConverter    converter;

    /* // Majority of 5
    ConverterOptions options;
    options.bmpFileName         = "majorityof5.bmp";
    options.txtFileName         = "majorityof5.txt";
    options.writeText           = false;
    options.writeBitmap         = true;
    options.excludeNets         = {};
    options.layersToWorkWith    = {1, 2, 3, 4, 5};
    std::string defFile         = "tests/majorityof5/majorityof5.def";
    //*/
    
    //* // Counter
    ConverterOptions options;
    options.bmpFileName         = "counter.bmp";
    options.txtFileName         = "counter.txt";
    options.writeText           = false;
    options.writeBitmap         = true;
    options.excludeNets         = {};
    options.layersToWorkWith    = {1, 2, 3};
    std::string defFile         = "tests/counter/counter.def";
    //*/

    /* // SPM
    ConverterOptions options;
    options.bmpFileName         = "spm.bmp";
    options.txtFileName         = "spm.txt";
    options.writeText           = false;
    options.writeBitmap         = true;
    options.excludeNets         = {};
    options.layersToWorkWith    = {1, 2, 3, 4};
    std::string defFile         = "tests/spm/spm.def";
    //*/


    if (!readerDEF.read(defFile, def))
        return EXIT_FAILURE;
    if (!converter.writeTo(def, options))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}