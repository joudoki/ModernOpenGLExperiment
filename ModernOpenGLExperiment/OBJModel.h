#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <cstdint>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Rendering.h"
#include "ModelLoader.h"

namespace OBJ {
    typedef struct {
        uint32_t vertex, texCoord, normal;
    } FaceVertex_t;

    typedef struct {
        FaceVertex_t verts[3];
    } Face_t;

    typedef struct {
        uint32_t faceBegin;
        uint32_t faceCount;
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

    OBJ::FaceVertex_t ReadFaceVertex(std::istream& infile) const;
    OBJ::Face_t ReadFace(std::istream& infile) const;

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

    uint32_t GetMeshCount() const { return surfaces.size(); }

    /**
     * Retrieves from the internal format a list of formatted vertices 
     * of the form MD3Model::Vertex_t.
     * 
     * s - The index of the surface to get the vertices from
     * f - The index of the frame to get the vertices from
     * vertexData  - Where to put the vertices
     * vertexCount - How many vertices there are in vertexData
     */
    void GetVertices(uint32_t s, MeshVertex_t*& vertexData, uint32_t& vertexCount) const;

    /**
     * Retrieves from the internal format the list of indices making
     * up the primitives needed to render this model.
     *
     * s - The index of the surface to retrieve indices from
     * indexData     - Where to put the indices
     * triangleCount - The number of primitives in indexData
     */
    void GetIndices(uint32_t s, GLushort*& indexData, uint32_t& triangleCount) const;
};

#endif