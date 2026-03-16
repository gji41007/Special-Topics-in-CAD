#include "graph.h"

#include <queue>

void Graph::init(std::vector<Gate*> g, std::vector<std::string> w_in, std::vector<std::string> w, std::vector<std::string> w_out){
    gates = g;
    wire_in = w_in;
    wires = w;
    wire_out = w_out;

    // for(auto gate:g){
    //     std::cout<<gate->name<<" "<<type2Str(gate->type)<<std::endl;
    //     std::cout<<gate->input1<<" "<<gate->input2<<" "<<gate->output<<std::endl;
    // }
    // std::cout<<"----"<<std::endl;
    int netidx;
    for(netidx = 0 ; netidx < (int)w_in.size(); ++netidx){
        Net* n = new Net(w_in[netidx], netidx);
        n->isPriInput = true;
        nets.push_back(n);

        netTable[w_in[netidx]] = netidx;
    }
    for(int i = 0 ; i < (int)w.size(); ++i, ++netidx){
        Net* n = new Net(w[i], netidx);
        nets.push_back(n);

        netTable[w[i]] = netidx;
    }
    for(int i = 0 ; i < (int)w_out.size(); ++i, ++netidx){
        Net* n = new Net(w_out[i], netidx);
        n->isPriOutput = true;
        nets.push_back(n);

        netTable[w_out[i]] = netidx;
    }
    // cell and input wire
    for(int i = 0 ; i < (int)gates.size(); ++i){
        Cell* c = new Cell(gates[i], i);
        cells.push_back(c);
        int netId = netTable[gates[i]->input1];
        nets[netId]->cells.push_back(c);
        c->inNet1 = nets[netId];
        if(gates[i]->type != gateType::INV){
            int netId2 = netTable[gates[i]->input2];
            nets[netId2]->cells.push_back(c);
            c->inNet2 = nets[netId2];
        }
    }
    //output wire
    for(int i = 0 ; i < (int)gates.size(); ++i){
        int netId = netTable[gates[i]->output];
        Net* n = nets[netId];
        cells[i]->nets = n;
        for(int neighbor = 0; neighbor < (int)n->cells.size(); ++neighbor){
            cells[i]->cells.push_back(n->cells[neighbor]);
        }
        n->inputCell = cells[i];
    }

    // for(auto c:cells){
    //     std::cout<<c->gatePtr->name<<":"<<std::endl;
    //     for(auto neigh: c->cells)std::cout<<neigh->gatePtr->name<<" ";
    //     std::cout<<std::endl;
    //     std::cout<<c->nets->name<<" ";
    //     std::cout<<std::endl;
    // }
    // std::cout<<"---"<<std::endl;
    // for(auto n:nets){
    //     std::cout<<n->name<<":"<<std::endl;
    //     for(auto neigh: n->cells)std::cout<<neigh->gatePtr->name<<" ";
    //     std::cout<<std::endl;
    // }
    // std::cout<<"---"<<std::endl;

};
void Graph::topoSort(){
    std::vector<int> inDegree(cells.size());
    for(int i = 0 ; i < (int)cells.size(); ++i){
        int count = 0;
        if(!cells[i]->inNet1->isPriInput){++count;}
        if(cells[i]->gatePtr->type != gateType::INV && !cells[i]->inNet2->isPriInput){++count;}
        inDegree[i] = count;
    }

    std::queue<int> cellQ;
    for(int i = 0; i < (int)cells.size(); ++i){
        if(inDegree[i] == 0){
            cellQ.push(i);
            cells[i]->arriveTime = 0;
            cells[i]->inTrans = 0;
            cells[i]->parentCell = nullptr;
        }
    }
    
    topoOrder.clear();
    for(int _ = 0; _ < (int)cells.size(); ++_){
        int cid = cellQ.front();
        cellQ.pop();
        topoOrder.push_back(cid);

        for(auto c: cells[cid]->cells){
            if(c->inNet1 == cells[cid]->nets){--inDegree[c->id];}
            if(c->inNet2 == cells[cid]->nets){--inDegree[c->id];}
            if(inDegree[c->id] == 0){cellQ.push(c->id);}
        }
    }
    
};