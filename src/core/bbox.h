//
// Created by eli on 18-12-29.
//

#ifndef ERENDER_BBOX_H
#define ERENDER_BBOX_H
#include <iostream>
#include "../glm/glm.hpp"
#include "../glm/vec3.hpp"
#include "../utilis/misc.h"
#include "cuda.h"
#include "cuda_runtime.h"
#include "color.h"

using namespace std;
using namespace glm;

class BBox {
public:

    vec3 max;	    ///< min corner of the bounding box 最大点坐标
    vec3 min;	    ///< max corner of the bounding box 最小点坐标
    vec3 extent;   ///< extent of the bounding box (min -> max) 向量max-min

    /**
     * Constructor.
     * The default constructor creates a new bounding box which contains no
     * points.
     * INF_D=std::numeric_limits<double>::infinity()
     * 默认构造函数 极端最小 max最小点   min最大点
     */
    BBox() {
        max = vec3(-INF_D, -INF_D, -INF_D);
        min = vec3( INF_D,  INF_D,  INF_D);
        extent = max - min;
    }

    /**
     * Constructor.
     * Creates a bounding box that includes a single point.
     * 构造只包含一个点的包围盒
     */
    BBox(const vec3& p) : min(p), max(p) { extent = max - min; }

    /**
     * Constructor.
     * Creates a bounding box with given bounds.
     * \param min the min corner
     * \param max the max corner
     * 构造给定两个点构成的包围盒
     */
    BBox(const vec3& min, const vec3& max) :
            min(min), max(max) { extent = max - min; }

    /**
     * Constructor.
     * Creates a bounding box with given bounds (component wise).
     * 给定点坐标的构造函数
     */
    BBox(const double minX, const double minY, const double minZ,
         const double maxX, const double maxY, const double maxZ) {
        min = vec3(minX, minY, minZ);
        max = vec3(maxX, maxY, maxZ);
        extent = max - min;
    }

    /**
     * Expand the bounding box to include another (union).
     * If the given bounding box is contained within *this*, nothing happens.
     * Otherwise *this* is expanded to the minimum volume that contains the
     * given input.
     * union 并操作
     * 获取最小的min 最大的max坐标
     * \param bbox the bounding box to be included
     */
    __host__ __device__
    void expand(const BBox& bbox) {
        min.x = fmin(min.x, bbox.min.x);
        min.y = fmin(min.y, bbox.min.y);
        min.z = fmin(min.z, bbox.min.z);
        max.x = fmax(max.x, bbox.max.x);
        max.y = fmax(max.y, bbox.max.y);
        max.z = fmax(max.z, bbox.max.z);
        extent.x = max.x - min.x;
        extent.y = max.y - min.y;
        extent.z = max.z - min.z;
    }

    /**
     * Expand the bounding box to include a new point in space.
     * If the given point is already inside *this*, nothing happens.
     * Otherwise *this* is expanded to a minimum volume that contains the given
     * point.
     * \param p the point to be included
     */
    void expand(const vec3& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
        extent = max - min;
    }

    //返回包围盒的中心坐标
    vec3 centroid() const {
        vec3 temp=min+max;
        temp.x/=2;
        temp.y/=2;
        temp.z/=2;
        return temp;
    }

    /**
     * Compute the surface area of the bounding box.
     * 计算包围盒表面积
     * \return surface area of the bounding box.
     */
    double surface_area() const {
        if (empty()) return 0.0;
        return 2 * (extent.x * extent.z +
                    extent.x * extent.y +
                    extent.y * extent.z);
    }

    /**
     * Compute the maximum dimension of the bounding box (x, y, or z).
     * 返回最长边的维度
     * \return 0 if max dimension is x,
     *         1 if max dimension is y,
     *         2 if max dimension is z
     * TODO: replace with enum (or #define)
     *  - sure but please make sure indexing with the returned value still works
     *
     */
    uint8_t max_dimension() const {
        uint8_t d = 0;
        if(extent.y > extent.x) d = 1;
        if(extent.z > extent.y) d = 2;
        return d;
    }

    /**
     * Check if bounding box is empty.
     * Bounding box that has no size is considered empty. Note that since
     * bounding box are used for objects with positive volumes, a bounding
     * box of zero size (empty, or contains a single vertex) are considered
     * empty.
     */
    bool empty() const {
        return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    /**
     * Ray - bbox intersection.
     * Intersects ray with bounding box, does not store shading information.
     * 光线r是否与该包围盒相交
     * \param r the ray to intersect with
     * \param t0 lower bound of intersection time
     * \param t1 upper bound of intersection time
     */
     //TODO
   // bool intersect(const Ray& r, double& t0, double& t1) const;

    /**
     * Draw box wireframe with OpenGL.
     * \param c color of the wireframe
     * OpenGL 画出该包围盒
     */
    void draw(Color c) const;

    /**
     * Calculate and return an object's
     * normalized position in the unit
     * cube defined by this BBox. if the
     * object is not inside of the BBox, its
     * position will be clamped into the BBox.
     *
     * \param pos the position to be evaluated
     * \return the normalized position in the unit
     * cube, with x,y,z ranging from [0,1]
     */
    vec3 getUnitcubePosOf(vec3 pos)
    {
        vec3 o2pos = pos - min;
        if(extent.x==0&&extent.y==0&&extent.z==0){
            return vec3();
        }else{
            vec3 normalized_pos = o2pos / extent;
            return normalized_pos;
        }
    }
};
std::ostream& operator<<(std::ostream& os, const BBox& b);

#endif //ERENDER_BBOX_H
