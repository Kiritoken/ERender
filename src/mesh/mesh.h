//
// Created by eli on 18-12-28.
//

#ifndef ERENDER_MESH_H
#define ERENDER_MESH_H


#include "polymesh.h"

class Mesh {
public:
    Mesh(PolymeshInfo* polyMesh);

    ~Mesh();

    void render_in_opengl() const;
private:

    PolymeshInfo* polymeshInfo;

    // Helpers for render_in_opengl.
    void draw_faces() const;
    void draw_edges() const;
    void draw_vertex() const;
};


#endif //ERENDER_MESH_H
