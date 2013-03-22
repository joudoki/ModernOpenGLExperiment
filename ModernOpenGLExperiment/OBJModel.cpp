#include "OBJModel.h"

glm::vec3 OBJModel::ReadVec3(std::istream& infile) const {
    float x,y,z;
    infile >> x >> y >> z;
    return glm::vec3(x,y,z);
}

glm::vec2 OBJModel::ReadVec2(std::istream& infile) const {
    float x,y;
    infile >> x >> y;
    return glm::vec2(x,y);
}

OBJModel::OBJModel(std::ifstream& infile) {
    std::string line;

    while (infile) {
        std::getline(infile, line);
        std::istringstream iss(line);

        if (!iss)
            continue;
        
        std::string cmd;
        iss >> cmd;

        if (cmd.size() == 0 || cmd[0] == '#')
            continue;

        // Parse rest of command
        std::cout << cmd << " ";

        glm::vec3 vec3;
        glm::vec2 vec2;

        if (cmd == "v") {
            vertices.push_back(ReadVec3(iss));

            vec3 = *(vertices.end()-1);
            std::cout << vec3[0] << " " << vec3[1] << " " << vec3[2];
        } else if (cmd == "vn") {
            normals.push_back(ReadVec3(iss));

            vec3 = *(normals.end()-1);
            std::cout << vec3[0] << " " << vec3[1] << " " << vec3[2];
        } else if (cmd == "vt") {
            texCoords.push_back(ReadVec2(iss));

            vec2 = *(texCoords.end()-1);
            std::cout << vec2[0] << " " << vec2[1];
        } else if (cmd == "g") {
            iss >> cmd;
            std::cout << cmd;
        } else if (cmd == "s") {
            int surfaceNum;
            iss >> surfaceNum;
            std::cout << surfaceNum;
        } else if (cmd == "f") {
            OBJ::Face_t face;

            int index;
            for (int i=0; i<3; ++i) {
                for (int j=0; j<3; ++j) {
                    if (j != 0)
                        iss.get(); // skip /
                    iss >> index;
                    face.ids[3*i+j] = index - 1;
                }
            }

            std::cout << face.ids[0] << '/' << face.ids[1] << '/' << face.ids[2] << ' '
                      << face.ids[3] << '/' << face.ids[4] << '/' << face.ids[5] << ' '
                      << face.ids[6] << '/' << face.ids[7] << '/' << face.ids[8];

            faces.push_back(face);
        } else {
            // ????
            std::cout << "???";
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

        // Faces are laid out in v/vt/vn order
        vertexData[v].coord    =  vertices[faces[f].ids[3*fv]];
        vertexData[v].texCoord = texCoords[faces[f].ids[3*fv+1]];
        vertexData[v].normal   =   normals[faces[f].ids[3*fv+2]];
    }
}

void OBJModel::GetIndices(size_t s, GLushort*& indexData, size_t& triangleCount) {
    triangleCount = faces.size();
    indexData = new GLushort[3*triangleCount];

    for (int i=0; i<3*triangleCount; ++i)
        indexData[i] = i;
}