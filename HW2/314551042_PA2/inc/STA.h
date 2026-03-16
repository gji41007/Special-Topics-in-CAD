#ifndef STA_H
#define STA_H

#include "netlist.h"
#include "graph.h"

#include <unordered_map>
class Graph;
class Pin{
public:
    std::string pinname;
    bool isOutput;
    double capacitance;
};
class GateInfo{
public:
    gateType type;
    Pin pins[3];
    std::unordered_map<std::string, int> pinId;
};
class ValTable{
public:
    ValTable(){};
    ValTable(std::vector<double> _index1, std::vector<double> _index2){
        index1 = _index1;
        index2 = _index2;
    }
    std::vector<double> index1, index2;
    std::vector<std::vector<double>> valtable;
    double lookUp(Cell* c);
    void dumpTable(){
        std::cout<<valtable.size()<<std::endl;//<<" "<<valtable[1][1]<<std::endl;
        for(int i = 0; i < (int)valtable.size(); ++i){
            for(int j = 0; j < (int)valtable[0].size(); ++j){
                std::cout<<valtable[i][j]<<" ";
            }
            std::cout<<std::endl;
        }
    }

};
class LUT{
public:
    //Gate info
    GateInfo gateinfo[3];
    //Table info
    std::unordered_map<std::string, int> indexMap;
    std::vector<double> index1[5], index2[5];
    // std::vector<std::vector<double>> rpower[3], fpower[3], rtime[3], ftime[3], rtrans[3], ftrans[3];
    ValTable rpower[3], fpower[3], rtime[3], ftime[3], rtrans[3], ftrans[3];

    std::vector<std::vector<bool>> patterns;
    // double interpolationl(double x, double y)
};
class STA{
private:
    std::string moduleName;
    std::vector<std::string> inputs, outputs, wires;
    std::vector<Gate*> gates;

    std::string netlistFile;
    std::string libFile;
    std::string patternFile;
    std::string outputLib;
    std::string outputModule;
    // TopModule topmodule;

    Graph graph;
    LUT table;
public:
    STA(std::string netlist, std::string lib, std::string pattern) {
        netlistFile = netlist;
        libFile = lib;
        patternFile = pattern;

        // outputPreffix = libFile.substr(0, (int)libFile.size()-4);
        // outputPreffix.push_back('_');
        // outputPreffix += netlistFile.substr(0, (int)netlistFile.size()-2);
    }
    void run();
    void parseVerilog();
    void dumpGate();
    void parseLib();
    void parsePattern();
    void calLoad();
    void dumpDelay(std::string outputPath,bool append);
    void calDelay();
    void findLongestShortestPath();
    void assignPattern(std::vector<bool>& pattern);
    void simulate();
    void calDelay_controlVal();
    void clearFile(std::string outputPath);
};

#endif 