#include "BoundingBox.h"

glm::mat4 BoundingBox::GetTransform() const {
    // Determine the translation/scale transformation of the form:
    //    [S | T]
    //    [0 | 1]

    // Such that:
    // The point (-1,-1,-1) is mapped into min
    // The point ( 1, 1, 1) is mapped into max

    return glm::mat4(
        max.x - min.x, 0, 0, max.x + min.x,
        0, max.y - min.y, 0, max.y + min.y,
        0, 0, max.z - min.z, max.z + min.z,
        0, 0, 0, 2
    ) / 2.0f;
}