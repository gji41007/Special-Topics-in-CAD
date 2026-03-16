#include "QM.h"

#include <fstream>
#include <sstream>
#include <limits>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>
#include <unordered_set>

#include <thread>
#include <future>
#include <vector>
#include <mutex>

void QM::genPrimeSet(){
    std::vector<std::vector<Implicant>> currTable(nBit+1);
    std::vector<std::vector<Implicant>> nextTable(nBit);

    for(auto& imp: onSet){
        currTable[imp.getOnes()].push_back(imp);
    }
    for(auto& imp: dcSet){
        currTable[imp.getOnes()].push_back(imp);
    }


    bool isDone = false;
    int numCandidate = (int)onSet.size() + (int)dcSet.size();
    while(!isDone){
        isDone = true;
        nextTable.clear();
        nextTable.resize(currTable.size()-1);
        std::unordered_set<std::string> SOPset;
        SOPset.reserve(numCandidate);
        numCandidate = 0;
        for(int ones = 0; ones < (int)currTable.size() -1; ++ones){
            for(auto& imp: currTable[ones]){
                if(imp.isMerge){continue;}
                for(auto& cmp: currTable[ones+1]){
                    if(ENABLECOUNTPRIME && imp.isMerge){break;}
                    int idx = imp.quickDiff(cmp);//diffIndex(cmp);
                    // std::cout<<idx<<std::endl;
                    if(idx == -1){continue;}
                    std::vector<uint32_t> contain = imp.containNum;
                    for(auto& n:cmp.containNum){contain.push_back(n);}
                    Implicant tImp = Implicant(imp.bits, imp.mask, imp.isON || cmp.isON, contain);
                    tImp.setMask(idx, 1);
                    if(SOPset.find(tImp.getSOP(nBit)) != SOPset.end()){continue;}
                    SOPset.insert(tImp.getSOP(nBit));
                    nextTable[ones].push_back(tImp);

                    //set flag
                    imp.isPrime = false;
                    cmp.isPrime = false;
                    isDone = false;
                    ++numCandidate;
                    if(ENABLECOUNTPRIME){
                        // --imp.countPrime;
                        // --cmp.countPrime;
                        imp.isMerge = true;
                        cmp.isMerge = true;
                        break;
                    }
                }
            }

        }

        //prepare data
        for(int ones = 0; ones < (int)currTable.size(); ++ones){
            for(auto& imp: currTable[ones]){
                // std::cout<<imp.getSOP(nBit)<<std::endl;
                if(imp.isPrime && imp.isON)primeSet.push_back(imp);
            }
        }
        int sum = 0;
        for(int ones = 0; ones < (int)currTable.size(); ++ones){
            // std::cout<<ones<<": "<<currTable[ones].size()<<std::endl;
            sum += currTable[ones].size();
        }
        std::cout<<"Total: "<<sum<<std::endl;
        currTable = nextTable;
    }
}




int QM::findLargest(){
    int maxCover = 0;
    int bestID = -1;
    for(int i = 0; i < (int)coverTable.size(); ++i){
        if((int)coverTable[i].size() > maxCover){
            maxCover = coverTable[i].size();
            bestID = i;
        }
    }
    return bestID;
}
int QM::findClosest(){
    // std::cout<<"from closest"<<std::endl;
    for(int i = (int)countTable.size() - 1; i >= 0 ; --i){
        if(countTable[i] > 0){
            return i;
        }
    }
    return -1;
}
int QM::findEssential(){
    for(auto& n: unCovered){
        if(ElementToImp[n] == 1&&unCovered.find(n)!=unCovered.end()){
           for(auto& impID: reverseCoverTable[n]){
                if(countTable[impID] > 0){return impID;}
           }
           std::cerr<<"error in  find essential error, n = "<<n<<std::endl;
           for(auto& impID: reverseCoverTable[n]){
                std::cerr<<impID<<": "<<countTable[impID]<<std::endl;
           }
        }
    }
    // return findLargest();
    return findClosest();
}

void QM::removeNum(uint32_t n){
    for(auto& impID: reverseCoverTable[n]){
        // coverTable[impID].erase(n);
        if(countTable[impID]!=0)countTable[impID];
    }
    
    ElementToImp[n] = 0;
    unCovered.erase(n);
}
void QM::genCoverSet(){
    // std::map<int, std::set<uint32_t>> coverTable;
    std::cout<<"Generating cover set with "<<onSet.size()<<" onSet and "<<primeSet.size()<<" prime implicant"<<std::endl;
    reverseCoverTable.resize(pow(2, nBit) + 1);
    countTable = std::vector<int>((int)primeSet.size());
    ElementToImp = std::vector<int>((int)pow(2, nBit) + 1, 0);
    for(auto& imp: onSet){unCovered.insert(imp.bits);} 
    for(int i = 0; i < (int)primeSet.size(); ++i){
        std::set<uint32_t> coverNum;
        for(auto& n: primeSet[i].containNum){
            coverNum.insert(n);
            reverseCoverTable[n].insert(i);
            ++ElementToImp[n];

        }
        countTable[i] = coverNum.size();
        coverTable.push_back(coverNum);
    }
    std::cout<<primeSet.size()<<"---"<<std::endl;
    while(!unCovered.empty()){
        int impID = findEssential();
        if(impID == -1){std::cerr<<"error in getCoverSet: impID == -1"<<std::endl;exit(-1);}
        coverSet.push_back(primeSet[impID]);
        std::set<uint32_t> t = coverTable[impID];
        // std::cout<<primeSet[impID].getSOP(nBit)<<std::endl;
        if(t.size() <= 0){std::cerr<<"error in t.size(): impID == -1"<<std::endl;exit(-1);}
        countTable[impID] = 0;
        for(auto& n: t){
            // std::cout<<n<<" ";
            removeNum(n);
        }
        // std::cout<<std::endl;
    }

}



void QM::readFile(std::string inputFile){
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Error opening input file!" << std::endl;
        exit(-1);
    }


    inFile>>nBit;

    //read second line(onSet)
    std::string buffer;
    std::getline(inFile, buffer);
    std::getline(inFile, buffer);
    std::istringstream iss(buffer);
    uint32_t num;
    while(iss >> num)
    {
        onSet.push_back(Implicant(num, 0, true));
    }

    //read second line(onSet)
    std::getline(inFile, buffer);
    iss = std::istringstream(buffer);
    while(iss >> num)
    {
       dcSet.push_back(Implicant(num, 0, false));
    }
}
void QM::dump(){
    std::cout<<"onSet:\n";
    for(auto& n:onSet){std::cout<<n.mask<<" ";}
    std::cout<<std::endl;
    std::cout<<"dcSet:\n";
    for(auto& n:dcSet){std::cout<<n.bits<<" ";}
    std::cout<<std::endl;
}
void QM::dumpPrime(std::string outputFile){
    std::ofstream outFile(outputFile);  
    if (!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        exit(-1);
    }

    std::cout<<"start dumping: "<<primeSet.size()<<std::endl;
    std::set<std::string> histPrime;
    for (auto& imp : primeSet) {
        std::string SOP = imp.getSOP(nBit);
        if(histPrime.find(SOP) == histPrime.end()){
            histPrime.insert(SOP);
            outFile<<SOP<<std::endl;
            // std::cout<<SOP<<std::endl;
        }
    }
}
void QM::dumpCover(std::string outputFile){
    std::ofstream outFile(outputFile);  
    if (!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        exit(-1);
    }

    std::cout<<"start dumping: "<<coverSet.size()<<std::endl;
    std::set<std::string> histPrime;
    for (auto& imp : coverSet) {
        std::string SOP = imp.getSOP(nBit);
        if(histPrime.find(SOP) == histPrime.end()){
            histPrime.insert(SOP);
            outFile<<SOP<<std::endl;
            // std::cout<<SOP<<std::endl;
        }
    }
}

void multiQM::readFile(std::string inputFile){
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Error opening input file!" << std::endl;
        exit(-1);
    }


    inFile>>nBit;

    //read second line(onSet)
    std::string buffer;
    std::getline(inFile, buffer);
    std::getline(inFile, buffer);
    std::istringstream iss(buffer);
    uint32_t num;
    while(iss >> num)
    {
        onSet.push_back(Implicant(num, 0, true));
    }

    //read second line(onSet)
    std::getline(inFile, buffer);
    iss = std::istringstream(buffer);
    while(iss >> num)
    {
       dcSet.push_back(Implicant(num, 0, false));
    }
}

std::vector<Implicant> getRandomImplicants(const std::vector<Implicant>& dcSet, int n) {
    std::vector<Implicant> result;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<Implicant> temp = dcSet;
    std::shuffle(temp.begin(), temp.end(), gen);
    result.assign(temp.begin(), temp.begin() + n);
    
    return result;
}
std::pair<std::vector<Implicant>, std::vector<Implicant>> multiQM::job(int onStart, int sepSize){
    int onEnd = std::min(onStart + sepSize, (int)this->onSet.size());
                
    std::vector<Implicant> sepOnSet(this->onSet.begin() + onStart, this->onSet.begin() + onEnd);
    std::vector<Implicant> tempDcSet;
    if(this->dcSet.size() > DCSIZE){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<Implicant> temp = this->dcSet;
        std::shuffle(temp.begin(), temp.end(), gen);
        tempDcSet.assign(temp.begin(), temp.begin() + DCSIZE);
    } else {
        tempDcSet = this->dcSet;
    }
    
    QM qm(this->nBit, sepOnSet, tempDcSet);
    qm.genPrimeSet();
    // qm.genCoverSet();
    
    return std::make_pair(qm.getPrimeSet(), qm.getCoverSet());
}
void multiQM::runParellel(int numSep){
    if((int)onSet.size() + (int)dcSet.size() < TOTALSIZE){
        QM qm(nBit, onSet, dcSet);
        qm.genPrimeSet();
        qm.genCoverSet();
        primeSet = qm.getPrimeSet();
        return ;
    }
    int sepSize = numSep - std::min(DCSIZE, (int)dcSet.size());
    
    std::mutex primeMtx;
    std::mutex coverMtx;
    
   

    std::vector<std::thread> threads;
    for(int onStart = 0; onStart < (int)onSet.size(); onStart += sepSize){
        threads.emplace_back([&, onStart, sepSize]() {
            std::pair<std::vector<Implicant>, std::vector<Implicant>> p= this->job(onStart, sepSize);
            std::vector<Implicant> partPrimeSet = p.first;
            std::vector<Implicant> partCoverSet = p.second;
            {
                std::lock_guard<std::mutex> lock(primeMtx);
                primeSet.insert(primeSet.end(), partPrimeSet.begin(), partPrimeSet.end());
            }
            {
                std::lock_guard<std::mutex> lock(coverMtx);
                coverSet.insert(coverSet.end(), partCoverSet.begin(), partCoverSet.end());
            }
        });
    }
    
    for(auto& thread : threads){
        thread.join();
    }
}
void multiQM::dumpCover(std::string outputFile){
    std::ofstream outFile(outputFile);  
    if (!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        exit(-1);
    }

    std::cout<<"start dumping: "<<coverSet.size()<<std::endl;
    std::set<std::string> histPrime;
    for (auto& imp : coverSet) {
        std::string SOP = imp.getSOP(nBit);
        if(histPrime.find(SOP) == histPrime.end()){
            histPrime.insert(SOP);
            outFile<<SOP<<std::endl;
            // std::cout<<SOP<<std::endl;
        }
    }
}
void multiQM::dumpPrime(std::string outputFile) {
    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        exit(-1);
    }

    std::cout << "start dumping: " << primeSet.size() << std::endl;

    std::unordered_set<std::string> histPrime;
    histPrime.reserve(primeSet.size());
    
    for (auto& imp : primeSet) {
        auto result = histPrime.emplace(imp.getSOP(nBit));
        if (result.second) { 
            outFile << *result.first << '\n'; 
        }
    }
    
    // 確保資料寫入磁碟
    outFile.flush();
}