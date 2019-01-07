//
// Created by eli on 18-12-29.
//

#ifndef ERENDER_PATHTRACER_H
#define ERENDER_PATHTRACER_H


#include "../mesh/mesh.h"
#include "aggregate.h"
#include "../utilis/timer.h"
#include "../accelerators/bvh.h"
#include "../accelerators/KmeansBvh.h"
#include "../accelerators/PeakDensity.h"
#include "../utilis/image.h"
#include "../camera/camera.h"
#include <stack>

class PathTracer{
public:
    PathTracer();
    ~PathTracer();

    void setMesh(Mesh* mesh);
    void updateScreen();

    /**
 * Transitions from any running state to READY.
 */
    void stop();

    /**
     * If the pathtracer is in READY, delete all internal data, transition to INIT.
     */
    void clear();

    /**
     * If the pathtracer is in READY, transition to VISUALIZE.
     */
    void start_visualizing();

    /**
     * If the pathtracer is in READY, transition to RENDERING.
     */
    void start_raytracing();

    void set_camera(Camera* camera);

    void set_frame_size(size_t width, size_t height);
    /**
     * 画出遍历热度图
     */
    void draw_heat_map(int x,int y);

    size_t traverse_pixel(int x,int y);
private:
    /**
   * Build acceleration structures.
   */
    void build_accel();

    /**
     * Visualize acceleration structures.
     */
    void visualize_accel() const;

    enum State {
        INIT,               ///< to be initialized
        READY,              ///< initialized ready to do stuff
        VISUALIZE,          ///< visualizing BVH accelerator aggregate
        RENDERING,          ///< started but not completed raytracing
        DONE                ///< started and completed raytracing
    };

    // Configurables //

    State state;          ///< current state
    Mesh* mesh;          ///< current scene
    BVHAccel* bvh;

    //KmeansBvh//
    KmeansBvh* kbvh;

    //PeakDensity
    PeakDensity* peakDensity;
    std::stack<BVHNode*> selectionHistory;  ///< node selection history
    Timer timer;

    //Convert to Image
    ImageBuffer frameBuffer;       ///< frame buffer
    Camera* camera;       ///< current camera
};


#endif //ERENDER_PATHTRACER_H
