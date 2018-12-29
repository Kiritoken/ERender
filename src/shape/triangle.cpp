//
// Created by eli on 18-12-29.
//

#include <GL/glew.h>
#include "triangle.h"

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
        mesh(mesh), v1(v1), v2(v2), v3(v3) { }

//v1,v2,v3分别是三个点的索引
BBox Triangle::get_bbox() const {
    BBox bbox(mesh->polymeshInfo->vertices[v1]);
    bbox.expand(mesh->polymeshInfo->vertices[v2]);
    bbox.expand(mesh->polymeshInfo->vertices[v3]);
    return bbox;
}

/*bool Triangle::intersect(const Ray& r) const {
    double alpha, beta, gamma, t;
    return intersect_triangle(r,
                              mesh->positions[v1],
                              mesh->positions[v2],
                              mesh->positions[v3],
                              alpha, beta, gamma, t);
}*/

/*bool Triangle::intersect(const Ray& r, Intersection *isect) const {

    double alpha, beta, gamma, t;
    Vector3D a = mesh->positions[v1];
    Vector3D b = mesh->positions[v2];
    Vector3D c = mesh->positions[v3];

    if (intersect_triangle(r, a, b, c, alpha, beta, gamma, t)) {

        // interpolate normal
        Vector3D n = alpha * mesh->normals[v1] +
                     beta  * mesh->normals[v2] +
                     gamma * mesh->normals[v3];

        r.max_t = t;
        isect->t = t;

        // if we hixt the back of a triangle, we want to flip the normal so
        // the shading normal is pointing toward the incoming ray
        if (dot(n, r.d) > 0)
            isect->n = -n;
        else
            isect->n = n;

        isect->primitive = this;
        isect->bsdf = mesh->get_bsdf();

        return true;
    }
    return false;
}*/

//画三角片元
void Triangle::draw(const Color& c) const {
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLES);
    glVertex3d(mesh->polymeshInfo->vertices[v1].x,
               mesh->polymeshInfo->vertices[v1].y,
               mesh->polymeshInfo->vertices[v1].z);

    glVertex3d(mesh->polymeshInfo->vertices[v2].x,
               mesh->polymeshInfo->vertices[v2].y,
               mesh->polymeshInfo->vertices[v2].z);

    glVertex3d(mesh->polymeshInfo->vertices[v3].x,
               mesh->polymeshInfo->vertices[v3].y,
               mesh->polymeshInfo->vertices[v3].z);
    glEnd();
}

void Triangle::drawOutline(const Color& c) const {
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_LINE_LOOP);
    glVertex3d(mesh->polymeshInfo->vertices[v1].x,
               mesh->polymeshInfo->vertices[v1].y,
               mesh->polymeshInfo->vertices[v1].z);

    glVertex3d(mesh->polymeshInfo->vertices[v2].x,
               mesh->polymeshInfo->vertices[v2].y,
               mesh->polymeshInfo->vertices[v2].z);

    glVertex3d(mesh->polymeshInfo->vertices[v3].x,
               mesh->polymeshInfo->vertices[v3].y,
               mesh->polymeshInfo->vertices[v3].z);
    glEnd();
}