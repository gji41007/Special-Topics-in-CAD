#ifndef NETLIST_H
#define NETLIST_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

enum class gateType { NAND, NOR, INV };
gateType str2Type(const std::string& typeStr);
std::string type2Str(gateType type);

class Gate {
public:
    Gate(gateType t, std::string n, std::string in, std::string out){
        type = t;
        name = n;;
        input1 = in; 
        output = out;
    }
    Gate(gateType t, std::string n, std::string in1, std::string in2, std::string out){
        type = t;
        name = n;;
        input1 = in1; 
        input2 = in2; 
        output = out;
    }
    gateType type;
    std::string name;
    // std::vector<std::string> inputs;
    std::string input1;
    std::string input2;
    std::string output;
    // double output_load;
    // double input_transition_time;
    // double propagation_delay;
    // double output_transition_time;
};

class TopModule {
public:
    
};

// class Graph{
// public:
//     std::vector<Gate*> gates;

// }

#endif