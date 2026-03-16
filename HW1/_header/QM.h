#ifndef _QM_H_
#define _QM_H_

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <set>

#define ENABLECOUNTPRIME true
#define ENABLEMULTIQM true
#define DCSIZE 100000
#define TOTALSIZE 200000

class Implicant{
public:
    Implicant(uint32_t b, uint32_t m, bool on):bits(b), mask(m), isON(on) {containNum.push_back(b);}
    Implicant(uint32_t b, uint32_t m, bool on, std::vector<uint32_t> c):bits(b), mask(m), isON(on), containNum(c) {}
    uint32_t bits;  //0 or 1
    uint32_t mask;  //1 = dc
    bool isPrime = true;
    int countPrime = 1;
    bool isMerge = false;
    bool isON;
    std::vector<uint32_t> containNum;
    int getOnes(){
        // int count = 0;
        // uint32_t filtered = bits & ~mask;
        // while (filtered) {
        //     count += filtered & 1;  
        //     filtered >>= 1;        
        // }
        return __builtin_popcount(bits);
    }
    int quickDiff(const Implicant& other) const {
        if(mask != other.mask) return -1;
        
        uint32_t diff = (bits ^ other.bits) & ~mask;
        if(__builtin_popcount(diff) != 1) return -1;
        
        return __builtin_ctz(diff); 
    }
    int diffIndex(Implicant comp){
        // std::cout<<"compare\n";
        // std::cout<<std::bitset<32>(bits)<<std::endl;
        // std::cout<<std::bitset<32>(mask)<<std::endl;
        // std::cout<<std::bitset<32>(comp.bits)<<std::endl;
        // std::cout<<std::bitset<32>(comp.mask)<<std::endl;
        if(mask != comp.mask){return -1;}
        uint32_t a = bits & ~mask;
        uint32_t b = comp.bits & ~comp.mask;

        //the same
        uint32_t XOR = a ^ b;
        if (XOR == 0) {return -1;}
        

        // n & (n-1) clear the rightmost 1
        //more than one diff
        if ((XOR & (XOR - 1)) != 0) {return -1;}
        
        //find index of 1
        int position = 0;
        while (XOR > 0) {
            if (XOR & 1) {return position;}
            XOR >>= 1;
            position++;
        }
        
        return -1;  
    }
    void setBits(int index, bool val){
        bits = (bits & ~(1U << index)) | (static_cast<uint32_t>(val) << index);
    }
    void setMask(int index, bool val){
        mask = (mask & ~(1U << index)) | (static_cast<uint32_t>(val) << index);
    }
    std::string getSOP(int nBit){
        std::string SOP;
        for (int i = nBit-1; i >= 0; i--) {
            if (mask & (1 << i)) {SOP += '-';} 
            else {SOP += ((bits >> i) & 1) ? '1' : '0';}
        }
        return SOP;
    }
    bool isCover(uint32_t n){//, int nBit){
        uint32_t diff = (bits ^ n) & ~mask;
        return diff == 0;        
    }
};

// class Column{
// public:
//     uint32_t n;
//     std::vector<int>
// };

class QM{
public:
    QM(){onSet.clear();dcSet.clear();primeSet.clear();}
    QM(int b, std::vector<Implicant> on, std::vector<Implicant> dc): nBit(b), onSet(on), dcSet(dc){}
    void readFile(std::string inputFile);
    void genPrimeSet();
    void genCoverSet();

    void dump();
    void dumpPrime(std::string outputFile);
    void dumpCover(std::string outputFile);
    std::vector<Implicant> getCoverSet(){return coverSet;}
    std::vector<Implicant> getPrimeSet(){return primeSet;}

private:
    int nBit = -1; 
    std::vector<Implicant> onSet;
    std::vector<Implicant> dcSet;

    // std::vector<std::vector<Implicant>> implicationTable;
    std::vector<Implicant> primeSet;



    //for column covering
    std::vector<Implicant> coverSet;
    std::vector<std::set<uint32_t>> coverTable;
    std::vector<std::set<int>> reverseCoverTable;
    std::vector<int> countTable;
    std::vector<int> ElementToImp;
    std::set<uint32_t> unCovered;
    std::vector<bool> isSelected;

    int findEssential();
    int findLargest();
    int findClosest();
    void removeNum(uint32_t n);
};

class multiQM{
public:
    void readFile(std::string inputFile);
    void runParellel(int numSep);
    void dumpPrime(std::string outputFile);
    void dumpCover(std::string outputFile);
    std::pair<std::vector<Implicant>, std::vector<Implicant>> job(int onStart, int sepSize);
private:
    int nBit = -1; 
    std::vector<Implicant> onSet;
    std::vector<Implicant> dcSet;
    std::vector<Implicant> primeSet;
    std::vector<Implicant> coverSet;
};

#endif