//
// Created by eli on 18-12-26.
//

#ifndef ERENDER_CAMERA_INFO_H
#define ERENDER_CAMERA_INFO_H
#include "../glm/vec3.hpp"

struct CameraInfo {

    //view_dir=(targetPos-cameraPos)
    glm::dvec3 view_dir;
    //up_dir 相机上方先
    glm::dvec3 up_dir;
    /**
     * hFov:水平视野 角度
     * vFov:垂直视野 角度
     * nClip 近裁平面
     * fClip 远裁平面
     */
    float hFov,vFov,nClip,fClip;
};


#endif //ERENDER_CAMERA_INFO_H
