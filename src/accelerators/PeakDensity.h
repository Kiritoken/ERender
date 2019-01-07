//
// Created by eli on 19-1-6.
//

#ifndef ERENDER_PEAKDENSITY_H
#define ERENDER_PEAKDENSITY_H


#include "bvh.h"
#include "../utilis/timer.h"
#include <vector>
#include "../glm/glm.hpp"

class PeakDensity {
private:

    BVHNode* root;



    //索引
    vector <size_t> index;

    //密度
    vector <double> p;

    //距离
    vector <double> distance;

    //截断距离
    double dc;
    //计时用
    Timer timer;

    //cut-off kernal
    double cutOffKernal(double dis);

    //gaussian kernal
    double gaussianKernal(double dis);

    //计算mortoncode相关方法
    unsigned int expandBits(unsigned int v);
    unsigned int morton3D(float x, float y, float z);
    unsigned int morton3D(glm::vec3 pos);
    static bool mortonCompare(Primitive* p1, Primitive* p2);

public:
    //输入数据
    vector <Primitive* > primitives;

    //构造函数
    PeakDensity(vector<Primitive*> primitives,double dc);

    //启动
    void run();

    //设置截断距离
    void setDc(double val){ dc=val;}

    //计算每个片元的密度p
    void calP();

    //计算每个片元的距离Θ
    void calDistance();


};


#endif //ERENDER_PEAKDENSITY_H
