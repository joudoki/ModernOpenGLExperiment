#include "MD3ModelLoader.h"

#define LATLONSCALE (360.0f/255.0f)

// Decodes an spherical coordinate system encoded normal
// http://www.icculus.org/homepages/phaethon/q3a/formats/md3format.html#Normals
// Format for a normal is as follows:
// [0..7]  (8 bits) longitude
// [8..15] (8 bits) latitude
// Values should be expected to fill whole range [0,255]
glm::vec3 MD3ModelLoader::DecodeNormal(short normal) {
    float lat = LATLONSCALE * ((normal >> 8) && 255);
    float lon = LATLONSCALE * (normal && 255);

    return glm::vec3(
        glm::cos(lat) * glm::sin(lon),
        glm::sin(lat) * glm::sin(lon),
        glm::cos(lon)
    );
}

bool MD3ModelLoader::LoadFromFile(
    const char* fileName,
    Vertex_t*& vertexData, int& vertexCount,
    GLushort*& indexData, int& triangleCount
) {
    std::ifstream modelFile(fileName, std::ios::binary);
    if (!modelFile) throw (errno);
    
    // Check magic and version
    int version;
    char magic[5] = {0};

    modelFile.read(magic, 4*sizeof(char));
    modelFile.read((char*)&version, sizeof(int));

    if (strcmp(magic, MD3_MAGIC) != 0 || version != MD3_VERSION) {
        printf("Got unexpected file format \"%s\" v%d, expecting \"%s\" v%d\n", magic, version, MD3_MAGIC, MD3_VERSION);
        return false;
    }

    MD3::Header_t header;
    modelFile.read((char*)&header, sizeof(MD3::Header_t));

    // Just read the first surface for now
    int surfaceOffset = header.offsetSurfaces;
    //for (int s=0; s<header.numSurfaces; ++s) {
        modelFile.seekg(surfaceOffset);

        MD3::Surface_t surface;
        modelFile.read((char*)&surface, sizeof(MD3::Surface_t));

        vertexData = new Vertex_t[surface.numVerts];
        indexData = new GLushort[surface.numTriangles*3];

        vertexCount = surface.numVerts;
        triangleCount = surface.numTriangles;
        
        // Read triangles
        modelFile.seekg(surfaceOffset + surface.offsetTriangles);

        for (int t=0; t<surface.numTriangles; ++t) {
            MD3::Triangle_t triangle;
            modelFile.read((char*)&triangle, sizeof(MD3::Triangle_t));

            // TODO: Dump these directly into indices instead of copying
            indexData[3*t]   = triangle.a;
            indexData[3*t+1] = triangle.b;
            indexData[3*t+2] = triangle.c;
        }

        /*
        // Read shaders
        modelFile.seekg(surfaceOffset + surface.offsetShaders);

        for (int i=0; i<surface.numShaders; ++i) {
            MD3::Shader_t shader;
            modelFile.read((char*)&shader, sizeof(MD3::Shader_t));
        }
        */

        // Read vertices
        modelFile.seekg(surfaceOffset + surface.offsetVertexCoords);

        for (int i=0; i<surface.numVerts; ++i) {
            MD3::Vertex_t vertex;
            modelFile.read((char*)&vertex, sizeof(MD3::Vertex_t));

            vertexData[i].coord = glm::vec3(
                vertex.x * MD3_SCALE,
                vertex.y * MD3_SCALE,
                vertex.z  *MD3_SCALE
            );

            vertexData[i].normal = DecodeNormal(vertex.n);
        }

        // Read Texture coordinates
        modelFile.seekg(surfaceOffset + surface.offsetTexCoords);

        for (int i=0; i<surface.numVerts; ++i) {
            MD3::TexCoord_t texCoord;
            modelFile.read((char*)&texCoord, sizeof(MD3::TexCoord_t));

            vertexData[i].texCoord = glm::vec2(
                texCoord.u,
                texCoord.v
            );

            /*printf("[%d] (%f, %f, %f) (%f, %f, %f) (%f, %f)\n", i,
                vertices[i].coord.x,    vertices[i].coord.y,    vertices[i].coord.z,
                vertices[i].normal.x,   vertices[i].normal.y,   vertices[i].normal.z,
                vertices[i].texCoord.x, vertices[i].texCoord.y
            );*/
        }

    //    surfaceOffset += surface.offsetEnd;
    //}

    modelFile.close();

    return true;
}