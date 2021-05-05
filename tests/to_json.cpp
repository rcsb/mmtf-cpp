// *************************************************************************
//
// Simple test for compilation issues.
//
// *************************************************************************

#include <mmtf.hpp>

int main(int argc, char** argv) {
    // decode MMTF file
    std::string const filename(argv[1]);
    mmtf::StructureData data;
    mmtf::decodeFromFile(data, filename);
    std::cout << data.to_json() << std::endl;

    return 0;
}
