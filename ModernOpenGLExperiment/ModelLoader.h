#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "Mesh.h"

#include <cstdint>
#include <iostream>

// I was initially thinking I might do some sort of template specialization here...
// Something along the lines of ModelLoader<ModelType::MD3> or ModelLoader<ModelType::OBJ>
// But there seems to be some complications surrounding that, and perhaps that isn't
// really even an appropriate usage of template specialization. I'll have to investigate
// it later.

class ModelLoader {
protected:
    ModelLoader() {}

public:
    /**
     * Whether or not the loaded file is valid or not
     * The outputs of the other functions are undefined if this is false
     */
    virtual bool IsValid() const = 0;
    
    /**
     * Returns the number of meshes countained within the model loaded by this loader
     */
    virtual uint32_t GetMeshCount() const = 0;

    /**
     * Retrieves from the internal format a list of formatted vertices 
     * of the form MD3Model::Vertex_t.
     * 
     * s - The index of the surface to get the vertices from
     * vertexData  - Where to put the vertices
     * vertexCount - How many vertices there are in vertexData
     */
    virtual void GetVertices(uint32_t s, MeshVertex_t*& vertexData, uint32_t& vertexCount) const = 0;

    /**
     * Retrieves from the internal format the list of indices making
     * up the primitives needed to render this model.
     *
     * s - The index of the surface to retrieve indices from
     * indexData     - Where to put the indices
     * triangleCount - The number of primitives in indexData
     */
    virtual void GetIndices(uint32_t s, GLushort*& indexData, uint32_t& triangleCount) const = 0;
};

#endif