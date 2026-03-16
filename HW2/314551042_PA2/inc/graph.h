#ifndef GRAPH_H
#define GRAPH_H

#include "netlist.h"

#include <unordered_map>

class Net;
class Cell;
class Graph;

class Cell{
public:
    Gate* gatePtr;
    int id = -1;
    Net* inNet1, * inNet2, * nets;
    

    //for step1
    double outputLoad;
    //for step2
    Cell* parentCell = nullptr;
    double arriveTime = -1;
    double inTrans, outTrans;
    double propagation;
    bool isOne;


    //for step4
    bool inputVal1, inputVal2, outputVal;

    


    std::vector<Cell*> cells;
    Cell(Gate* g, int _id){
        gatePtr = g;
        id = _id;
    }
};

class Net{
public:
    std::string name;
    int numCell;
    int id;
    Cell* inputCell;
    std::vector<Cell*> cells;
    bool isPriInput = false;
    bool isPriOutput = false;
    bool Val;
    Net(std::string _name, int _numCell, int _id, std::vector<Cell*> _cells){
        name = _name;
        numCell = _numCell;
        id = _id;
        cells = _cells;
    }
    Net(std::string _name, int _id){
        name = _name;
        id = _id;
    }
};


class Graph{
public:
    //original data
    std::vector<Gate*> gates;
    std::vector<std::string> wire_in;
    std::vector<std::string> wires;
    std::vector<std::string> wire_out;

    //graph data
    std::vector<Cell*> cells;
    std::vector<Net*> nets;
    std::unordered_map<std::string, int> netTable;

    std::vector<std::pair<std::string, int>> dumpOrder;
    //Topo
    std::vector<int> topoOrder;

    Graph(){};
    void init(std::vector<Gate*> g, std::vector<std::string> w_in, std::vector<std::string> w, std::vector<std::string> w_out);
    void topoSort();
    void dumpSort();
    void setCellTime(Cell* c);
    void setCellTime_controlVal(Cell* c);
};



#endif