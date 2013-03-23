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

        glm::vec3 vec3;
        glm::vec2 vec2;

        if (cmd == "v") {
            // Vertex (x,y,z)
            vertices.push_back(ReadVec3(iss));
        } else if (cmd == "vn") {
            // Vertex Normal (x,y,z)
            normals.push_back(ReadVec3(iss));
        } else if (cmd == "vt") {
            // Vertex Texture Coordinate (u,v)
            texCoords.push_back(ReadVec2(iss));
        } else if (cmd == "g") {
            // Object (name)
            // Do nothing for now
            // iss >> cmd;
        } else if (cmd == "s") {
            // Smoothing Group s
            //int surfaceNum;
            //iss >> surfaceNum;
            //std::cout << surfaceNum;
        } else if (cmd == "f") {
            // Face v/vt/vn v/vt/vn v/vt/vn

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

            faces.push_back(face);
        }
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

void OBJModel::GetVertices(size_t s, MeshVertex_t*& vertexData, size_t& vertexCount) {
    vertexCount = 3*faces.size();
    vertexData = new MeshVertex_t[vertexCount];

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