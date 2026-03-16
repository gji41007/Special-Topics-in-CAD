#include "STA.h"

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <netlist_file> -l <lib_file> -i <input_patterns_file>" << std::endl;
        return 1;
    }
    std::string netlistFile = argv[1];
    std::string libFile = ((std::string)argv[2] == "-l")?argv[3]:argv[5];
    std::string patternFile = ((std::string)argv[2] == "-i")?argv[3]:argv[5];
    // std::cout<<argv[2]<<" "<<libFile<<" "<<patternFile;

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    
    STA sta(netlistFile, libFile, patternFile);
    sta.run();
}