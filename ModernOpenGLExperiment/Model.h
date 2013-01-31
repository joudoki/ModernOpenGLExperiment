#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Program.h"
#include "Rendering.h"

// Model: Contains a mesh and a program
class Model {
private:
    Mesh* mesh;
    Program* program;

    Model() : mesh(NULL), program(NULL) {}
    ~Model() {}

public:
    void Render() const;
};

#endif