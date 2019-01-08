//
// Created by eli on 18-12-29.
//

#include <GL/glew.h>
#include <fstream>
#include "pathtracer.h"
#include "../accelerators/bvh.h"
#include "ray.h"

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

void PathTracer::set_camera(Camera *camera) {
    this->camera = camera;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
    frameBuffer.resize(width, height);
}

void PathTracer::build_accel() {
    cout<<"开始载入<Primitives*> "<<endl;
    timer.start();
    vector<Primitive*> primitives=mesh->get_primitives();
    timer.stop();
    cout<<"载入完毕,共耗时(s): "<<timer.duration()<<endl;

    cout<<"开始构建BVH"<<endl;
    timer.start();
    bvh=new BVHAccel(primitives,4,SPLIT_SAH);
    timer.stop();
    cout<<"构建完成,共耗时(s): "<<timer.duration()<<endl;

    bvh2=new BVHAccel(primitives,4,SPLIT_EQUAL_COUNTS);

    //test my KmeansBVH
    kbvh=new KmeansBvh(primitives,4);

    peakDensity=new PeakDensity(primitives,0.01);
    peakDensity->run();

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
            //draw heat map  画出热度图 即颜色越深代表该像素点遍历BVH次数越多
            glDrawPixels(frameBuffer.w, frameBuffer.h, GL_RGBA,
                         GL_UNSIGNED_BYTE, &frameBuffer.data[0]);
            break;
        case DONE:
            //sampleBuffer.tonemap(frameBuffer, tm_gamma, tm_level, tm_key, tm_wht);
            glDrawPixels(frameBuffer.w, frameBuffer.h, GL_RGBA,
                         GL_UNSIGNED_BYTE, &frameBuffer.data[0]);
            break;
    }
}

void PathTracer::stop() {

}

void PathTracer::clear() {
    frameBuffer.resize(0, 0);
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

    //BVHNode *svelected = selectionHistory.top();



    glDisable(GL_POLYGON_OFFSET_FILL);
/*
    // draw geometry outline 画三角片元
    for (size_t i = 0; i < selected->range; ++i) {
        bvh->primitives[selected->start + i]->drawOutline(cprim_hl_edges);
    }*/

/**
   * 测试kmeans
   *
   *
   *
   */

   Kmeans *kmeans=kbvh->getKmeans();
    //产生k种不同颜色
    vector <Color> v_color;
    v_color.push_back(Color(.6,.6,1.0,1));//浅蓝色
    v_color.push_back(Color(1.0,0.0,1.0,1));//梅红色
    v_color.push_back(Color(0.0,1.0,1.0,1));//亮蓝色
    v_color.push_back(Color(0.5,0.5,0.0,1));//土黄色
    v_color.push_back(Color(0.5,0.75,0.3,1));//浅绿色
    v_color.push_back(Color(0.9,0.2,0.8,1));//
    v_color.push_back(Color(0.0,0.6,0.25,1));
    v_color.push_back(Color(0.25,0.25,0.6,1));
    for(size_t i=0;i<8;++i){
        //cout<<kmeans->cluster[i].indexOfPrimitives.size()<<endl;
       // cout<<v_color[i]<<endl;
        kmeans->cluster[i].representive.draw(cnode_hl);
        for(size_t idx=0;idx<kmeans->cluster[i].indexOfPrimitives.size();++idx){
            size_t r_index=kmeans->cluster[i].indexOfPrimitives[idx];
            kmeans->primitives[r_index]->draw(v_color[i]);
        }
    }

    //画出每一层的包围盒 kmeans+AC
    Kmeans *kk=kbvh->getKmeans();
    stack<Kmeans*> kstack;
    kstack.push(kk);
    while(!kstack.empty()){

        Kmeans *temp=kstack.top();
        kstack.pop();
        if(temp!=NULL){
            stack<KBVHNode*> bstack;
            bstack.push(temp->root);
            while(!bstack.empty()){
                KBVHNode *current=bstack.top();
                bstack.pop();
                current->bb.draw(cnode);
                if(current->l) bstack.push(current->l);
                if(current->r) bstack.push(current->r);
            }

            //入栈子节点
            for(size_t i=0;i<temp->m_K;++i)
                kstack.push(temp->children[i]);
        }
    }


   // glDepthMask(GL_FALSE);

    // create traversal stack
/*    stack<BVHNode *> tstack;

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
    }*/
}


//画出热度图 HEAT MAP
void PathTracer::start_raytracing() {
   // state=RENDERING;
    frameBuffer.clear();
    draw_heat_map(frameBuffer.w,frameBuffer.h);
    draw_heat_map_kmeansbvh(frameBuffer.w,frameBuffer.h);
}



void PathTracer::draw_heat_map(int w,int h) {
    cout<<"输出sah-binned heat map"<<endl;
    ofstream out;
    out.open("../sah-heat-map.ppm");
    out<<"P3"<<endl<<w<<" "<<h<<endl<<"255"<<endl;


    size_t max=0;
    vector<size_t > v;
    for (size_t y = 0; y <h ; ++y) {
        for (size_t x = 0; x < w; x++) {
            size_t s = traverse_pixel(x, y);
           // cout<<x<<" "<<y<<" "<<s<<endl;
            v.push_back(s);
            if(s>=max)
                max=s;
               // frameBuffer.update_heat_map(s,x,y);
        }
    }

    for(size_t i=0;i<v.size();++i){
        float s=float(v[i])/max;
        glm::vec3 c=(1-s)*glm::vec3(0.48627,0.98824,0.0)+s*glm::vec3(1.0,0.0,0.0);
        int ir=int(c.x*255.99);
        int ig=int(c.y*255.99);
        int ib=int(c.z*255.99);
        out<<ir<<" "<<ig<<" "<<ib<<endl;
    }
    out.close();
}


void PathTracer::draw_heat_map_kmeansbvh(int w, int h) {
    cout<<"输出kBVH heat map"<<endl;
    ofstream out;
    out.open("../k-heat-map.ppm");
    out<<"P3"<<endl<<w<<" "<<h<<endl<<"255"<<endl;


    size_t max=0;
    vector<size_t > v;
    for (size_t y = 0; y <h ; ++y) {
        for (size_t x = 0; x < w; x++) {
            size_t s = traverse_pixel_kmeansbvh(x, y);
            //cout<<x<<" "<<y<<" "<<s<<endl;
            v.push_back(s);
            if(s>=max)
                max=s;
            // frameBuffer.update_heat_map(s,x,y);
        }
    }

    for(size_t i=0;i<v.size();++i){
        float s=float(v[i])/max;
        glm::vec3 c=(1-s)*glm::vec3(0.48627,0.98824,0.0)+s*glm::vec3(1.0,0.0,0.0);
        int ir=int(c.x*255.99);
        int ig=int(c.y*255.99);
        int ib=int(c.z*255.99);
        out<<ir<<" "<<ig<<" "<<ib<<endl;
    }
    out.close();
}



//光线遍历BVH
size_t PathTracer::traverse_pixel(int x, int y) {

    size_t screenW = frameBuffer.w;
    size_t screenH = frameBuffer.h;

    Ray r = camera->generate_ray((x + 0.5) / screenW,
                                 (y + 0.5) / screenH);
    size_t res=bvh->intersect(r);
    return res;
}



/*
 * 光线遍历Kmeans 构造的BVH
 */
size_t PathTracer::traverse_pixel_kmeansbvh(int x, int y) {
    size_t screenW = frameBuffer.w;
    size_t screenH = frameBuffer.h;

    Ray r = camera->generate_ray((x + 0.5) / screenW,
                                 (y + 0.5) / screenH);
    size_t res=bvh2->intersect(r);
    return res;
}