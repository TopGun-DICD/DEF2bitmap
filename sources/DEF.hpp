#pragma once

#include <string>
#include <vector>

struct Point {
    uint32_t    x   = -1, 
                y   = -1;
};

struct Rect {
    Point       lb,
                rt;
};

struct  Component {
    std::string name,
                type;
    Point       position;
};

struct Leg {
    uint8_t     layer   = -1;
    Point       a,
                b;
};

class Net {
    std::string         name;
    std::vector<Leg *>  legs;
public:
   ~Net() {
        for (Leg *leg : legs)
            delete leg;
        legs.clear();
    }
public:
    void setName(std::string &_name) {
        name = _name;
    }
    std::string getName() {
        return name;
    }
    Leg* addLeg(uint8_t _layer, Point &_a, Point &_b) {
        legs.push_back(new Leg{_layer, _a, _b});
        return legs.back();
    }
    const std::vector<Leg *>& getLegs() const {
        return legs;
    }
};

class DEF {
    std::string                 designName;
    Rect                        dieArea;
    std::vector<Component *>    components;
    std::vector<Net *>          nets;
public:
   ~DEF() {
        for (Component *component : components)
            delete component;
        components.clear();
        for (Net *net : nets)
            delete net;
        nets.clear();
    }
public:
    void setDesignName(std::string& _name) {
        designName = _name;
    }
    void setArea(Rect &_area) {
        dieArea = _area;
    }
    void addComponent(Component *_component) {
        if (!_component)
            return;
        components.push_back(_component);
    }
    void addNet(Net *_net) {
        if (!_net)
            return;
        nets.push_back(_net);
    }
    const Rect&                     getArea()       const   { return dieArea; }
    const std::vector<Component *>  getComponents() const   { return components; }
    const std::vector<Net *>        getNets()       const   { return nets; }
};
