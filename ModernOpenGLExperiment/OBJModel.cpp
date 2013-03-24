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

OBJ::FaceVertex_t OBJModel::ReadFaceVertex(std::istream& infile) const {
    OBJ::FaceVertex_t fv;

    // Skip '/' between components of vector
    char c;

    infile 
        >> fv.vertex >> c
        >> fv.texCoord >> c
        >> fv.normal;

    // Decrement the indices because they're 1-based
    --fv.vertex;
    --fv.texCoord;
    --fv.normal;

    return fv;
}

OBJ::Face_t OBJModel::ReadFace(std::istream& infile) const {
    OBJ::Face_t face;

    for (int i=0; i<3; ++i)
        face.verts[i] = ReadFaceVertex(infile);

    return face;
}

OBJModel::OBJModel(std::istream& infile) {
    OBJ::Surface_t surface = {0, 0, "Default"};
    
    std::string line;
    while (infile) {
        std::getline(infile, line);
        std::istringstream iss(line);

        if (!iss)
            continue;
        
        std::string cmd;
        iss >> cmd;

        // Skip comments
        if (cmd.size() == 0 || cmd[0] == '#')
            continue;

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
            if (surface.faceCount > 0)
                surfaces.push_back(surface);

            iss >> surface.name;
            surface.faceBegin += surface.faceCount;
            surface.faceCount = 0;
        } else if (cmd == "s") {
            // Smoothing Group s
            //int surfaceNum;
            //iss >> surfaceNum;
            //std::cout << surfaceNum;
        } else if (cmd == "f") {
            // Face v/vt/vn v/vt/vn v/vt/vn
            ++surface.faceCount;
            faces.push_back(ReadFace(iss));
        }
    }

    if (surface.faceCount > 0)
        surfaces.push_back(surface);
}

OBJModel::~OBJModel() {}

OBJModel* OBJModel::LoadFromFile(const char* filename) {
    std::ifstream infile(filename);
    if (!infile) throw errno;

    OBJModel* model = new OBJModel(infile);

    infile.close();

    return model;
}

void OBJModel::GetVertices(uint32_t s, MeshVertex_t*& vertexData, uint32_t& vertexCount) const {
    assert(s < surfaces.size());

    OBJ::Surface_t surface = surfaces[s];

    vertexCount = 3*surface.faceCount;
    vertexData = new MeshVertex_t[vertexCount];

    int v;
    for (uint32_t i=0; i<surface.faceCount; ++i) {
        const OBJ::Face_t& face = faces[surface.faceBegin + i];

        for (uint32_t j=0; j<3; ++j) {
            const OBJ::FaceVertex_t& faceVert = face.verts[j];

            // Calculate vertex index
            v = 3*i + j;
            
            // Faces are laid out in v/vt/vn order
            vertexData[v].coord    =  vertices[faceVert.vertex];
            vertexData[v].texCoord = texCoords[faceVert.texCoord];
            vertexData[v].normal   =   normals[faceVert.normal];
        }
    }
}

void OBJModel::GetIndices(uint32_t s, GLushort*& indexData, uint32_t& triangleCount) const {
    assert(s < surfaces.size());

    OBJ::Surface_t surface = surfaces[s];

    triangleCount = surface.faceCount;
    indexData = new GLushort[3*triangleCount];

    for (size_t i=0; i<3*triangleCount; ++i)
        indexData[i] = i;
}