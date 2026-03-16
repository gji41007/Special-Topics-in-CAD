#include "QM.h"

int main(int argc, char* argv[]){

    if(argc != 3){
        std::cerr<<"Usage: ./sop <input file> <output file>"<<std::endl;
        exit(-1);
    }
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    if(ENABLEMULTIQM){
        multiQM mQM;
        mQM.readFile(inputFile);
        mQM.runParellel(TOTALSIZE);
        mQM.dumpPrime(outputFile);
        // mQM.dumpCover(outputFile);
    }
    else{
        QM qm;
        qm.readFile(inputFile);
        qm.genPrimeSet();
        // qm.genCoverSet();
        qm.dumpPrime(outputFile);
        // qm.dumpCover(outputFile);
    }
    return 0;
}