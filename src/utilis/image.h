//
// Created by eli on 19-1-6.
//

#ifndef ERENDER_IMAGE_H
#define ERENDER_IMAGE_H
#include <iostream>
#include <vector>
#include <cstring>
#include "../core/color.h"
#include "misc.h"
#include "../glm/glm.hpp"
using namespace misc;

struct ImageBuffer {
    /**
     * Default constructor.
     * The default constructor creates a zero-sized image.
     */
    ImageBuffer() : w(0), h(0) {}

    /**
     * Parameterized constructor.
     * Create an image of given size.
     * \param w width of the image
     * \param h height of the image
     */
    ImageBuffer(size_t w, size_t h) : w(w), h(h) { data.resize(w * h); }

    /**
     * Resize the image buffer.
     * \param w new width of the image
     * \param h new height of the image
     */
    void resize(size_t w, size_t h) {
        this->w = w;
        this->h = h;
        data.resize(w * h);
        clear();
    }

    /**
     * Update the color of a given pixel.
     * \param c color value to be set
     * \param x row of the pixel
     * \param y column of the pixel
     */
    void update_pixel(const Color& c, size_t x, size_t y) {
        // assert(0 <= x && x < w);
        // assert(0 <= y && y < h);
        uint32_t p = 0;
        p += ((uint32_t) (clamp(0.f, 1.f, c.a) * 255)) << 24;
        p += ((uint32_t) (clamp(0.f, 1.f, c.b) * 255)) << 16;
        p += ((uint32_t) (clamp(0.f, 1.f, c.g) * 255)) << 8;
        p += ((uint32_t) (clamp(0.f, 1.f, c.r) * 255));
        data[x + y * w] = p;
    }



    /**
     * 显示遍历热度图  颜色由
     * @param countOfStepsAndIntersect  遍历BVH 求交次数
     * @param x 图像像素点
     * @param y 图像像素点
     */
    void update_heat_map(size_t countOfStepsAndIntersect,size_t x,size_t y){
        glm::vec3 c=(float)(1-countOfStepsAndIntersect)*glm::vec3(1,1,1)+(float)countOfStepsAndIntersect*glm::vec3(0.5,0.7,1.0);
        uint32_t p = 0;
        p += ((uint32_t) (clamp(0.f, 1.f, 1.f) * 255)) << 24;
        p += ((uint32_t) (clamp(0.f, 1.f, c.z) * 255)) << 16;
        p += ((uint32_t) (clamp(0.f, 1.f, c.y) * 255)) << 8;
        p += ((uint32_t) (clamp(0.f, 1.f, c.x) * 255));
        data[x + y * w] = p;
    }

    /**
     * If the buffer is empty.
     */
    bool is_empty() { return (w == 0 && h == 0); }

    /**
     * Clear image data.
     */
    void clear() { memset(&data[0], 0, w * h * sizeof(uint32_t)); }

    size_t w; ///< width
    size_t h; ///< height
    std::vector<uint32_t> data;  ///< pixel buffer
};


#endif //ERENDER_IMAGE_H
