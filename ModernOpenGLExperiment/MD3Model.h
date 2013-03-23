#ifndef MD3MODEL_H
#define MD3MODEL_H

#include <iostream>
#include <fstream>
#include <vector>

#include "BoundingBox.h"
#include "Rendering.h"
#include "Mesh.h"

#define MD3_MAGIC 860898377
#define MD3_VERSION 15
#define MD3_MAX_QPATH 64

namespace MD3 {
    typedef struct {
        char name[MD3_MAX_QPATH];
        int flags;

        int numFrames;
        int numTags;
        int numSurfaces;
        int numSkins;

        int offsetFrames;
        int offsetTags;
        int offsetSurfaces;
        int offsetEOF;
    } Header_t;

    typedef struct {
        glm::vec3 minBounds;
        glm::vec3 maxBounds;
        glm::vec3 localOrigin;
        float radius;
        char name[16];
    } Frame_t;

    typedef struct {
        char name[MD3_MAX_QPATH];
        glm::vec3 origin;
        glm::vec3 axis[3];
    } Tag_t;

    typedef struct {
        int magic;
        char name[MD3_MAX_QPATH];
        int flags;
    
        int numFrames;
        int numShaders;
        int numVerts;
        int numTriangles;

        int offsetTriangles;
        int offsetShaders;
        int offsetTexCoords;
        int offsetVertexCoords;
        int offsetEnd;
    } Surface_t;

    typedef struct {
        char name[MD3_MAX_QPATH];
        int index;
    } Shader_t;

    typedef struct {
        int a;
        int b;
        int c;
    } Triangle_t;

    typedef struct {
        float u;
        float v;
    } TexCoord_t;

    #define MD3_SCALE (1.0f/64.0f)

    typedef struct {
        short x;
        short y;
        short z;
        unsigned short n;
    } Vertex_t;
};

class MD3Model {
private:
    char* buffer;

    // Derived pointers into fileContents
    MD3::Header_t* header;
    std::vector<MD3::Frame_t*> frames;
    std::vector<MD3::Tag_t*> tags;
    std::vector<MD3::Surface_t*> surfaces;

    static glm::vec3 DecodeNormal(GLushort index);

    MD3Model(std::ifstream& infile);

    // Functions to get pointers for a specific surface/frame

    MD3::Vertex_t* GetVertices(size_t s, size_t f) const;
    MD3::Shader_t* GetShaders(size_t s) const;
    MD3::Triangle_t* GetTriangles(size_t s) const;
    MD3::TexCoord_t* GetTexCoords(size_t s) const;

public:
    static MD3Model* LoadFromFile(const char* filename);

    ~MD3Model();

    size_t GetSurfaceCount() const { return header->numSurfaces; }

    /**
     * Retrieves from the internal format a list of formatted vertices 
     * of the form MD3Model::Vertex_t.
     * 
     * s - The index of the surface to get the vertices from
     * f - The index of the frame to get the vertices from
     * vertexData  - Where to put the vertices
     * vertexCount - How many vertices there are in vertexData
     */
    void GetVertices(size_t s, size_t f, MeshVertex_t*& vertexData, size_t& vertexCount);

    /**
     * Retrieves from the internal format the list of indices making
     * up the primitives needed to render this model.
     *
     * s - The index of the surface to retrieve indices from
     * indexData     - Where to put the indices
     * triangleCount - The number of primitives in indexData
     */
    void GetIndices(size_t s, GLushort*& indexData, size_t& triangleCount);
    
    MD3::Frame_t* GetFrame(size_t f);
};

#endif