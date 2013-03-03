#include "MD3ModelLoader.h"

Mesh* MD3ModelLoader::LoadFromFile(const char* fileName) {
    std::ifstream modelFile(fileName, std::ios::binary);
    if (!modelFile) throw (errno);
    
    // Check magic and version
    int version;
    char magic[5] = {0};

    modelFile.read(magic, 4*sizeof(char));
    modelFile.read((char*)&version, sizeof(int));

    if (strcmp(magic, MD3_MAGIC) != 0 || version != MD3_VERSION) {
        printf("Got unexpected file format \"%s\" v%d, expecting \"%s\" v%d\n", magic, version, MD3_MAGIC, MD3_VERSION);
        return NULL;
    }

    MD3::Header_t header;
    modelFile.read((char*)&header, sizeof(MD3::Header_t));

    int surfaceOffset = header.offsetSurfaces;
    for (int s=0; s<header.numSurfaces; ++s) {
        modelFile.seekg(surfaceOffset);

        MD3::Surface_t surface;
        modelFile.read((char*)&surface, sizeof(MD3::Surface_t));
        
        // Read triangles
        modelFile.seekg(surfaceOffset + surface.offsetTriangles);

        for (int t=0; t<surface.numTriangles; ++t) {
            MD3::Triangle_t triangle;
            modelFile.read((char*)&triangle, sizeof(MD3::Triangle_t));
            printf("[%d] (%d, %d, %d)\n", t, triangle.a, triangle.b, triangle.c);
        }

        // Read shaders
        modelFile.seekg(surfaceOffset + surface.offsetShaders);

        for (int i=0; i<surface.numShaders; ++i) {
            MD3::Shader_t shader;
            modelFile.read((char*)&shader, sizeof(MD3::Shader_t));
        }

        // Read vertices
        modelFile.seekg(surfaceOffset + surface.offsetVertexCoords);

        for (int i=0; i<surface.numVerts; ++i) {
            MD3::Vertex_t vertex;
            modelFile.read((char*)&vertex, sizeof(MD3::Vertex_t));
            printf("[%d] (%d, %d, %d) %d\n", i, vertex.x, vertex.y, vertex.z, vertex.n);
        }

        // Read Texture coordinates
        modelFile.seekg(surfaceOffset + surface.offsetTexCoords);

        for (int i=0; i<surface.numVerts; ++i) {
            MD3::TexCoord_t texCoord;
            modelFile.read((char*)&texCoord, sizeof(MD3::TexCoord_t));
        }

        surfaceOffset += surface.offsetEnd;
    }

    modelFile.close();

    return NULL;
}