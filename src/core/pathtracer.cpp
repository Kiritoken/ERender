//
// Created by eli on 18-12-29.
//

#include <GL/glew.h>
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

    selectionHistory.push(bvh->get_root());
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
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glLineWidth(1);
    glEnable(GL_DEPTH_TEST);

    // hardcoded color settings
    Color cnode = Color(.5, .5, .5, .25);
    Color cnode_hl = Color(1., .25, .0, .6);
    Color cnode_hl_child = Color(1., 1., 1., .6);

    Color cprim_hl_left = Color(.6, .6, 1., 1);
    Color cprim_hl_right = Color(.8, .8, 1., 1);
    Color cprim_hl_edges = Color(0., 0., 0., 0.5);//黑边

    BVHNode *selected = selectionHistory.top();

    // render solid geometry (with depth offset)
    glPolygonOffset(1.0, 1.0);


    //解决Z-fighting问题
    glEnable(GL_POLYGON_OFFSET_FILL);

    //只画选择的包围盒内的片元
    //叶子节点  依次画出所有片元
/*    if (selected->isLeaf()) {
        for (size_t i = 0; i < selected->range; ++i) {
             bvh->primitives[selected->start + i]->draw(cprim_hl_left);
        }
    } else {
        //非叶子节点 存在左子树
        if (selected->l) {
            BVHNode* child = selected->l;
              //cout<<"左子树节点  共计:"<<child->range<<" triangles"<<endl;
            for (size_t i = 0; i < child->range; ++i) {
                bvh->primitives[child->start + i]->draw(cprim_hl_left);
            }
        }
        //右子树
        if (selected->r) {
            BVHNode* child = selected->r;
              //cout<<"右子树节点  共计:"<<child->range<<" triangles"<<endl;
            for (size_t i = 0; i < child->range; ++i) {
                bvh->primitives[child->start + i]->draw(cprim_hl_right);
            }
        }
    }*/

    glDisable(GL_POLYGON_OFFSET_FILL);

    // draw geometry outline 画三角片元
    for (size_t i = 0; i < selected->range; ++i) {
        bvh->primitives[selected->start + i]->drawOutline(cprim_hl_edges);
    }


    glDepthMask(GL_FALSE);

    // create traversal stack
    stack<BVHNode *> tstack;

    // push initial traversal data
    tstack.push(bvh->get_root());

    // draw all BVH bboxes with non-highlighted color
    //画出所有包围盒
    while (!tstack.empty()) {

        BVHNode *current = tstack.top();
        tstack.pop();
        current->bb.draw(cnode);
        if (current->l) tstack.push(current->l);
        if (current->r) tstack.push(current->r);
    }
}