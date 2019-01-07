//
// Created by eli on 18-12-26.
//

#include "camera.h"
#include "../utilis/misc.h"
using std::min;
using std::max;
using namespace misc;

void Camera::configure(const CameraInfo& info, size_t screenW, size_t screenH) {
    //初始化
    this->screenW = screenW;
    this->screenH = screenH;
    nClip = info.nClip;
    fClip = info.fClip;
    hFov = info.hFov;
    vFov = info.vFov;

    double ar1 = tan(radians(hFov) / 2) / tan(radians(vFov) / 2);

    //aspect ration 宽高比
    ar = static_cast<double>(screenW) / screenH;
    if (ar1 < ar) {
        // hFov is too small
        hFov = 2 * degrees(atan(tan(radians(vFov) / 2) * ar));
    } else if (ar1 > ar) {
        // vFov is too small
        vFov = 2 * degrees(atan(tan(radians(hFov) / 2) / ar));
    }
    screenDist = ((double) screenH) / (2.0 * tan(radians(vFov) / 2));
}



void Camera::place(const glm::dvec3& targetPos, const double phi,
                   const double theta, const double r, const double minR,
                   const double maxR) {
    double r_ = min(max(r, minR), maxR);
    double phi_ = (sin(phi) == 0) ? (phi + EPS_F) : phi;
    this->targetPos = targetPos;
    this->phi = phi_;
    this->theta = theta;
    this->r = r_;
    this->minR = minR;
    this->maxR = maxR;
    compute_position();
}


void Camera::copy_placement(const Camera& other) {
    pos = other.pos;
    targetPos = other.targetPos;
    phi = other.phi;
    theta = other.theta;
    minR = other.minR;
    maxR = other.maxR;
    c2w = other.c2w;
}

void Camera::set_screen_size(const size_t screenW, const size_t screenH) {
    this->screenW = screenW;
    this->screenH = screenH;
    ar = 1.0 * screenW / screenH;
    hFov = 2 * degrees(atan(((double) screenW) / (2 * screenDist)));
    vFov = 2 * degrees(atan(((double) screenH) / (2 * screenDist)));
}


void Camera::move_by(const double dx, const double dy, const double d) {
    const double scaleFactor = d / screenDist;
    const glm::dvec3& displacement =
            c2w[0] * (dx * scaleFactor) + c2w[1] * (dy * scaleFactor);
    pos += displacement;
    targetPos += displacement;
}


void Camera::move_forward(const double dist) {
    double newR = min(max(r - dist, minR), maxR);
    pos = targetPos + ((pos - targetPos) * (newR / r));
    r = newR;
}

void Camera::rotate_by(const double dPhi, const double dTheta) {
    phi = clamp(phi + dPhi, 0.0, (double) PI);
    theta += dTheta;
    compute_position();
}

void Camera::compute_position() {
    double sinPhi = sin(phi);
    if (sinPhi == 0) {
        phi += EPS_F;
        sinPhi = sin(phi);
    }
    const glm::dvec3 dirToCamera(r * sinPhi * sin(theta),
                               r * cos(phi),
                               r * sinPhi * cos(theta));
    pos = targetPos + dirToCamera;
    glm::dvec3 upVec(0, sinPhi > 0 ? 1 : -1, 0);
    glm::dvec3 screenXDir = cross(upVec, dirToCamera);
    screenXDir=glm::normalize(screenXDir);
    glm::dvec3 screenYDir = cross(dirToCamera, screenXDir);
    screenYDir=glm::normalize(screenYDir);

    c2w[0] = screenXDir;
    c2w[1] = screenYDir;
    c2w[2] = glm::normalize(dirToCamera);  // camera's view direction is the
    // opposite of of dirToCamera, so
    // directly using dirToCamera as
    // column 2 of the matrix takes [0 0 -1]
    // to the world space view direction
}


// ===RUI===
glm::dvec2 Camera::get_screen_pos(glm::dvec3 p) const {

    glm::dvec3 dir = p - this->pos;
    dir=glm::normalize(dir);
    glm::dmat3x3 w2c(glm::transpose(c2w));
    dir = w2c * dir;

    dir =  ((-1.0) / dir.z) * dir;

    double sensor_height = 2.0 * tan( radians(vFov/2.0) );
    double sensor_width = ar * sensor_height;

    // cout<<dir<<"sensor_height "<<sensor_height<<" sensor_width "<<sensor_width<<endl;

    double sensor_x = dir.x;
    double sensor_y = dir.y;

    double x = sensor_x / sensor_width;
    double y = sensor_y / sensor_height;

    return glm::dvec2(x, y);
}


Ray Camera::generate_ray(double x, double y) const {

    // compute position of the input sensor sample coordinate on the
    // canonical sensor plane one unit away from the pinhole.

    double sensor_height = 2.0 * tan( radians(vFov/2.0) );
    double sensor_width = ar * sensor_height;

    double sensor_x = sensor_width  * (x - 0.5);
    double sensor_y = sensor_height * (y - 0.5);

    glm::vec3 dir(sensor_x, sensor_y, -1);
    // cout<<"dir(sensor_x, sensor_y, -1);"<<dir<<'\t'<<x<<'\t'<<y<<endl;

    return Ray(pos, c2w * glm::normalize(dir));
}