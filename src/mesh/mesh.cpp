//
// Created by eli on 18-12-28.
//
#include "mesh.h"
#include <GL/glew.h>
#include "../shape/triangle.h"

Mesh::Mesh(PolymeshInfo *polyMesh) {
    this->polymeshInfo=polyMesh;
}

Mesh::~Mesh() {

}

void Mesh::render_in_opengl() const {
    if(polymeshInfo) {
        glEnable(GL_LIGHTING);
        draw_faces();

        // Edges are drawn with flat shading.
        glDisable(GL_LIGHTING);
        draw_edges();

        glEnable(GL_LIGHTING);
    }
}


void Mesh::draw_faces() const {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    for(size_t i=0;i<polymeshInfo->polygons.size();++i){

        glBegin(GL_POLYGON);
        for(size_t j=0;j<polymeshInfo->polygons[i].vertex_indices.size();++j){
/*            size_t in=polymeshInfo->polygons[i].normal_indices[j];
            glNormal3f(polymeshInfo->normals[in].x, polymeshInfo->normals[in].y, polymeshInfo->normals[in].z);*/

            size_t iv=polymeshInfo->polygons[i].vertex_indices[j];
            glVertex3f(polymeshInfo->vertices[iv].x, polymeshInfo->vertices[iv].y, polymeshInfo->vertices[iv].z);
        }
        glEnd();
    }
}



void Mesh::draw_edges() const {

}

vector<Primitive*> Mesh::get_primitives() const {
    vector<Primitive*> res;
    res.reserve(polymeshInfo->polygons.size());
    for(size_t i=0;i<polymeshInfo->polygons.size();++i) {
        res.emplace_back(new Triangle(this, polymeshInfo->polygons[i].vertex_indices[0],
                                      polymeshInfo->polygons[i].vertex_indices[1],
                                      polymeshInfo->polygons[i].vertex_indices[2]));
    }
    return res;
}