#ifndef MD3MODELLOADER_H
#define MD3MODELLOADER_H

#include <fstream>

#include "Rendering.h"
#include "Mesh.h"

#define MD3_MAGIC "IDP3"
#define MD3_VERSION 15
#define MD3_MAX_QPATH 64

namespace MD3 {
    typedef struct {
        char name[MD3_MAX_QPATH];
        int flags;

        int numFrmes;
        int numTag;
        int numSurfaces;
        int numSkins;

        int offsetFrames;
        int offsetTags;
        int offsetSurfaces;
        int offsetEOF;
    } Header_t;

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

    typedef struct {
        float x;
        float y;
        float z;

        float nx;
        float ny;
        float nz;

        float tu;
        float tv;
    } MD3Vertex_t;
};

class MD3ModelLoader
{
private:
    MD3ModelLoader() {};
    ~MD3ModelLoader() {};

public:
    static Mesh* LoadFromFile(const char* fileName);
};

#endif