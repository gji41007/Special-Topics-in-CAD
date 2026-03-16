#include "STA.h"



#include <fstream>
#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cfloat>



void STA::run(){
    parseLib();
    parseVerilog();
    parsePattern();
    calLoad();
    calDelay();
    findLongestShortestPath();
    calDelay_controlVal();
}
std::vector<std::string> parse_wires(std::string wires){
    std::regex wirePattern(R"(\w+)");

    std::string::const_iterator iterStart = wires.begin();
    std::string::const_iterator iterEnd = wires.end();
    std::smatch result;
    std::vector<std::string> ret;
    while(std::regex_search(iterStart, iterEnd, result, wirePattern)){
        iterStart = result.suffix().first;
        ret.push_back(result[0]);
        // std::cout<<result[0]<<std::endl;
    }
    return ret;
}
void STA::parseVerilog(){
    std::ifstream inFile(netlistFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << netlistFile << std::endl;
        return;
    }


    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string rawCode = buffer.str();


    std::string cleanCode;
    int cid = 0;
    while(cid < (int)rawCode.size()){
        if(rawCode[cid] == '/' && rawCode[cid+1] == '*'){
            cid += 2;
            while(rawCode[cid] != '*' || rawCode[cid+1] != '/'){
                ++cid;
            }
            cid+=2;
        }
        else if(rawCode[cid] == '/' && rawCode[cid+1] == '/'){
            cid += 2;
            while(cid < (int)rawCode.size() && rawCode[cid] != '\n'){
                ++cid;
            }
            ++cid;
        }
        else{cleanCode += rawCode[cid++];}
        // std::cout<<cid<<std::endl;
    } 
    // std::cout<<cleanCode<<std::endl<<"========="<<std::endl;;

    // std::regex singleLineWithoutMutilLine("//(?!.*\\*/).*\n");
    // rawCode = std::regex_replace(rawCode, singleLineWithoutMutilLine, "");
    // // std::cout<<rawCode<<std::endl;std::cout<<"==="<<std::endl;
    // std::regex singleLineBlock(R"(\/\*[^\n]*?\*\/)");
    // rawCode = std::regex_replace(rawCode, singleLineBlock, "");
    // std::cout<<rawCode<<std::endl;std::cout<<"==="<<std::endl;
    // std::regex singleLine(R"(\/\/[^\n]*)");
    // rawCode = std::regex_replace(rawCode, singleLine, "");

    // std::cout<<rawCode<<std::endl;std::cout<<"==="<<std::endl;
    // std::string temp = rawCode;
    // const std::string newlineMarker = "\x01";
    // size_t pos = 0;
    // while ((pos = rawCode.find("\n", pos)) != std::string::npos) {
    //     rawCode.replace(pos, 1, newlineMarker);
    //     pos += newlineMarker.length();
    // }
    // std::cout<<rawCode<<std::endl;std::cout<<"==="<<std::endl;
    // std::regex multiLineComment(R"(/\*.*?\*/)");
    // rawCode = std::regex_replace(rawCode, multiLineComment, "");
    // pos = 0;
    // while ((pos = rawCode.find(newlineMarker, pos)) != std::string::npos) {
    //     rawCode.replace(pos, newlineMarker.length(), "\n");
    //     pos += 1;
    // }
    // std::cout<<rawCode<<std::endl;
    // std::cout<<"========="<<std::endl;
    rawCode = cleanCode;

        // std::cout<<"pass"<<std::endl;
    // std::regex multipleSpaces("\\s+"); 
    // rawCode = std::regex_replace(rawCode, multipleSpaces, " ");

    std::regex modulePattern(R"(module\s*(\w+))"); 
    std::string::const_iterator iterStart = rawCode.begin();
    std::string::const_iterator iterEnd = rawCode.end();
    std::smatch result;
    // std::vector<double> ret;
    
    std::regex_search(iterStart, iterEnd, result, modulePattern);
    moduleName = result[1];



    // std::regex inputPattern(R"(input([^;]*))");
    // std::regex outputPattern(R"(output([^;]*))");
    // std::regex wirePattern(R"(wire([^;]*))");

    iterStart = rawCode.begin();
    iterEnd = rawCode.end();

    // std::cout<<rawCode.size();
        // std::cout<<"pass"<<std::endl;
    // std::regex_search(iterStart, iterEnd, result, inputPattern);
    // std::cout<<"pass"<<std::endl;
    // inputs = parse_wires(result[1]);
    // std::regex_search(iterStart, iterEnd, result, outputPattern);
    // outputs = parse_wires(result[1]);
    // std::regex_search(iterStart, iterEnd, result, wirePattern);
    // wires = parse_wires(result[1]);

    // std::cout<<rawCode<<std::endl;
    std::stringstream ss(rawCode);
    std::string s;
    char c;
    bool isEnd = false;
    while(s != "input"){ss>>s;}
    while(1){
        std::string wireName;
        while(ss>>c){
            if(isalpha(c) || isdigit(c)){wireName += c;}
            else if(c == ';'){
                isEnd = true;
                break;
            }
            else{break;}
        }
        inputs.push_back(wireName);
        if(isEnd){break;}
    }
    ss.clear();
    ss.seekg(0);
    isEnd = false;
    while(s != "output"){ss>>s;}
    while(1){
        std::string wireName;
        while(ss>>c){
            if(isalpha(c) || isdigit(c)){wireName += c;}
            else if(c == ';'){
                isEnd = true;
                break;
            }
            else{break;}
        }
        outputs.push_back(wireName);
        if(isEnd){break;}
    }
    ss.clear();
    ss.seekg(0);
    isEnd = false;
    while(s != "wire"){ss>>s;}
    while(1){
        std::string wireName;
        while(ss>>c){
            if(isalpha(c) || isdigit(c)){wireName += c;}
            else if(c == ';'){
                isEnd = true;
                break;
            }
            else{break;}
        }
        wires.push_back(wireName);
        if(isEnd){break;}
    }
    
    // std::cout<<rawCode<<std::endl;std::cout<<"==="<<std::endl;
    // for(auto& w:inputs){std::cout<<w<<std::endl;}
    // std::cout<<"---"<<std::endl;
    // for(auto& w:outputs){std::cout<<w<<std::endl;}
    // std::cout<<"---"<<std::endl;
    // for(auto& w:wires){std::cout<<w<<std::endl;}
    // std::cout<<"---"<<std::endl;

    /*read gate*/
    std::regex invPattern(R"(INV\w*\s+(\w+))");
    std::regex nandPattern(R"(NAND\w*\s+(\w+))");
    std::regex norPattern(R"(NOR\w*\s+(\w+))");

    iterStart = rawCode.begin();
    iterEnd = rawCode.end();


    while(std::regex_search(iterStart, iterEnd, result, invPattern)){
        iterStart = result.suffix().first;
        std::string gateName = result[1];
        std::string::const_iterator iterGate = result.suffix().first;
        std::string::const_iterator currIter;
        //search pin[0]
        std::string input1Name = table.gateinfo[(int)gateType::INV].pins[0].pinname;
        std::regex input1Pattern(input1Name);
        std::regex_search(iterGate, iterEnd, result, input1Pattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string input1 = result[0];

        //search pin[1]
        std::string outputName = table.gateinfo[(int)gateType::INV].pins[1].pinname;
        std::regex outputPattern(outputName);
        std::regex_search(iterGate, iterEnd, result, outputPattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string output = result[0];

        Gate* g = new Gate(gateType::INV, gateName, input1, output);
        gates.push_back(g);
        // std::cout<<gateName<<" "<<input1<<" "<<output<<std::endl;
    }
    iterStart = rawCode.begin();
    while(std::regex_search(iterStart, iterEnd, result, nandPattern)){
        iterStart = result.suffix().first;
        std::string gateName = result[1];
        std::string::const_iterator iterGate = result.suffix().first;
        std::string::const_iterator currIter;

        //search pin[0]
        std::string input1Name = table.gateinfo[(int)gateType::NAND].pins[0].pinname;
        std::regex input1Pattern(input1Name);
        std::regex_search(iterGate, iterEnd, result, input1Pattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string input1 = result[0];

        //search pin[1]
        std::string input2Name = table.gateinfo[(int)gateType::NAND].pins[1].pinname;
        std::regex input2Pattern(input2Name);
        std::regex_search(iterGate, iterEnd, result, input2Pattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string input2 = result[0];

        //search pin[2]
        std::string outputName = table.gateinfo[(int)gateType::NAND].pins[2].pinname;
        std::regex outputPattern(outputName);
        std::regex_search(iterGate, iterEnd, result, outputPattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string output = result[0];

        Gate* g = new Gate(gateType::NAND, gateName, input1, input2, output);
        gates.push_back(g);
        // std::cout<<gateName<<" "<<input1<<" "<<output<<std::endl;
    }
    iterStart = rawCode.begin();
    while(std::regex_search(iterStart, iterEnd, result, norPattern)){
        iterStart = result.suffix().first;
        std::string gateName = result[1];
        std::string::const_iterator iterGate = result.suffix().first;
        std::string::const_iterator currIter;

        //search pin[0]
        std::string input1Name = table.gateinfo[(int)gateType::NOR].pins[0].pinname;
        std::regex input1Pattern(input1Name);
        std::regex_search(iterGate, iterEnd, result, input1Pattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string input1 = result[0];

        //search pin[1]
        std::string input2Name = table.gateinfo[(int)gateType::NOR].pins[1].pinname;
        std::regex input2Pattern(input2Name);
        std::regex_search(iterGate, iterEnd, result, input2Pattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string input2 = result[0];

        //search pin[2]
        std::string outputName = table.gateinfo[(int)gateType::NOR].pins[2].pinname;
        std::regex outputPattern(outputName);
        std::regex_search(iterGate, iterEnd, result, outputPattern);
        currIter = result.suffix().first;
        std::regex_search(currIter, iterEnd, result, std::regex(R"(\w+)"));
        std::string output = result[0];

        Gate* g = new Gate(gateType::NOR, gateName, input1, input2, output);
        gates.push_back(g);
        // std::cout<<gateName<<" "<<input1<<" "<<output<<std::endl;
    }
    // dumpGate();
    // gates.clear();


    // dumpGate();
}

std::ostream& operator<<(std::ostream& os, const Gate& gate) {
    os << "Gate: " << gate.name << " [Type: " << type2Str(gate.type) << "]";
    os << " Inputs: " << gate.input1;
    if (!gate.input2.empty()) {
        os << ", " << gate.input2;
    }
    os << " -> Output: " << gate.output;
    return os;
}
void STA::dumpGate(){for(auto& g:gates)if(g)std::cout<<(*g)<<std::endl;}
std::vector<double> parse_floats(std::string floats){
    std::regex floatPattern(R"([0-9]+\.[0-9]+)");
    std::string::const_iterator iterStart = floats.begin();
    std::string::const_iterator iterEnd = floats.end();
    std::smatch result;

    std::vector<double> ret;
    while(std::regex_search(iterStart, iterEnd, result, floatPattern)){
        iterStart = result.suffix().first;
        double d = stod(result[0]);
        ret.push_back(d);
        // std::cout<<d<<std::endl;
    }
    return ret;
}
std::vector<std::vector<double>> parse_floats_2D(std::string float2D, int x, int y){
    std::regex floatsPattern(R"(\"\s*([^"]*)\s*\")");
    std::string::const_iterator iterStart = float2D.begin();
    std::string::const_iterator iterEnd = float2D.end();
    std::smatch result;

    std::vector<std::vector<double>> ret;
    // std::cout<<float2D<<std::endl<<"-----"<<std::endl;
    while(std::regex_search(iterStart, iterEnd, result, floatsPattern)){
        iterStart = result.suffix().first;
        // std::cout<<parse_floats(result[1]).size()<<std::endl;
        // for(auto d:parse_floats(result[1])){std::cout<<d<<" ";}
        // std::cout<<std::endl;
        ret.push_back(parse_floats(result[1]));
    }
    // std::cout<<"===="<<std::endl;
    // std::cout<<ret.size()<<" "<<ret[0].size()<<std::endl;
    return ret;
}
void STA::parseLib(){
    std::ifstream inFile(libFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << libFile << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string rawLib = buffer.str();

    std::string cleanCode;
    int cid = 0;
    while(cid < (int)rawLib.size()){
        if(rawLib[cid] == '/' && rawLib[cid+1] == '*'){
            cid += 2;
            while(rawLib[cid] != '*' || rawLib[cid+1] != '/'){
                ++cid;
            }
            cid+=2;
        }
        else{cleanCode += rawLib[cid++];}
    } 
    rawLib = cleanCode;


    std::regex libNamePattern(R"(library\s*\((\w+)\))");
    std::regex templatePattern(R"(lu_table_template\s*\((\w+))");
    std::regex index1Pattern(R"(index_1\s*\(\s*\"([^"]*)\"\);)");
    std::regex index2Pattern(R"(index_2\s*\(\s*\"([^"]*)\"\);)");

    std::regex cellPattern(R"(cell\s*\((\w+)\)\s*\{)");
    std::regex pinPattern(R"(pin\s*\((\w+)\)\s*\{\s*)");//direction\s*:\s*(\w+)\s*;\s*capacitance\s*:\s*)");
    std::regex dirPattern(R"(direction\s*:\s*(\w+)\s*;\s*)");
    std::regex capPattern(R"(capacitance\s*:\s*)");
    std::regex floatPattern(R"([0-9]+\.[0-9]+)");
    std::regex floatsPattern(R"(\"\s*([^"]*)\s*)");
    std::regex valuesPattern(R"(values\s*\(([^\)]*)\))");


    std::string::const_iterator iterStart = rawLib.begin();
    std::string::const_iterator iterEnd = rawLib.end();
    std::smatch result;

    std::regex_search(iterStart, iterEnd, result, libNamePattern);
    iterStart = result.suffix().first;
    outputLib = result[1];
    int templateId = 0;
    while(std::regex_search(iterStart, iterEnd, result, templatePattern)){
        iterStart = result.suffix().first;
        // std::string::const_iterator templateIter = result.suffix().first;
        std::string tableTemplate = result[1];
        std::regex_search(iterStart, iterEnd, result, index1Pattern);
        int tid;
        if(table.indexMap.find(tableTemplate) == table.indexMap.end()){
            tid = templateId;
            table.indexMap[tableTemplate] = tid;
            ++templateId;
        }
        else{
            tid = table.indexMap[tableTemplate];
        }
        for(int i = 0; i < 3; ++i){
            table.index1[tid] = parse_floats(result[1]);
        }
        std::regex_search(iterStart, iterEnd, result, index2Pattern);
        for(int i = 0; i < 3; ++i){
            table.index2[tid] = parse_floats(result[1]);
        }
    }
    

    iterStart = rawLib.begin();
    while(std::regex_search(iterStart, iterEnd, result, cellPattern)){
        iterStart = result.suffix().first;
        std::string gname = result[1];
        // std::cout<<gname<<std::endl;

        // int pinNum = (gname.substr(0, 3) == "INV")? 2:3;
        gateType gtype = str2Type(gname);
        int pinNum = (gtype == gateType::INV)? 2:3;
        
        int id = 0;
        for(int _ = 0; _ < pinNum; ++_){
            std::regex_search(iterStart, iterEnd, result, pinPattern);
            iterStart = result.suffix().first;
            std::string pinname = result[1];

            std::regex_search(iterStart, iterEnd, result, dirPattern);
            iterStart = result.suffix().first;
            std::string direct = result[1];
            // std::cout<<pinname<<" "<<direct<<std::endl;

            std::regex_search(iterStart, iterEnd, result, floatPattern);
            iterStart = result.suffix().first;
            double cap = std::stod(result[0]);

            int pId;
            if(direct == "output"){pId = pinNum - 1;}
            else{pId = id; ++id;}
            table.gateinfo[(int)gtype].pinId[pinname] = pId;
            table.gateinfo[(int)gtype].pins[pId].pinname = pinname;
            table.gateinfo[(int)gtype].pins[pId].isOutput = (pId == pinNum-1);
            table.gateinfo[(int)gtype].pins[pId].capacitance = cap;
            // std::cout<<pinname<<" "<<pId<<" "<<table.gateinfo[(int)str2Type(gname)].pins[pId].capacitance<<std::endl;


            std::string::const_iterator iterOriginal = iterStart;
            if(direct == "output"){
                // std::cout << "Searching for rise_power from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, std::regex(R"(rise_power\s*\((\w+)\s*\))"));
                iterStart = result.suffix().first;
                int tid = table.indexMap[result[1]]; 
                // std::cout << "Searching for rise_power from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, valuesPattern);
                table.rpower[(int)gtype] = ValTable(table.index1[tid], table.index2[tid]);
                table.rpower[(int)gtype].valtable = parse_floats_2D(result[1], (int)table.index1[tid].size(), (int)table.index2[tid].size());
                iterStart = iterOriginal;

                std::regex_search(iterStart, iterEnd, result, std::regex(R"(fall_power\s*\((\w+)\s*\))"));
                iterStart = result.suffix().first;
                tid = table.indexMap[result[1]]; 
                // std::cout << "Searching for fall_power from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, valuesPattern);
                table.fpower[(int)gtype] = ValTable(table.index1[tid], table.index2[tid]);
                table.fpower[(int)gtype].valtable = parse_floats_2D(result[1], (int)table.index1[tid].size(), (int)table.index2[tid].size());
                iterStart = iterOriginal;
                // table.fpower[(int)str2Type(gname)].dumpTable();

                std::regex_search(iterStart, iterEnd, result, std::regex(R"(cell_rise\s*\((\w+)\s*\))"));
                iterStart = result.suffix().first;
                tid = table.indexMap[result[1]]; 
                // std::cout << "Searching for cell_rise from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, valuesPattern);
                table.rtime[(int)gtype] = ValTable(table.index1[tid], table.index2[tid]);
                table.rtime[(int)gtype].valtable = parse_floats_2D(result[1], (int)table.index1[tid].size(), (int)table.index2[tid].size());
                iterStart = iterOriginal;

                std::regex_search(iterStart, iterEnd, result, std::regex(R"(cell_fall\s*\((\w+)\s*\))"));
                iterStart = result.suffix().first;
                tid = table.indexMap[result[1]]; 
                // std::cout << "Searching for cell_fall from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, valuesPattern);
                table.ftime[(int)gtype] = ValTable(table.index1[tid], table.index2[tid]);
                table.ftime[(int)gtype].valtable = parse_floats_2D(result[1], (int)table.index1[tid].size(), (int)table.index2[tid].size());
                iterStart = iterOriginal;

                // std::cout << "Searching for rise_trans from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, std::regex(R"(rise_transition\s*\((\w+)\s*\))"));
                iterStart = result.suffix().first;
                tid = table.indexMap[result[1]]; 
                // std::cout << "Searching for rise_trans from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, valuesPattern);
                table.rtrans[(int)gtype] = ValTable(table.index1[tid], table.index2[tid]);
                table.rtrans[(int)gtype].valtable = parse_floats_2D(result[1], (int)table.index1[tid].size(), (int)table.index2[tid].size());
                iterStart = iterOriginal;

                // std::cout << "Searching for fall_trans from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, std::regex(R"(fall_transition\s*\((\w+)\s*\))"));
                iterStart = result.suffix().first;
                tid = table.indexMap[result[1]]; 
                // std::cout << "Searching for fall_trans from position: " << (iterStart - iterOriginal) << std::endl;
                std::regex_search(iterStart, iterEnd, result, valuesPattern);
                table.ftrans[(int)gtype] = ValTable(table.index1[tid], table.index2[tid]);
                table.ftrans[(int)gtype].valtable = parse_floats_2D(result[1], (int)table.index1[tid].size(), (int)table.index2[tid].size());
                iterStart = iterOriginal;
            }

        }
    }

}

void STA::parsePattern(){
    std::ifstream inFile(patternFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << libFile << std::endl;
        return;
    }

    // std::stringstream buffer;
    // buffer << inFile.rdbuf();
    // std::string rawLib = buffer.str();
    std::string strline;
    getline(inFile, strline);
    getline(inFile, strline);
    while(strline[0] != '.'){
        std::stringstream ss(strline);
        std::vector<bool> patt;
        // for(int _ = 0; _ < (int)table[0].index1.size(); ++_){
        bool b;
        while(ss>>b){
            
            patt.push_back(b);
        }
        table.patterns.push_back(patt);
        // std::cout<<strline<<std::endl;
        getline(inFile, strline);
    }
}


bool cmp(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    const std::string& strA = a.first;
    const std::string& strB = b.first;
    
    int numA = 0, numB = 0;
    int i = strA.length() - 1;
    while (i >= 0 && std::isdigit(strA[i])) {i--;}
    if (i < (int)strA.length() - 1){
        numA = std::stoi(strA.substr(i + 1));
    }
    
    i = strB.length() - 1;
    while (i >= 0 && std::isdigit(strB[i])) {i--;}
    if (i < (int)strB.length() - 1){
        numB = std::stoi(strB.substr(i + 1));
    }

    return numA < numB;
}
void Graph::dumpSort(){
    dumpOrder.clear();
    // std::vector<std::pair<std::string, int>> vec;
    for(int i = 0; i < (int)cells.size(); ++i){
        dumpOrder.push_back(std::make_pair(cells[i]->gatePtr->name, i));
    }
    std::sort(dumpOrder.begin(), dumpOrder.end(), cmp);
}
void STA::calLoad(){
    //init graph
    std::vector<std::string> merged;
    // merged.insert(merged.end(), topmodule.inputs.begin(), topmodule.inputs.end());
    // merged.insert(merged.end(), topmodule.wires.begin(), topmodule.wires.end());
    // merged.insert(merged.end(), topmodule.outputs.begin(), topmodule.outputs.end());
    graph.init(gates, inputs, wires, outputs);
    // std::cout<<merged.size()<<std::endl;
    // for(auto s:merged){std::cout<<s.size()<<" "<<s<<std::endl;}
    // std::cout<<"========"<<std::endl;


    //compute load

    // for(auto g:table.gateinfo)for(auto c:g.pins){std::cout<<c.capacitance<<std::endl;}

    std::vector<std::pair<std::string, double>> loadVal;
    for(auto& c: graph.cells){
        double totalLoad = 0;
        std::string outputNet = c->gatePtr->output;
        // std::cout<<c->gatePtr->name<<std::endl;
        if(c->nets->isPriOutput)totalLoad += 0.03;
        for(auto& fanout: c->cells){
            // std::cout<<fanout->gatePtr->name<<" "<<fanout->gatePtr->input1<<" "<<fanout->gatePtr->input2<<" "<<outputNet<<std::endl;
            if(fanout->gatePtr->input1 == outputNet)
                totalLoad += table.gateinfo[(int)fanout->gatePtr->type].pins[0].capacitance;
            if(fanout->gatePtr->input2 == outputNet)
                totalLoad += table.gateinfo[(int)fanout->gatePtr->type].pins[1].capacitance;
        }
        // std::cout<<totalLoad<<std::endl;
        c->outputLoad = totalLoad;
        loadVal.push_back(std::make_pair(c->gatePtr->name, totalLoad));
    }
    
    std::sort(loadVal.begin(), loadVal.end(), cmp);

    graph.dumpSort();

    // std::string outputPath = outputPreffix;
    std::string outputPath = outputLib + "_" + moduleName + "_load.txt";
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open file " << outputPath << std::endl;
        return;
    }
    outFile << std::fixed << std::setprecision(6);
    for (int i = 0 ; i < (int)graph.dumpOrder.size(); ++i) {
        outFile << graph.dumpOrder[i].first << " " << graph.cells[graph.dumpOrder[i].second]->outputLoad << std::endl;
    }
    
    outFile.close();
}

double bilinear(double x, double x1, double x2, double y1, double y2){
    if(x == x1){return y1;}
    if(x == x2){return y2;}
    return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
}

double ValTable::lookUp(Cell* c){
    double x = c->outputLoad, y = c->inTrans;

    int xid = -1, yid = -1;
    for(; xid+1<(int)index1.size() && x >= index1[xid+1]; ++xid);
    for(; yid+1<(int)index2.size() && y >= index2[yid+1]; ++yid);
    // std::cout<<xid<<" "<<yid<<std::endl;
    // std::cout<<x<<" "<<index1[xid+1]<<std::endl;
    // std::cout<<y<<" "<<index2[yid+1]<<std::endl;
    int xid1, xid2, yid1, yid2;
    if(xid == -1){
        xid1 = 0;
        xid2 = 1;
    }
    else if(xid >= (int)index1.size() - 1){
        xid1 = index1.size() - 2;
        xid2 = index1.size() - 1;
    }
    else{
        xid1 = xid;
        xid2 = xid + 1;
    }
    if(yid == -1){
        yid1 = 0;
        yid2 = 1;
    }
    else if(yid >= (int)index2.size() - 1){
        yid1 = index2.size() - 2;
        yid2 = index2.size() - 1;
    }
    else{
        yid1 = yid;
        yid2 = yid + 1;
    }

    // for(int i = 0; i < valtable.size(); ++i){
    //     for(int j = 0; j < valtable[0].size(); ++j){
    //         std::cout<<valtable[i][j]<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    double z11 = valtable[yid1][xid1];
    double z12 = valtable[yid2][xid1];
    double z21 = valtable[yid1][xid2];
    double z22 = valtable[yid2][xid2];
    // std::cout<<x<<" "<<y<<std::endl;
    // std::cout<<z11<<" "<<z12<<" "<<z21<<" "<<z22<<std::endl;
    double z1 = bilinear(y, index2[yid1], index2[yid2], z11, z12);
    double z2 = bilinear(y, index2[yid1], index2[yid2], z21, z22);
    return bilinear(x, index1[xid1], index1[xid2], z1, z2);

}
void Graph::setCellTime(Cell* c){
    if(c->gatePtr->type == gateType::INV){
        if(c->inNet1->isPriInput){
            c->inTrans = 0;
            c->arriveTime = 0;
        }
        else{
            Cell* prevCell = c->inNet1->inputCell;
            c->inTrans = prevCell->outTrans;
            c->arriveTime = prevCell->arriveTime + prevCell->propagation + 0.005;
            c->parentCell = prevCell;
        }
    }
    else{
        if(c->inNet1->isPriInput && c->inNet2->isPriInput){
            c->inTrans = 0;
            c->arriveTime = 0;
        }
        else if(c->inNet1->isPriInput){
            Cell* prevCell = c->inNet2->inputCell;
            c->inTrans = prevCell->outTrans;
            c->arriveTime = prevCell->arriveTime + prevCell->propagation + 0.005;
            c->parentCell = prevCell;
        }
        else if(c->inNet2->isPriInput){
            Cell* prevCell = c->inNet1->inputCell;
            c->inTrans = prevCell->outTrans;
            c->arriveTime = prevCell->arriveTime + prevCell->propagation + 0.005;
            c->parentCell = prevCell;
        }
        else{
            Cell* prevCell1 = c->inNet1->inputCell;
            Cell* prevCell2 = c->inNet2->inputCell;
            // c->inTrans = prevCell->outTrans;
            double arriveTime1 = prevCell1->arriveTime + prevCell1->propagation + 0.005;
            double arriveTime2 = prevCell2->arriveTime + prevCell2->propagation + 0.005;
            if(arriveTime1 > arriveTime2){
                c->inTrans = prevCell1->outTrans;
                c->arriveTime = arriveTime1;
                c->parentCell = prevCell1;
            }
            else{
                c->inTrans = prevCell2->outTrans;
                c->arriveTime = arriveTime2;
                c->parentCell = prevCell2;
            }
        }
    }
}
void STA::clearFile(std::string outputPath) {
    std::ofstream clearFile(outputPath, std::ios::trunc);
    if (!clearFile.is_open()) {
        std::cerr << "Error: Cannot clear file " << outputPath << std::endl;
        return;
    }
    clearFile.close();
}
void STA::dumpDelay(std::string outputPath, bool append){
    std::ios_base::openmode mode = std::ios::out;
    if (append) mode |= std::ios::app;  
    
    std::ofstream outFile(outputPath, mode);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open file " << outputPath << std::endl;
        return;
    }
    
    // 一次性构建所有内容
    std::ostringstream buffer;
    buffer << std::fixed << std::setprecision(6);
    
    char line[256];
    for (int i = 0; i < (int)graph.dumpOrder.size(); ++i) {
        auto cell = graph.cells[graph.dumpOrder[i].second];
        snprintf(line, sizeof(line), "%s %d %.6f %.6f\n",
                graph.dumpOrder[i].first.c_str(),
                cell->isOne,
                cell->propagation,
                cell->outTrans);
        buffer << line;
    }
    buffer << "\n";
    
    outFile << buffer.str();
    outFile.close();
}
void STA::calDelay(){
    graph.topoSort();

    for(int i = 0; i < (int)graph.topoOrder.size(); ++i){
        Cell* c = graph.cells[graph.topoOrder[i]];
        graph.setCellTime(c);
        double rt = table.rtime[(int)c->gatePtr->type].lookUp(c);
        double ft = table.ftime[(int)c->gatePtr->type].lookUp(c);
        double rtrans = table.rtrans[(int)c->gatePtr->type].lookUp(c);
        double ftrans = table.ftrans[(int)c->gatePtr->type].lookUp(c);

        if(rt > ft){
            c->outTrans = rtrans;
            c->propagation = rt;
            c->isOne = true;
        }
        else{
            c->outTrans = ftrans;
            c->propagation = ft;
            c->isOne = false;
        }

    }

    std::string outputPath = outputLib + "_" + moduleName + "_delay.txt";
    dumpDelay(outputPath, false);
    // std::ofstream outFile(outputPath);
    // if (!outFile.is_open()) {
    //     std::cerr << "Error: Cannot open file " << outputPath << std::endl;
    //     return;
    // }
    // outFile << std::fixed << std::setprecision(6);
    // for (int i = 0 ; i < (int)graph.dumpOrder.size(); ++i) {
    //     outFile << graph.dumpOrder[i].first << " " 
    //             << graph.cells[graph.dumpOrder[i].second]->isOne<< " " 
    //             << graph.cells[graph.dumpOrder[i].second]->propagation<< " " 
    //             << graph.cells[graph.dumpOrder[i].second]->outTrans
    //             << std::endl;
    // }

    // outFile.close();

}

void STA::findLongestShortestPath(){
    double longest = 0, shortest = DBL_MAX;
    int longId = -1, shortId = -1;
    for(int i = 0; i < (int)graph.cells.size(); ++i){
        if(!graph.cells[i]->nets->isPriOutput){continue;}
        double delay = graph.cells[i]->arriveTime + graph.cells[i]->propagation;
        if(delay > longest){
            longest = delay;
            longId = i;
        }
        if(delay < shortest){
            shortest = delay;
            shortId = i;
        }
    }
    std::string outputPath = outputLib + "_" + moduleName + "_path.txt";
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open file " << outputPath << std::endl;
        return;
    }
    
    std::vector<std::string> longestPath;
    longestPath.push_back(graph.cells[longId]->gatePtr->output);
    Cell* currCell = graph.cells[longId];
    while(currCell->parentCell != nullptr){
        currCell = currCell->parentCell;
        longestPath.push_back(currCell->gatePtr->output);
    }
    longestPath.push_back(currCell->gatePtr->input1);
    std::reverse(longestPath.begin(), longestPath.end());
    outFile << std::fixed << std::setprecision(6);
    outFile<<"Longest delay = "<<longest<<", the path is: ";
    for (int i = 0; i < (int)longestPath.size() - 1; ++i) {
        outFile<<longestPath[i]<<" -> ";
    }
    outFile<<longestPath.back()<<std::endl;


    std::vector<std::string> shortestPath;
    shortestPath.push_back(graph.cells[shortId]->gatePtr->output);
    currCell = graph.cells[shortId];
    while(currCell->parentCell != nullptr){
        currCell = currCell->parentCell;
        shortestPath.push_back(currCell->gatePtr->output);
    }
    shortestPath.push_back(currCell->gatePtr->input1);
    std::reverse(shortestPath.begin(), shortestPath.end());
    outFile << std::fixed << std::setprecision(6);
    outFile<<"Shortest delay = "<<shortest<<", the path is: ";
    for (int i = 0; i < (int)shortestPath.size() - 1; ++i) {
        outFile<<shortestPath[i]<<" -> ";
    }
    outFile<<shortestPath.back();

    outFile.close();
}

void STA::assignPattern(std::vector<bool>& pattern){
    int nid = 0;
    // std::cout<<graph.wire_in.size()<<" "<<graph.nets.size()<<std::endl;
    // for(auto n: graph.nets){
    //     std::cout<<n->name<<" "<<n->isPriInput<<std::endl;
    // }
    for(auto boolVal : pattern){
        while(!graph.nets[nid]->isPriInput){++nid;}
        // std::cout<<nid<<std::endl;}
        graph.nets[nid]->Val = boolVal;
        ++nid;
        // std::cout<<"----"<<std::endl;
    }
}
void Graph::setCellTime_controlVal(Cell* c){
    if(c->gatePtr->type == gateType::INV){
        if(c->inNet1->isPriInput){
            c->inTrans = 0;
            c->arriveTime = 0;
        }
        else{
            Cell* prevCell = c->inNet1->inputCell;
            c->inTrans = prevCell->outTrans;
            c->arriveTime = prevCell->arriveTime + prevCell->propagation + 0.005;
            c->parentCell = prevCell;
        }
        c->inputVal1 = c->inNet1->Val;
        c->outputVal = !c->inputVal1;
        c->nets->Val = c->outputVal;
    }
    else{
        bool controlVal = (c->gatePtr->type == gateType::NOR);
        if(c->inNet1->isPriInput && c->inNet2->isPriInput){
            c->inTrans = 0;
            c->arriveTime = 0;
        }
        else if(c->inNet1->isPriInput){
            if(c->inNet1->Val == controlVal){
                c->inTrans = 0;
                c->arriveTime = 0;
            }
            else{
                Cell* prevCell = c->inNet2->inputCell;
                c->inTrans = prevCell->outTrans;
                c->arriveTime = prevCell->arriveTime + prevCell->propagation + 0.005;
                c->parentCell = prevCell;
            }
        }
        else if(c->inNet2->isPriInput){
            if(c->inNet2->Val == controlVal){
                c->inTrans = 0;
                c->arriveTime = 0;
            }
            else{
                Cell* prevCell = c->inNet1->inputCell;
                c->inTrans = prevCell->outTrans;
                c->arriveTime = prevCell->arriveTime + prevCell->propagation + 0.005;
                c->parentCell = prevCell;
            }
        }
        else{
            Cell* prevCell1 = c->inNet1->inputCell;
            Cell* prevCell2 = c->inNet2->inputCell;
            // c->inTrans = prevCell->outTrans;
            double arriveTime1 = prevCell1->arriveTime + prevCell1->propagation + 0.005;
            double arriveTime2 = prevCell2->arriveTime + prevCell2->propagation + 0.005;
            if(arriveTime1 > arriveTime2){
                if(c->inNet2->Val == controlVal){   
                    c->inTrans = prevCell2->outTrans;
                    c->arriveTime = arriveTime2;
                    c->parentCell = prevCell2;
                }
                else{
                    c->inTrans = prevCell1->outTrans;
                    c->arriveTime = arriveTime1;
                    c->parentCell = prevCell1;
                }
            }
            else{
                if(c->inNet1->Val == controlVal){
                    c->inTrans = prevCell1->outTrans;
                    c->arriveTime = arriveTime1;
                    c->parentCell = prevCell1;
                }
                else{
                    c->inTrans = prevCell2->outTrans;
                    c->arriveTime = arriveTime2;
                    c->parentCell = prevCell2;
                }
            }
        }
        c->inputVal1 = c->inNet1->Val;
        c->inputVal2 = c->inNet2->Val;
        c->outputVal = (c->gatePtr->type == gateType::NOR)? !(c->inputVal1 || c->inputVal2) : !(c->inputVal1 && c->inputVal2);
        c->nets->Val = c->outputVal;
    }
}
void STA::simulate(){
    for(int i = 0; i < (int)graph.topoOrder.size(); ++i){
        Cell* c = graph.cells[graph.topoOrder[i]];
        graph.setCellTime_controlVal(c);
        graph.setCellTime_controlVal(c);

        if(c->outputVal){
            c->outTrans = table.rtrans[(int)c->gatePtr->type].lookUp(c);
            c->propagation = table.rtime[(int)c->gatePtr->type].lookUp(c);
            c->isOne = true;
        }
        else{
            c->outTrans = table.ftrans[(int)c->gatePtr->type].lookUp(c);
            c->propagation = table.ftime[(int)c->gatePtr->type].lookUp(c);
            c->isOne = false;
        }
    }
}
void STA::calDelay_controlVal(){
    std::string outputPath = outputLib + "_" + moduleName + "_gate_info.txt";
    clearFile(outputPath);
    for(auto& patt: table.patterns){
        assignPattern(patt);
        simulate();
        dumpDelay(outputPath, true);
    }
}

