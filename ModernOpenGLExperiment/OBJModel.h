#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Rendering.h"

namespace OBJ {
    typedef struct {
        int vertexID;
        int normalID;
        int texCoordID;
    } FaceVertex_t;

    typedef struct {
        FaceVertex_t verts[3];
    } Face_t;
};

class OBJModel
{
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> texCoords;

    // List of vertices
    std::vector<OBJ::Face_t> faces;

    OBJModel(std::ifstream& inFile);

public:
    static OBJModel* LoadFromFile(const char* filename);

    ~OBJModel();
};

#endif