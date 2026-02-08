#include "DEFReader.hpp"

#include <sstream>
#include <iostream>
#include <chrono>

bool DEFReader::read(const std::string &_fileName, DEF &_def) {
    std::cout << "Parsing input file " << _fileName << "...\n";
    
    auto start1 = std::chrono::steady_clock::now();

    file.open(_fileName, std::ios::binary | std::ios::in);
    if (!file.is_open())
        return false;
    
    def = &_def;

    std::cout << "  Reading file contents" << _fileName << "...\n";
    std::string line;
    while (std::getline(file, line)) {
        ++lineNum;
        std::stringstream stream(line);
        std::string firstToken;
        stream >> firstToken;
        if (firstToken == "DESIGN") {
            bool retCode = readDesign();
            if (!retCode) {
                file.close();
                return false;
            }
            continue;
        }
        if (firstToken == "COMPONENTS") {
            bool retCode = readComponents();
            if (!retCode) {
                file.close();
                return false;
            }
            continue;
        }
        if (firstToken == "PINS") {
            bool retCode = readPins();
            if (!retCode) {
                file.close();
                return false;
            }
            continue;
        }
        if (firstToken == "SPECIALNETS") {
            bool retCode = readSpecialNets();
            if (!retCode) {
                file.close();
                return false;
            }
            continue;
        }
        if (firstToken == "NETS") {
            bool retCode = readNets();
            if (!retCode) {
                file.close();
                return false;
            }
            continue;
        }
    }
    file.close();
    auto stop1 = std::chrono::steady_clock::now();
    auto duration1mins  = std::chrono::duration_cast<std::chrono::minutes>(stop1 - start1);
    auto duration1secs  = std::chrono::duration_cast<std::chrono::seconds>(stop1 - start1);
    auto duration1msecs = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);

    std::cout   << "  Reading file contents done in " 
                << duration1mins.count() << " mins " 
                << duration1secs.count() << " secs"
                << duration1msecs.count() << " millis" << std::endl;

    auto start2 = std::chrono::steady_clock::now();


    std::cout << "  Resolving pin coords...\n";

    bool retCode = resolveNetPinCoords();
    if (!retCode)
        return false;

    auto stop2 = std::chrono::steady_clock::now();
    auto duration2mins = std::chrono::duration_cast<std::chrono::minutes>(stop2 - start2);
    auto duration2secs = std::chrono::duration_cast<std::chrono::seconds>(stop2 - start2);
    auto duration2msecs = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);

    std::cout   << "  Resolving pin coords done in "
                << duration1mins.count() << " mins "
                << duration1secs.count() << " secs "
                << duration1msecs.count() << " millis" << std::endl;

    auto duration3mins = std::chrono::duration_cast<std::chrono::minutes>(stop2 - start1);
    auto duration3secs = std::chrono::duration_cast<std::chrono::seconds>(stop2 - start1);
    auto duration3msecs = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start1);

    std::cout   << "Parsing of the input completed in "
                << duration1mins.count() << " mins "
                << duration1secs.count() << " secs "
                << duration1msecs.count() << " millis" << std::endl;
    return true;
}

bool DEFReader::readDesign() {
    std::string line;
    std::streampos pos = file.tellg();
    while (std::getline(file, line)) {
        ++lineNum;
        std::stringstream   stream(line);
        std::string         firstToken;
        stream >> firstToken;
        if (firstToken == "DIEAREA") {
            char bracket = '?';
            Rect rect;
            stream >> bracket >> rect.lb.x >> rect.lb.y >> bracket >> bracket >> rect.rt.x >> rect.rt.y;
            def->setArea(rect);
            pos = file.tellg();
            continue;
        }
        if (firstToken == "COMPONENTS") {
            file.seekg(pos);
            return true;
        }
        pos = file.tellg();
    }
    return true;
}

bool DEFReader::readComponents() {
    std::string line;
    std::string firstToken,
                name, 
                type;
    Component  *component = nullptr;
    while (std::getline(file, line)) {
        ++lineNum;
        std::stringstream   stream(line);
        stream >> firstToken;
        if (firstToken == "END")
            return true;
        if (firstToken != "-") {
            std::cerr << "__err__ (" << lineNum << ") : unsupported first token '" << firstToken << "' in section COMPONENTS";
            return false;
        }
        component = new Component;
        stream >> component->name >> component->type >> firstToken >> firstToken;
        if (firstToken != "PLACED")
            stream >> firstToken >> firstToken >> firstToken >> firstToken;
        else
            stream >> firstToken;
        if (firstToken != "(") {
            std::cerr << "__err__ (" << lineNum << ") : an open bracket expected in section COMPONENTS, but '" << firstToken << "' was met";
            return false;
        }
        stream >> component->position.x >> component->position.y;
        def->addComponent(component);
    }
    return true;
}

bool DEFReader::readPins() {
    std::string line;
    std::string firstToken;
    while (std::getline(file, line)) {
        ++lineNum;
        std::stringstream   stream(line);
        stream >> firstToken;
        if (firstToken == "END")
            return true;
    }
    return true;
}

bool DEFReader::readSpecialNets() {
    std::string line;
    std::string firstToken,
                name,
                type;
    Net    *net = nullptr;
    Leg    *leg = nullptr;
    Point   a,
            b;
    uint8_t layer = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        std::stringstream   stream(line);
        stream >> firstToken;
        if (firstToken == "END")
            return true;
        /*
        if (firstToken == "-") {
            net = new Net;
            stream >> name;
            net->setName(name);
            def->addNet(net);
            continue;
        }
        if (firstToken == "+") {
            stream >> firstToken;
            if (firstToken != "ROUTED") {
                std::cerr << "__err__ (" << lineNum << ") : ROUTED expected to be the 2nd token in SPECIALNETS section, but '" << firstToken << "' was met";
                return false;
            }
        }
        else
            if (firstToken != "NEW") {
                std::cerr << "__err__ (" << lineNum << ") : NEW expected to be the 1st token in SPECIALNETS section, but '" << firstToken << "' was met";
                return false;
            }

        stream >> name;
        if (isdigit(name.back()))
            layer = name.back() - '0';
        stream >> firstToken >> firstToken >> firstToken >> firstToken >> firstToken;
        if (firstToken != "(") {
            std::cerr << "__err__ (" << lineNum << ") : an open bracket expected in section SPECIALNETS, but '" << firstToken << "' was met";
            return false;
        }
        stream >> a.x >> a.y;
        stream >> firstToken;
        if (firstToken != ")") {
            std::cerr << "__err__ (" << lineNum << ") : a close bracket expected in section SPECIALNETS, but '" << firstToken << "' was met";
            return false;
        }
        stream >> firstToken;
        if (firstToken != "(")
            continue;
        stream >> b.x >> b.y;
        net->addLeg(layer, a, b);
        //*/
    }
    return true;
}

bool DEFReader::readNets() {
    std::string line;
    std::string firstToken,
                name,
                type;
    Net    *net = nullptr;
    Leg    *leg = nullptr;
    std::string pointx,
                pointy;
    Point   a,
            b;
    uint8_t layer = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        std::stringstream   stream(line);
        stream >> firstToken;
        if (firstToken == "END")
            return true;
        if (firstToken == "(")
            continue;
        if (firstToken == "-") {
            net = new Net;
            def->addNet(net);
            stream >> name;
            net->setName(name);
            stream >> firstToken;
            while (firstToken != "+") {
                if (stream.eof()) {
                    std::getline(file, line);
                    ++lineNum;
                    stream.str(line);
                    stream >> firstToken;
                }
                if (firstToken != "(") {
                    std::cerr << "__err__ (" << lineNum << ") : '(' or '+' expected while parsing net '" << net->getName() << "'";
                    return false;
                }
                stream >> name;         // the cell name
                stream >> firstToken;   // the port name of the cell
                if (name != "PIN")
                    net->addPin(name, firstToken);
                stream >> firstToken;   // )
                stream >> firstToken;   // ( | +
            };
            continue;
        }

        if (firstToken == "+") {
            stream >> firstToken;
            if (firstToken != "ROUTED") {
                std::cerr << "__err__ (" << lineNum << ") : ROUTED expected to be the 2nd token in NETS section, but '" << firstToken << "' was met";
                return false;
            }
        }
        else
            if (firstToken != "NEW") {
                std::cerr << "__err__ (" << lineNum << ") : NEW expected to be the 1st token in NETS section, but '" << firstToken << "' was met";
                return false;
            }

        stream >> name;
        if (name[0] == 'l') // NEW li1 ( 31970 44370 ) L1M1_PR_MR
            continue;
        if (isdigit(name.back()))
            layer = name.back() - '0';
        stream >> firstToken;
        if (firstToken != "(") {
            std::cerr << "__ err__ (" << lineNum << ") : an open bracket expected in section NETS, but '" << firstToken << "' was met";
            return false;
        }
        stream >> a.x >> a.y;
        stream >> firstToken;
        if (firstToken != ")")
            if (firstToken == "0")
                stream >> firstToken;
            else {
                std::cerr << "__err__ (" << lineNum << ") : a close bracket expected in section SPECIALNETS, but '" << firstToken << "' was met";
                return false;
            }
        stream >> firstToken;
        if (firstToken != "(")
            continue;
        stream >> pointx >> pointy;
        if (pointx == "*")
            b.x = a.x;
        else
            b.x = atoi(pointx.c_str());
        if (pointy == "*")
            b.y = a.y;
        else
            b.y = atoi(pointy.c_str());
        net->addLeg(layer, a, b);
    }
    return true;
}

bool DEFReader::resolveNetPinCoords() {
    std::vector<Net *> defNets = def->getNets();
    std::vector<Component *> defComponents = def->getComponents();
    bool netNameResolved = false;
    for (Net *net : defNets) {
        std::vector<NetPin *> netPins = net->getPins();
        for (NetPin *netPin : netPins) {
            netNameResolved = false;
            for (Component *component : defComponents) {
                if (netPin->cellName != component->name)
                    continue;
                netPin->pinCoord = component->position;
                netNameResolved = true;
            }
            if (!netNameResolved) {
                std::cerr << "__err__ : net " << net->getName() << ", pin " << netPin->cellName << ":" << netPin->pinName << " was not resolved\n";
                return false;
            }
        }
    }
    return true;
}
