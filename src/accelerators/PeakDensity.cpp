//
// Created by eli on 19-1-6.
//


#include "PeakDensity.h"


// Expands a 10-bit integer into 30 bits
// by inserting 2 zeros after each bit.
unsigned int PeakDensity::expandBits(unsigned int v)
{
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
}

// Calculates a 30-bit Morton code for the
// given 3D point located within the unit cube [0,1].
unsigned int PeakDensity::morton3D(float x, float y, float z)
{
    x = min(max(x * 1024.0f, 0.0f), 1023.0f);
    y = min(max(y * 1024.0f, 0.0f), 1023.0f);
    z = min(max(z * 1024.0f, 0.0f), 1023.0f);
    unsigned int xx = expandBits((unsigned int)x);
    unsigned int yy = expandBits((unsigned int)y);
    unsigned int zz = expandBits((unsigned int)z);
    return xx * 4 + yy * 2 + zz;
}

/**
* a wrapper to calculate morton code from
* the position of an object inside the
* unit cube.
*/
unsigned int PeakDensity::morton3D(glm::vec3 pos)
{
    return morton3D(pos.x,pos.y,pos.z);
}

/**
* comparer used to sort primitives according
* to their morton code.
*/
bool PeakDensity::mortonCompare(Primitive* p1, Primitive* p2)
{
    return p1->morton_code < p2->morton_code;
}



//构造函数
PeakDensity::PeakDensity(std::vector< Primitive* > primitives, double dc){
    this->primitives=primitives;
    //密度
    this->p.resize(primitives.size());
    //距离Θ
    this->distance.resize(primitives.size());
    //截断距离
    this->dc=dc;


    // edge case
    if (primitives.empty()) {
        return;
    }

    // calculate root AABB size 计算世界包围盒
    BBox bb;
    for (size_t i = 0; i < primitives.size(); ++i) {
        bb.expand(primitives[i]->get_bbox());
    }
    root = new BVHNode(bb, 0, primitives.size());

    /**
     * 计算每个片元的morton码
     */
    timer.start();
    for (size_t i = 0; i < primitives.size(); ++i) {
        //getUnitcubePosOf 判断中心点在世界包围盒内的具体分割单元(栅格)
        unsigned int morton_code = morton3D(bb.getUnitcubePosOf(primitives[i]->get_bbox().centroid()));
        primitives[i]->morton_code = morton_code;
    }
    timer.stop();
    cout<<"计算MortonCode共耗时(s): "<<timer.duration()<<endl;


    timer.start();
    // sort primitives using morton code 排序
    std::sort(primitives.begin(), primitives.end(), mortonCompare);
    timer.stop();
    cout<<"排序共耗时(s): "<<timer.duration()<<endl;

//      for(size_t i=0;i<primitives.size();++i){
//          cout<<primitives[i]->morton_code<<endl;
//      }
}









/**
 *cut-off kernal
 * 离散值
 *
 */
double PeakDensity::cutOffKernal(double dis){
    if(dis<dc)
        return 1.0f;
    else
        return 0.0f;
}


/**
 *gaussianKernal
 * 连续值
 * 避免冲突
 */
double PeakDensity::gaussianKernal(double dis){

    double x=pow(dis/dc,2);
    x=-x;
    double gaussianVal=exp(x);
    return gaussianVal;
}


//根据截断距离计算每个片元的密度
void PeakDensity::calP(){
    /*timer.start();
     for(size_t i=0;i<primitives.size();++i){
       cout<<i<<endl;
       for(size_t j=0;j<primitives.size();++j){
          if(i==j)
        continue;
      //欧式距离
      double euclidenDistance=(primitives[i]->get_bbox().centroid()-primitives[j]->get_bbox().centroid()).norm2();
      //cout<<euclidenDistance<<endl;
      p[i]+=gaussianKernal(euclidenDistance);
      }
    }
    timer.stop();
    cout<<"计算密度共耗时(sec):  "<<timer.duration()<<endl;
    //打印所有
    for(size_t i=0;i<p.size();i++){
       cout<<p[i]<<" ";
       if(i%40==0)
         cout<<endl;
    }*/

    timer.start();
    //搜索半径
    size_t searchRadius=25;
    for(size_t i=0;i<primitives.size();i++){
        for(size_t j=i-searchRadius;j<i+searchRadius;++j){
            if(i==j||j<0)
                continue;
            if(j>=primitives.size())
                break;
            double euclidenDistance=glm::length(primitives[i]->get_bbox().centroid()-primitives[j]->get_bbox().centroid());
            p[i]+=gaussianKernal(euclidenDistance);
        }
    }
    timer.stop();
    cout<<"计算密度共耗时(sec):  "<<timer.duration()<<endl;
    //打印所有
//    for(size_t i=0;i<p.size();i++) {
//        cout << p[i] << " ";
//        if (i % 40 == 0)
//            cout << endl;
//    }
}


void PeakDensity::run(){
    calP();
}

//计算距离属性
void PeakDensity::calDistance(){

}