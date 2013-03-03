#include "MD3Model.h"

#define LATLONSCALE (360.0f/255.0f)

MD3Model::MD3Model(const char* fileName) : modelFile(fileName) {

};

MD3Model::~MD3Model() { 
    modelFile.close();
};

// Decodes an spherical coordinate system encoded normal
// http://www.icculus.org/homepages/phaethon/q3a/formats/md3format.html#Normals
// Format for a normal is as follows:
// [0..7]  (8 bits) longitude
// [8..15] (8 bits) latitude
// Values should be expected to fill whole range [0,255]
glm::vec3 MD3Model::DecodeNormal(short normal) {
    float lat = LATLONSCALE * ((normal >> 8) && 255);
    float lon = LATLONSCALE * (normal && 255);

    return glm::vec3(
        glm::cos(lat) * glm::sin(lon),
        glm::sin(lat) * glm::sin(lon),
        glm::cos(lon)
    );
}

MD3Model* MD3Model::LoadFromFile(const char* fileName) {
    MD3Model* model = new MD3Model(fileName);
    
    if (!model->modelFile) {
        delete model;
        throw errno;
    }
    
    // Check magic and version
    int version;
    char magic[5] = {0};

    model->modelFile.read(magic, 4*sizeof(char));
    model->modelFile.read((char*)&version, sizeof(int));

    if (strcmp(magic, MD3_MAGIC) != 0 || version != MD3_VERSION) {
        printf("Got unexpected file format \"%s\" v%d, expecting \"%s\" v%d\n", magic, version, MD3_MAGIC, MD3_VERSION);
        delete model;
        return NULL;
    }

    // Get the header
    model->modelFile.read((char*)&model->header, sizeof(MD3::Header_t));

    // Read surfaces
    model->ReadSurfaces();
    
    // Read frames
    model->ReadFrames();

    return model;
}

void MD3Model::ReadFrames() {
    // Frames are of fixed length and are sequential
    modelFile.seekg(header.offsetFrames);

    for (int i=0; i<header.numFrames; ++i) {
        MD3::Frame_t frame;
        modelFile.read((char*)&frame, sizeof(MD3::Frame_t));

        frames.push_back(frame);
    }
}

void MD3Model::ReadSurfaces() {
    int surfaceOffset = header.offsetSurfaces;

    for (int s=0; s<header.numSurfaces; ++s) {
        modelFile.seekg(surfaceOffset);

        MD3::Surface_t surface;
        modelFile.read((char*)&surface, sizeof(MD3::Surface_t));

        surfaces.push_back(surface);
        surfaceOffsets.push_back(surfaceOffset);

        surfaceOffset += surface.offsetEnd;
    }
}

void MD3Model::GetVertices(size_t i, Vertex_t*& vertexData, size_t& vertexCount) {
    assert(i < header.numSurfaces);

    MD3::Surface_t surface = surfaces[i];
    int surfaceOffset = surfaceOffsets[i];

    vertexData = new Vertex_t[surface.numVerts];
    vertexCount = surface.numVerts;

    // Read vertices
    modelFile.seekg(surfaceOffset + surface.offsetVertexCoords);

    for (int i=0; i<surface.numVerts; ++i) {
        MD3::Vertex_t vertex;
        modelFile.read((char*)&vertex, sizeof(MD3::Vertex_t));

        vertexData[i].coord = glm::vec3(
            vertex.x * MD3_SCALE,
            vertex.y * MD3_SCALE,
            vertex.z * MD3_SCALE
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
    }

    /*printf("[%d] (%f, %f, %f) (%f, %f, %f) (%f, %f)\n", i,
        vertices[i].coord.x,    vertices[i].coord.y,    vertices[i].coord.z,
        vertices[i].normal.x,   vertices[i].normal.y,   vertices[i].normal.z,
        vertices[i].texCoord.x, vertices[i].texCoord.y
    );*/
}

void MD3Model::GetIndices(size_t i, GLushort*& indexData, size_t& triangleCount) {
    assert(i < header.numSurfaces);

    MD3::Surface_t surface = surfaces[i];
    int surfaceOffset = surfaceOffsets[i];
    
    indexData = new GLushort[surface.numTriangles*3];
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
}

BoundingBox MD3Model::GetFrame(size_t i) {
    assert(i < header.numFrames);

    MD3::Frame_t frame = frames[i];
    return BoundingBox(frame.minBounds, frame.maxBounds);
}