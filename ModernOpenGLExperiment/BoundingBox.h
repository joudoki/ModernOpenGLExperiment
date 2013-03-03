#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Rendering.h"

class BoundingBox {
public:
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox() {};
    BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}

    // Determine the transform to use to render
    // a mesh that's defined on [-1,-1,-1],[1,1,1]
    glm::mat4 GetTransform() const;
};

#endif