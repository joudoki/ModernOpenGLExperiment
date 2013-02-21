#include "Trackball.h"

glm::vec3 Trackball::ScreenToTrackballCoordinates(int i, int j) const {
    // Sphere:              1 = (1/r**2) (x**2 + y**2 + z**2)
    // Hyperbolic Sheet:    1 = (1/r**2) (x**2 + y**2 - z**2)

	// Convert screen coordinates to trackball coordinates
	glm::vec2 p = glm::vec2(
		-1.0 + 2.0 * float(i) / resW,
		 1.0 - 2.0 * float(j) / resH
	);
	
	// Radius of Sphere
	//float r  = 1.0;
	//float r2 = r*r;
	
	float x2y2 = p.x*p.x + p.y*p.y;
	float d = r2 - x2y2;
	
	// Determine whether the point is on the sphere or the
	// hyperboloid sheet and set the z value accordingly
	float z = 2.0 * x2y2 <= r2
		? sqrt(d)		        // Sphere
		: r2/sqrt(4.0*x2y2);	// Hyperboloid
	
	return glm::vec3(p,z);
}

glm::mat4 Trackball::GetRotationMatrix() const {
    if (!dragging)
        return view;

    // If start = end, the in-progress rotation we apply
    // will be the additive identity (0 matrix), which will
    // not jive well with the view.
    if (dragStart[0] == dragStop[0] &&
        dragStart[1] == dragStop[1]) {
        
        return view;
    }

    glm::vec3 axis = glm::cross(dragStart, dragStop);
    
    float dot   = glm::dot(dragStart, dragStop);
    float angle = glm::acos(dot) * 180.0f / 3.14159f;
    
    glm::mat4 rot = glm::rotate(glm::mat4(), angle, axis);

    return rot * view;
}

void Trackball::MouseUpdate(bool mouseDown, int i, int j) {
    if (dragging) {
        dragStop = ScreenToTrackballCoordinates(i,j);

        if (!mouseDown) {
            // Update rotational matrix
            view = GetRotationMatrix();

            // Stop Dragging
            dragging = false;
        }
    } else {
        if (mouseDown) {
            // Start Dragging
            dragging = true;
            dragStart = ScreenToTrackballCoordinates(i,j);
            dragStop  = dragStart;
        }
    }
}