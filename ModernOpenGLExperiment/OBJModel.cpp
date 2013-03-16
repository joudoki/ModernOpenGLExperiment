#include "OBJModel.h"

OBJModel::OBJModel(std::ifstream& infile) {
    std::string line;
    while (infile) {
        std::getline(infile, line);

        // Grab the command from the line
        // This will be (should be) one of v, vt, vn, #
        //std::string cmd = line.substr(

        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == 0)
            continue;

        std::cout << line << std::endl;


        // v vt vn f
    }
}

OBJModel::~OBJModel() {
}

OBJModel* OBJModel::LoadFromFile(const char* filename) {
    std::ifstream infile(filename);
    if (!infile) throw errno;

    OBJModel* model = new OBJModel(infile);

    infile.close();

    return model;
}