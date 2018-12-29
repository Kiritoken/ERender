//
// Created by eli on 18-12-28.
//
#include "mesh.h"
#include <GL/glew.h>

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
    glColor3f(1, 0, 0);
    for(size_t i=0;i<polymeshInfo->polygons.size();++i){
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
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