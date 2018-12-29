//
// Created by eli on 18-12-29.
//

#include "pathtracer.h"
#include "../accelerators/bvh.h"

PathTracer::PathTracer() {
    mesh=nullptr;
    state=INIT;
    bvh= nullptr;
}

PathTracer::~PathTracer() {
    delete mesh;
    delete bvh;
}

void PathTracer::setMesh(Mesh *mesh) {
    if (state != INIT) {
        return;
    }
    this->mesh=mesh;
    build_accel();
    state=READY;
}


void PathTracer::build_accel() {
    cout<<"开始载入<Primitives*> "<<endl;
    timer.start();
    vector<Primitive*> primitives=mesh->get_primitives();
    timer.stop();
    cout<<"载入完毕,共耗时(s): "<<timer.duration()<<endl;

    cout<<"开始构建BVH"<<endl;
    timer.start();
    bvh=new BVHAccel(primitives);
    timer.stop();
    cout<<"构建完成,共耗时(s): "<<timer.duration()<<endl;

}

void PathTracer::updateScreen() {
    switch (state) {
        case INIT:
        case READY:
            break;
        case VISUALIZE:
            visualize_accel();

            break;
        case RENDERING:
/*            glDrawPixels(frameBuffer.w, frameBuffer.h, GL_RGBA,
                         GL_UNSIGNED_BYTE, &frameBuffer.data[0]);*/
            break;
        case DONE:
            //sampleBuffer.tonemap(frameBuffer, tm_gamma, tm_level, tm_key, tm_wht);
/*            glDrawPixels(frameBuffer.w, frameBuffer.h, GL_RGBA,
                         GL_UNSIGNED_BYTE, &frameBuffer.data[0]);*/
            break;
    }
}

void PathTracer::stop() {

}

void PathTracer::clear() {


}

void PathTracer::start_raytracing() {

}

void PathTracer::start_visualizing() {
    if (state != READY) {
        return;
    }
    state = VISUALIZE;
}

//画BVH
void PathTracer::visualize_accel() const {

}