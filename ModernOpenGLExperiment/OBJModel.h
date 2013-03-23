#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Rendering.h"
#include "ModelLoader.h"

namespace OBJ {
    typedef struct {
        size_t vertex, texCoord, normal;
    } FaceVertex_t;

    typedef struct {
        FaceVertex_t verts[3];

        // Take advantage of the layout of the struct in memory
        // to allow indexing inside for easy parsing
        size_t& operator[](size_t i) { 
            assert(i < 9);
            return *(reinterpret_cast<size_t*>(this) + i); 
        };
    } Face_t;

    typedef struct {
        size_t faceBegin;
        size_t faceCount;
        std::string name;
    } Surface_t;
};

class OBJModel : public ModelLoader {
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    // List of vertices
    std::vector<OBJ::Face_t> faces;

    // List of offsets into faces for each surface
    std::vector<OBJ::Surface_t> surfaces;

    // Helper loading functions
    glm::vec3 ReadVec3(std::istream& infile) const;
    glm::vec2 ReadVec2(std::istream& infile) const;

    OBJModel(std::istream& inFile);

public:
    ~OBJModel();

    static OBJModel* LoadFromFile(const char* filename);
    
    bool IsValid() const { 
        return 
            vertices.size()     > 0 && 
            normals.size()      > 0 && 
            texCoords.size()    > 0 && 
            faces.size()        > 0;
    };

    size_t GetMeshCount() const { return surfaces.size(); }

    /**
     * Retrieves from the internal format a list of formatted vertices 
     * of the form MD3Model::Vertex_t.
     * 
     * s - The index of the surface to get the vertices from
     * f - The index of the frame to get the vertices from
     * vertexData  - Where to put the vertices
     * vertexCount - How many vertices there are in vertexData
     */
    void GetVertices(size_t s, MeshVertex_t*& vertexData, size_t& vertexCount) const;

    /**
     * Retrieves from the internal format the list of indices making
     * up the primitives needed to render this model.
     *
     * s - The index of the surface to retrieve indices from
     * indexData     - Where to put the indices
     * triangleCount - The number of primitives in indexData
     */
    void GetIndices(size_t s, GLushort*& indexData, size_t& triangleCount) const;
};

#endif