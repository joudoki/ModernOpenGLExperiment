#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Trackball {
private:
    float r2;
    bool dragging;
    int resW, resH;

    glm::vec3 dragStart;
    glm::vec3 dragStop;

    glm::mat4 view;

    /**
     * ScreenToTrackballCoordinates
     * Converts a pair of screen coordinates (i,j) to trackball coordinates (x,y,z)
     *    Screen Coords: Top left is (0,0),    bottom right is (scrW,scrH)
     * Trackball Coords: Top left is (-1,1,z), bottom right is (1,-1,z)
     */
    glm::vec3 ScreenToTrackballCoordinates(int i, int j) const;

public:
    Trackball(int resW, int resH, float r, glm::mat4 view)
        : resW(resW), resH(resH), r2(r*r),
          dragging(false), view(view) {};

    ~Trackball() {};

    /**
     * GetRotationMatrix
     * Gets the current rotation matrix to use when displaying the model
     */
    glm::mat4 GetRotationMatrix() const;

    /**
     * MouseUpdate
     * Updates the trackball interface with new mouse input
     * mouesDown is true if the LMB is being held down
     * (i,j) is the screen-space coordinate of the mouse
     */
    void MouseUpdate(bool mouseDown, int i, int j);
    
};

#endif