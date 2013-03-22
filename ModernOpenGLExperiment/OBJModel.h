#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Rendering.h"
#include "MD3Model.h"

namespace OBJ {
    typedef struct {
        int ids[9];
    } Face_t;
};

class OBJModel
{
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    // List of vertices
    std::vector<OBJ::Face_t> faces;

    // Helper loading functions
    glm::vec3 ReadVec3(std::istream& infile) const;
    glm::vec2 ReadVec2(std::istream& infile) const;

    OBJModel(std::ifstream& inFile);

public:
    ~OBJModel();

    static OBJModel* LoadFromFile(const char* filename);
    
    size_t GetSurfaceCount() const { return 1; }

    /**
     * Retrieves from the internal format a list of formatted vertices 
     * of the form MD3Model::Vertex_t.
     * 
     * s - The index of the surface to get the vertices from
     * f - The index of the frame to get the vertices from
     * vertexData  - Where to put the vertices
     * vertexCount - How many vertices there are in vertexData
     */
    void GetVertices(size_t s, MD3Model::Vertex_t*& vertexData, size_t& vertexCount);

    /**
     * Retrieves from the internal format the list of indices making
     * up the primitives needed to render this model.
     *
     * s - The index of the surface to retrieve indices from
     * indexData     - Where to put the indices
     * triangleCount - The number of primitives in indexData
     */
    void GetIndices(size_t s, GLushort*& indexData, size_t& triangleCount);
};

#endif