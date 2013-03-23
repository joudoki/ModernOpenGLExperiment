#include "MD3Model.h"

#define LATLONSCALE (3.141926f/128.0f)

// Decodes an spherical coordinate system encoded normal
// http://www.icculus.org/homepages/phaethon/q3a/formats/md3format.html#Normals
// Format for a normal is as follows:
// [0..7]  (8 bits) longitude
// [8..15] (8 bits) latitude
// Values should be expected to fill whole range [0,255]
glm::vec3 MD3Model::DecodeNormal(GLushort normal) {
    float lat = LATLONSCALE * (float)((normal >> 8u) & 0xFF);
    float lng = LATLONSCALE * (float)(normal         & 0xFF);

    return glm::vec3(
        glm::cos(lat) * glm::sin(lng),
        glm::sin(lat) * glm::sin(lng),
                        glm::cos(lng)
    );
}

MD3Model::MD3Model(std::ifstream& infile) : buffer(NULL), header(NULL) {
    // Determine length of file
    infile.seekg(0, std::ifstream::end);
    size_t length = infile.tellg();
    infile.seekg(0);

    // Read whole file into buffer
    buffer = new char[length];
    infile.read(buffer, length);

    // Extract dependent pointers from headers
    header = reinterpret_cast<MD3::Header_t*>(buffer + 8);

    for (int i=0; i<header->numFrames; ++i) {
        frames.push_back(reinterpret_cast<MD3::Frame_t*>(buffer + header->offsetFrames + i * sizeof(MD3::Frame_t)));
    }

    for (int i=0; i<header->numTags; ++i) {
        tags.push_back(reinterpret_cast<MD3::Tag_t*>(buffer + header->offsetTags + i * sizeof(MD3::Tag_t)));
    }

    int surfaceOffset = header->offsetSurfaces;
    for (int i=0; i<header->numSurfaces; ++i) {
        MD3::Surface_t* surface = reinterpret_cast<MD3::Surface_t*>(buffer + surfaceOffset);
        surfaceOffset += surface->offsetEnd;
        surfaces.push_back(surface);
    }
};

MD3Model::~MD3Model() {
    delete[] buffer;
};

MD3Model* MD3Model::LoadFromFile(const char* filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) throw errno;

    int magic, version;
    infile.read(reinterpret_cast<char*>(&magic),   sizeof(int));
    infile.read(reinterpret_cast<char*>(&version), sizeof(int));

    if (magic != MD3_MAGIC || version != MD3_VERSION) {
        infile.close();
        
        fprintf(stderr, "Got unexpected file format %d v%d, expecting %d v%d\n", 
            magic, version, MD3_MAGIC, MD3_VERSION);

        return NULL;
    }

    MD3Model* model = new MD3Model(infile);
    infile.close();

    return model;
}

MD3::Vertex_t* MD3Model::GetVertices(size_t s, size_t f) const {
    MD3::Surface_t* surface = surfaces[s];
    return reinterpret_cast<MD3::Vertex_t*>(
        reinterpret_cast<char*>(surface) +
        surface->offsetVertexCoords + 
        f * surface->numVerts * sizeof(MD3::Vertex_t)
    );
}

MD3::Shader_t* MD3Model::GetShaders(size_t s) const {
    MD3::Surface_t* surface = surfaces[s];
    return reinterpret_cast<MD3::Shader_t*>(
        reinterpret_cast<char*>(surface) +
        surface->offsetShaders
    );
}

MD3::Triangle_t* MD3Model::GetTriangles(size_t s) const {
    MD3::Surface_t* surface = surfaces[s];
    return reinterpret_cast<MD3::Triangle_t*>(
        reinterpret_cast<char*>(surface) +
        surface->offsetTriangles
    );
}

MD3::TexCoord_t* MD3Model::GetTexCoords(size_t s) const {
    MD3::Surface_t* surface = surfaces[s];
    return reinterpret_cast<MD3::TexCoord_t*>(
        reinterpret_cast<char*>(surface) +
        surface->offsetTexCoords
    );
}

void MD3Model::GetVertices(size_t s, size_t f, MeshVertex_t*& vertexData, size_t& vertexCount) const {
    assert(s < header->numSurfaces);
    assert(f < header->numFrames);
    
    MD3::Surface_t*    surface = surfaces[s];
    MD3::Vertex_t*    vertices = GetVertices(s, f);
    MD3::TexCoord_t* texCoords = GetTexCoords(s);
    
    vertexCount = surface->numVerts;
    vertexData  = new MeshVertex_t[vertexCount];

    for (int i=0; i<vertexCount; ++i) {
        vertexData[i].coord = glm::vec3(
            vertices[i].x,
            vertices[i].y,
            vertices[i].z
        ) * MD3_SCALE;

        vertexData[i].normal = DecodeNormal(vertices[i].n);

        vertexData[i].texCoord = glm::vec2(
            texCoords[i].u,
            texCoords[i].v
        );
    }
}

void MD3Model::GetVertices(size_t s, MeshVertex_t*& vertexData, size_t& vertexCount) const {
    GetVertices(s, 0, vertexData, vertexCount);
}

void MD3Model::GetIndices(size_t s, GLushort*& indexData, size_t& triangleCount) const {
    assert(s < header->numSurfaces);

    MD3::Surface_t*    surface = surfaces[s];
    MD3::Triangle_t* triangles = GetTriangles(s);
    
    triangleCount = surface->numTriangles;
    indexData = new GLushort[3*triangleCount];

    // Could probably re-index this and play with pointer arithmetic
    // to do this in less lines
    for (int i=0; i<triangleCount; ++i) {
        indexData[3*i]   = (GLushort)triangles[i].a;
        indexData[3*i+1] = (GLushort)triangles[i].b;
        indexData[3*i+2] = (GLushort)triangles[i].c;
    }
}

MD3::Frame_t* MD3Model::GetFrame(size_t f) {
    assert(f < header->numFrames);
    return frames[f];
}