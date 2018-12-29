//
// Created by eli on 18-12-28.
//

#ifndef ERENDER_MESH_H
#define ERENDER_MESH_H

#include <vector>
#include "polymesh.h"
#include "../core/primitive.h"

using std::vector;

class Mesh {
public:
    PolymeshInfo* polymeshInfo;

    Mesh(PolymeshInfo* polyMesh);

    ~Mesh();

    void render_in_opengl() const;

    vector<Primitive*> get_primitives() const;
private:
    // Helpers for render_in_opengl.
    void draw_faces() const;
    void draw_edges() const;
    void draw_vertex() const;
};


#endif //ERENDER_MESH_H
