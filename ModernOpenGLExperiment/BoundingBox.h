#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Rendering.h"

class BoundingBox
{
private:
    glm::vec3 min;
    glm::vec3 max;

public:
    BoundingBox();
    ~BoundingBox();
};

#endif