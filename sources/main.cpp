#include <iostream>

#include "DEF.hpp"
#include "DEFReader.hpp"
#include "DEFConverter.hpp"

int main(int argc, char *argv[]) {
    DEF             def;
    DEFReader       readerDEF;
    DEFConverter    converter;

    /* // Majority of 5
    std::string     defFile = "tests/majorityof5/majorityof5.def";
    std::string     txtFile = "result_majorityof5.txt";
    //*/
    
    //* // Counter
    std::string     defFile = "tests/counter/counter.def";
    std::string     txtFile = "result_counter.txt";
    //*/

    /* // SPM
    std::string     defFile = "tests/spm/spm.def";
    std::string     txtFile = "result_spm.txt";
    //*/


    if (!readerDEF.read(defFile, def))
        return EXIT_FAILURE;
    if (!converter.writeTo(txtFile, def, 1))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}