#include "DEFConverter.hpp"

#include <iostream>
#include <chrono>

//https://github.com/baderouaich/BitmapPlusPlus
#include "BitmapPlusPlus.hpp"

bool DEFConverter::writeTo(DEF &_def, ConverterOptions &_options) {
    std::cout << "Starting DEF data conversion..." << std::endl;

    options = &_options;
    def     = &_def;
    
    auto start1 = std::chrono::steady_clock::now();

    std::cout << "  Initializing the bit matrix" << std::endl;
    initMatrix();

    if (options->writeBitmap && !options->bmpFileName.empty()) {
        std::cout << "  Dumping nets to the output bitmap file" << std::endl;
        bmp::Bitmap image(cols, rows);
        image.clear(bmp::Pixel(255, 255, 255));
        for (uint32_t row = 0; row < rows; ++row) {
            for (uint32_t col = 0; col < cols; ++col) {
                if (matrix[col][rows - row - 1] == 1)
                    image.set(col, row, bmp::Black);
            }
        }
        image.save(options->bmpFileName);
        std::cout << "  Done!" << std::endl;
    }

    if (options->writeText && !options->txtFileName.empty()) {
        std::cout << "  Dumping nets to the output text file" << std::endl;
        file.open(options->txtFileName, std::ios::out);
        if (!file.is_open())
            return false;
        //*
        file << "ROWS " << rows << " COLS " << cols << "\n";
        for (uint32_t row = 0; row < rows; ++row) {
            for (uint32_t col = 0; col < cols; ++col) {
                file << (int)matrix[col][rows - row - 1]; // << ' ';
            }
            file << "\n";
        }
        file.close();
        //*/
        std::cout << "  Done!" << std::endl;
    }

    std::cout << "  Deinitializing the bit matrix" << std::endl;
    uninitMatrix();
    std::cout << "  Done!" << std::endl;

    std::cout << "DEF data conversion has finished." << std::endl;
    return true;
}

// URL: https://pro-prof.com/forums/topic/gcd_lcm_cplusplus
uint32_t greatest_common_divisor(uint32_t _a, uint32_t _b) {
    if (_a % _b == 0)
        return _b;
    if (_b % _a == 0)
        return _a;
    if (_a > _b)
        return greatest_common_divisor(_a % _b, _b);
    return greatest_common_divisor(_a, _b % _a);
}

/*
void DEFConverter::findGCDs(uint8_t _layer) {
    const std::vector<Net *> nets = def->getNets();

    uint32_t a = 0;
    uint32_t gcd = 0;
    
    // X
    a = gcdX = nets[0]->getLegs()[0]->a.x;

    for (int i = 0; i < nets.size(); ++i) {
        const std::vector<Leg *> legs = nets[i]->getLegs();
        for (int j = 0; j < legs.size(); ++j) {
            if (_layer && legs[j]->layer != _layer)
                continue;
            uint32_t b = legs[j]->a.x;
            gcd = greatest_common_divisor(a, b);
            if (gcd < gcdX)
                gcdX = gcd;
        }
    }

    // Y
    a = gcdY = nets[0]->getLegs()[0]->a.y;

    for (int i = 0; i < nets.size(); ++i) {
        const std::vector<Leg*> legs = nets[i]->getLegs();
        for (int j = 0; j < legs.size(); ++j) {
            if (_layer && legs[j]->layer != _layer)
                continue;
            uint32_t b = legs[j]->a.y;
            gcd = greatest_common_divisor(a, b);
            if (gcd < gcdY)
                gcdY = gcd;
        }
    }
}
//*/

void DEFConverter::initMatrix() {
    rows = (def->getArea().rt.y - def->getArea().lb.y) / gcdY;
    cols = (def->getArea().rt.x - def->getArea().lb.x) / gcdX;

    matrix = new uint8_t *[cols];
    for (uint32_t col = 0; col < cols; ++col) {
        matrix[col] = new uint8_t[rows];
        for (uint32_t row = 0; row < rows; ++row)
            matrix[col][row] = 0;
    }


    const std::vector<Net*> nets = def->getNets();

    for (size_t i = 0; i < nets.size(); ++i) {
        const std::vector<Leg*> legs = nets[i]->getLegs();
        for (size_t j = 0; j < legs.size(); ++j) {
            bool layerIsAllowed = false;
            for (auto layer : options->layersToWorkWith) {
                if (legs[j]->layer != layer)
                    continue;
                layerIsAllowed = true;
                break;
            }
            if (!layerIsAllowed)
                continue;
            //*
            if (legs[j]->a.x == legs[j]->b.x) {
                uint32_t constX = legs[j]->b.x / gcdX;
                if (legs[j]->a.y < legs[j]->b.y) {
                    uint32_t runFrom    = legs[j]->a.y / gcdY;
                    uint32_t runTo      = legs[j]->b.y / gcdY;
                    for (uint32_t k = runFrom; k <= runTo; ++k)
                        matrix[constX][k] = 1;
                }
                if (legs[j]->a.y > legs[j]->b.y) {
                    uint32_t runFrom    = legs[j]->b.y / gcdY;
                    uint32_t runTo      = legs[j]->a.y / gcdY;
                    for (uint32_t k = runFrom; k <= runTo; ++k)
                        matrix[constX][k] = 1;
                }
                continue;
            }
            //*/
            //*
            if (legs[j]->a.y == legs[j]->b.y) {
                uint32_t constY = legs[j]->b.y / gcdY;
                if (legs[j]->a.x < legs[j]->b.x) {
                    uint32_t runFrom = legs[j]->a.x / gcdX;
                    uint32_t runTo = legs[j]->b.x / gcdX;
                    for (uint32_t k = runFrom; k <= runTo; ++k)
                        matrix[k][constY] = 1;
                }
                if (legs[j]->a.x > legs[j]->b.x) {
                    uint32_t runFrom = legs[j]->b.x / gcdX;
                    uint32_t runTo = legs[j]->a.x / gcdX;
                    for (uint32_t k = runFrom; k <= runTo; ++k)
                        matrix[k][constY] = 1;
                }
                continue;
            }
            //*/
            std::cerr << "__err__ : net " << nets[i]->getName() << ", leg #" << j << " is neither vertical nor horizontal\n";
        }
    }

}

void DEFConverter::uninitMatrix() {
    if (!matrix)
        return;

    for (uint32_t col = 0; col < cols; ++col)
        delete [] matrix[col];

    delete [] matrix;
}