//
// Created by eli on 19-1-6.
//

#ifndef ERENDER_RAY_H
#define ERENDER_RAY_H

#include <iostream>
#include "../glm/glm.hpp"
#include "../utilis/misc.h"

struct Ray {
    size_t depth;  ///< depth of the Ray

    glm::vec3 o;  ///< origin
    glm::vec3 d;  ///< direction
    mutable double min_t; ///< treat the ray as a segment (ray "begin" at max_t)
    mutable double max_t; ///< treat the ray as a segment (ray "ends" at max_t)

    glm::vec3 inv_d;  ///< component wise inverse
    int sign[3];     ///< fast ray-bbox intersection

    /**
     * Constructor.
     * Create a ray instance with given origin and direction.
     * \param o origin of the ray
     * \param d direction of the ray
     * \param depth depth of the ray
     */
    Ray(const glm::vec3& o, const glm::vec3& d, int depth = 0)
            : o(o), d(d), min_t(0.0), max_t(INF_D), depth(depth) {
        inv_d = glm::vec3(1 / d.x, 1 / d.y, 1 / d.z);
        sign[0] = (inv_d.x < 0);
        sign[1] = (inv_d.y < 0);
        sign[2] = (inv_d.z < 0);
    }

    /**
     * Constructor.
     * Create a ray instance with given origin and direction.
     * \param o origin of the ray
     * \param d direction of the ray
     * \param max_t max t value for the ray (if it's actually a segment)
     * \param depth depth of the ray
     */
    Ray(const glm::vec3& o, const glm::vec3& d, double max_t, int depth = 0)
            : o(o), d(d), min_t(0.0), max_t(max_t), depth(depth) {
        inv_d = glm::vec3(1 / d.x, 1 / d.y, 1 / d.z);
        sign[0] = (inv_d.x < 0);
        sign[1] = (inv_d.y < 0);
        sign[2] = (inv_d.z < 0);
    }


    /**
     * Returns the point t * |d| along the ray.
     */
    inline glm::vec3 at_time(double t) const {
        glm::vec3 temp=d;
        temp*=d;
        return o + temp;
    }


    /**
     * Returns the result of transforming the ray by the given transformation
     * matrix.
     */
/*    Ray transform_by(const glm::mat4x4& t) const {
        const glm::vec4& newO = t * glm::vec4(o, 1.0);
        return Ray((newO / newO.w).to3D(), (t * Vector4D(d, 0.0)).to3D());
    }*/
};
#endif //ERENDER_RAY_H
