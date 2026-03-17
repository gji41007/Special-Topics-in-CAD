#include "netlist.h"

#include <fstream>
#include <sstream>
#include <regex>


std::string type2Str(gateType type) {
    switch(type) {
        case gateType::NAND: return "NAND";
        case gateType::NOR: return "NOR";
        case gateType::INV: return "INV";
        default: return "UNKNOWN";
    }
}
gateType str2Type(const std::string& type) {
    if (type.find("INV") != std::string::npos) return gateType::INV;
    else if (type.find("NOR") != std::string::npos) return gateType::NOR;
    else return gateType::NAND;
}

