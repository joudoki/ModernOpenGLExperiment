#ifndef MD3MODEL_H
#define MD3MODEL_H

#include <cstdint>

#include <iostream>
#include <fstream>
#include <vector>

#include "Rendering.h"
#include "BoundingBox.h"
#include "ModelLoader.h"

#define MD3_MAGIC 860898377 // TODO-MAYBE: Deal with endian-ness
#define MD3_VERSION 15
#define MD3_MAX_QPATH 64

namespace MD3 {
    typedef struct {
        char name[MD3_MAX_QPATH];
        int32_t flags;

        uint32_t numFrames;
        uint32_t numTags;
        uint32_t numSurfaces;
        uint32_t numSkins;

        uint32_t offsetFrames;
        uint32_t offsetTags;
        uint32_t offsetSurfaces;
        uint32_t offsetEOF;
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
        int32_t magic;
        char name[MD3_MAX_QPATH];
        int32_t flags;
    
        uint32_t numFrames;
        uint32_t numShaders;
        uint32_t numVerts;
        uint32_t numTriangles;

        uint32_t offsetTriangles;
        uint32_t offsetShaders;
        uint32_t offsetTexCoords;
        uint32_t offsetVertexCoords;
        uint32_t offsetEnd;
    } Surface_t;

    typedef struct {
        char name[MD3_MAX_QPATH];
        int32_t index;
    } Shader_t;

    typedef struct {
        uint32_t a;
        uint32_t b;
        uint32_t c;
    } Triangle_t;

    typedef struct {
        float u;
        float v;
    } TexCoord_t;

    #define MD3_SCALE (1.0f/64.0f)

    typedef struct {
        int16_t x,y,z;
        uint16_t n;
    } Vertex_t;
};

class MD3Model : public ModelLoader {
private:
    char* buffer;

    // Derived pointers into fileContents
    MD3::Header_t* header;
    std::vector<MD3::Frame_t*> frames;
    std::vector<MD3::Tag_t*> tags;
    std::vector<MD3::Surface_t*> surfaces;

    static glm::vec3 DecodeNormal(uint16_t index);

    MD3Model(std::ifstream& infile);

    // Functions to get pointers for a specific surface/frame

    MD3::Vertex_t* GetVertices(uint32_t s, uint32_t f) const;
    MD3::Shader_t* GetShaders(uint32_t s) const;
    MD3::Triangle_t* GetTriangles(uint32_t s) const;
    MD3::TexCoord_t* GetTexCoords(uint32_t s) const;

public:
    static MD3Model* LoadFromFile(const char* filename);

    ~MD3Model();

    bool IsValid() const { return buffer != NULL; }

    uint32_t GetMeshCount() const { return header->numSurfaces; }

    /**
     * Retrieves from the internal format a list of formatted vertices 
     * of the form MD3Model::Vertex_t.
     * 
     * s - The index of the surface to get the vertices from
     * f - The index of the frame to get the vertices from
     * vertexData  - Where to put the vertices
     * vertexCount - How many vertices there are in vertexData
     */
    void GetVertices(uint32_t s, uint32_t f, MeshVertex_t*& vertexData, uint32_t& vertexCount) const;

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
    
    MD3::Frame_t* GetFrame(uint32_t f);
};

#endif