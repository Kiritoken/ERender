//
// Created by eli on 18-12-26.
//

#ifndef ERENDER_CAMERA_H
#define ERENDER_CAMERA_H
#include <iostream>
#include "../glm/glm.hpp"
#include "camera_info.h"
#include "../core/ray.h"

class Camera {
public:
    /*
      Sets the field of view to match screen screenW/H.
      NOTE: data and screenW/H will almost certainly disagree about the aspect
            ratio. screenW/H are treated as the source of truth, and the field
            of view is expanded along whichever dimension is too narrow.
      NOTE2: info.hFov and info.vFov are expected to be in DEGREES.

      宽高比  W/H
    */
    void configure(const CameraInfo& info,
                   size_t screenW, size_t screenH);

    /*
      Phi and theta are in RADIANS.弧度
    */
    void place(const glm::dvec3& targetPos, const double phi, const double theta,
               const double r, const double minR, const double maxR);

    /*
      Copies just placement data from the other camera.
    */
    void copy_placement(const Camera& other);

    /*
      Updates the screen size to be the specified size, keeping screenDist
      constant.
    */
    void set_screen_size(const size_t screenW, const size_t screenH);

    /*
      Translates the camera such that a value at distance d directly in front of
      the camera moves by (dx, dy). Note that dx and dy are in screen coordinates,
      while d is in world-space coordinates (like pos/dir/up).
    */
    void move_by(const double dx, const double dy, const double d);

    /*
      Move the specified amount along the view axis.
    */
    void move_forward(const double dist);

    /*
      Rotate by the specified amount around the target.
    */
    void rotate_by(const double dPhi, const double dTheta);

    glm::dvec3 position() const { return pos; }
    glm::dvec3 view_point() const { return targetPos; }
    glm::dvec3 up_dir() const { return c2w[1]; }
    double v_fov() const { return vFov; }
    double aspect_ratio() const { return ar; }
    double near_clip() const { return nClip; }
    double far_clip() const { return fClip; }

    Ray generate_ray(double x, double y) const;

    // Current position and target point (the point the camera is looking at).
    glm::dvec3 pos, targetPos;

    // ===RUI===
    glm::dvec2 get_screen_pos(glm::dvec3 p) const;

private:
    // Computes pos, screenXDir, screenYDir from target, r, phi, theta.
    void compute_position();

    // Field of view aspect ratio, clipping planes.
    double hFov, vFov, ar, nClip, fClip;

    // Orientation relative to target, and min & max distance from the target.
    double phi, theta, r, minR, maxR;

    // camera-to-world rotation matrix (note: also need to translate a
    // camera-space point by 'pos' to perform a full camera-to-world
    // transform)
    glm::dmat3x3 c2w;

    // Info about screen to render to; it corresponds to the camera's full field
    // of view at some distance.
    size_t screenW, screenH;
    double screenDist;
};


#endif //ERENDER_CAMERA_H
