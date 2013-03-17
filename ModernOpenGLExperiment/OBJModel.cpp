#include "OBJModel.h"

glm::vec3 OBJModel::ReadVec3(std::ifstream& infile) const {
    float x,y,z;
    infile >> x >> y >> z;
    return glm::vec3(x,y,z);
}

glm::vec2 OBJModel::ReadVec2(std::ifstream& infile) const {
    float x,y;
    infile >> x >> y;
    return glm::vec2(x,y);
}

OBJModel::OBJModel(std::ifstream& infile) {
    while (infile) {
        std::string cmd;
        infile >> cmd;

        // Skip rest of the line
        if (cmd[0] == '#') {
            std::getline(infile, cmd);
            continue;
        }

        // Parse rest of command
        std::cout << cmd << " ";

        if (cmd == "v") {
            vertices.push_back(ReadVec3(infile));
            glm::vec3 vec = *(vertices.end()-1);
            std::cout << vec[0] << " " << vec[1] << " " << vec[2];
        } else if (cmd == "vn") {
            normals.push_back(ReadVec3(infile));
            glm::vec3 vec = *(normals.end()-1);
            std::cout << vec[0] << " " << vec[1] << " " << vec[2];
        } else if (cmd == "vt") {
            texCoords.push_back(ReadVec2(infile));
            glm::vec2 vec = *(texCoords.end()-1);
            std::cout << vec[0] << " " << vec[1];
        } else if (cmd == "g") {
            infile >> cmd;
            std::cout << cmd;
        } else if (cmd == "s") {
            int surfaceNum;
            infile >> surfaceNum;
            std::cout << surfaceNum;
        } else if (cmd == "f") {
            OBJ::Face_t face;

            int index;
            for (int i=0; i<3; ++i) {
                for (int j=0; j<3; ++j) {
                    if (j != 0)
                        infile.get(); // skip /
                    infile >> index;
                    face.ids[3*i+j] = index - 1;
                }
            }

            faces.push_back(face);
        } else {
            // ????
        }

        std::cout << std::endl;
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

void OBJModel::GetVertices(size_t s, MD3Model::Vertex_t*& vertexData, size_t& vertexCount) {
    vertexCount = 3*faces.size();
    vertexData = new MD3Model::Vertex_t[vertexCount];

    // i - the vertex number
    for (int v=0; v<vertexCount; ++v) {
        int f  = v/3; // face
        int fv = v%3; // face vertex

        vertexData[v].coord    =  vertices[faces[f].ids[3*fv]];
        vertexData[v].normal   =   normals[faces[f].ids[3*fv+1]];
        vertexData[v].texCoord = texCoords[faces[f].ids[3*fv+2]];
    }
}

void OBJModel::GetIndices(size_t s, GLushort*& indexData, size_t& triangleCount) {

}