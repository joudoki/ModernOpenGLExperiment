#ifndef MD3MODEL_H
#define MD3MODEL_H

#include <fstream>
#include <vector>

#include "BoundingBox.h"
#include "Rendering.h"
#include "Mesh.h"

#define MD3_MAGIC "IDP3"
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
        short n;
    } Vertex_t;
};

class MD3Model
{
private:
    std::ifstream modelFile;
    MD3::Header_t header;

    std::vector<MD3::Surface_t> surfaces;
    std::vector<int> surfaceOffsets;

    std::vector<MD3::Frame_t> frames;

    MD3Model(const char* fileName);
    
    void ReadFrames();
    void ReadSurfaces();

    static glm::vec3 DecodeNormal(short index);

public:
    ~MD3Model();

    // Final format of the loading
    typedef struct {
        glm::vec3 coord;
        glm::vec3 normal;
        glm::vec2 texCoord;
    } Vertex_t;

    static MD3Model* LoadFromFile(const char* fileName);

    size_t GetSurfaceCount() const { return header.numSurfaces; }

    void GetVertices(size_t i, Vertex_t*& vertexData, size_t& vertexCount);
    void GetIndices(size_t i, GLushort*& indexData, size_t& triangleCount);
    BoundingBox GetFrame(size_t i);
};

#endif