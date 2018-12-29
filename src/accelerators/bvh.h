//
// Created by eli on 18-12-29.
//

#ifndef ERENDER_BVH_H
#define ERENDER_BVH_H


#include "../core/aggregate.h"
enum ConstructMethod { SPLIT_MIDDLE, SPLIT_EQUAL_COUNTS, SPLIT_AAC, SPLIT_SAH};

struct BVHNode {

    BVHNode(BBox bb, size_t start, size_t range)
            : bb(bb), start(start), range(range), l(nullptr), r(nullptr) { }

    inline bool isLeaf() const { return l == nullptr && r == nullptr; }

    BBox bb;        ///< bounding box of the node
    size_t start;   ///< start index into the primitive list
    size_t range;   ///< range of index into the primitive list   end() 超尾
    BVHNode* l;     ///< left child node
    BVHNode* r;     ///< right child node
};

class BVHAccel: public Aggregate {
public:


    /**
     * Parameterized Constructor.
     * Create BVH from a list of primitives. Note that the BVHAccel Aggregate
     * stores pointers to the primitives and thus the primitives need be kept
     * in memory for the aggregate to function properly.
     * \param primitives primitives to build from
     * \param max_leaf_size maximum number of primitives to be stored in leaves
     */
    BVHAccel(const std::vector<Primitive*>& primitives, size_t max_leaf_size = 4, ConstructMethod splitMethod=SPLIT_MIDDLE);


    /**
     * Destructor.
     * The destructor only destroys the Aggregate itself, the primitives that
     * it contains are left untouched.
     */
    ~BVHAccel();

    /**
     * Get the world space bounding box of the aggregate.
     * \return world space bounding box of the aggregate
     */
    BBox get_bbox() const;

    //TODO
/*    *//**
     * Ray - Aggregate intersection.
     * Check if the given ray intersects with the aggregate (any primitive in
     * the aggregate), no intersection information is stored.
     * \param r ray to test intersection with
     * \return true if the given ray intersects with the aggregate,
               false otherwise
     *//*
    bool intersect(const Ray& r) const;

    *//**
     * Ray - Aggregate intersection 2.
     * Check if the given ray intersects with the aggregate (any primitive in
     * the aggregate). If so, the input intersection data is updated to contain
     * intersection information for the point of intersection. Note that the
     * intersected primitive entry in the intersection should be updated to
     * the actual primitive in the aggregate that the ray intersected with and
     * not the aggregate itself.
     * \param r ray to test intersection with
     * \param i address to store intersection info
     * \return true if the given ray intersects with the aggregate,
               false otherwise
     *//*
    bool intersect(const Ray& r, Intersection* i) const;

    *//**
     * Get BSDF of the surface material
     * Note that this does not make sense for the BVHAccel aggregate
     * because it does not have a surface material. Therefore this
     * should always return a null pointer.
     *//*
    BSDF* get_bsdf() const { return NULL; }*/

    /**
     * Get entry point (root) - used in visualizer
     */
    BVHNode* get_root() const { return root; }

    /**
     * Draw the BVH with OpenGL - used in visualizer
     */
    void draw(const Color& c) const { }

    /**
     * Draw the BVH outline with OpenGL - used in visualizer
     */
    void drawOutline(const Color& c) const { }

private:
    BVHNode* root; ///< root node of the BVH
};


#endif //ERENDER_BVH_H
