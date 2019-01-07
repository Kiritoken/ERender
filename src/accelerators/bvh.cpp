//
// Created by eli on 18-12-29.
//

#include "bvh.h"
#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include "../utilis/misc.h"
using namespace std;

//SAH bucket的数量
static const size_t kMaxNumBuckets = 12;

struct BVHBuildData {
    BVHBuildData(BBox bb, size_t start, size_t range, BVHNode **dst)
            : bb(bb), start(start), range(range), node(dst) {}
    BBox bb;         ///  包围片元的包围盒
    size_t start;    ///  片元在索引数组的开始索引
    size_t range;    ///  在索引数组中的偏移量
    BVHNode **node;  ///  对应的BVHNODE
};


//记录bucket数据 O(N^2)更新
struct SAHBucketData {
    SAHBucketData() {}
    BBox bb;           /// 桶内包含的最大bbox
    size_t num_prims;  /// 桶内的片元数量
};



/**
 * 初始化构造BVH
 * @param primitives
 * @param max_leaf_size
 */
BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives, size_t max_leaf_size,ConstructMethod splitMethod) {
    this->primitives=_primitives;

    stack<BVHBuildData> bstack;
    //没有面片 直接返回
    if(primitives.empty()){
        return;
    }

    //包围所有三角形的包围盒 与初始化摄像机时计算的包围盒相同 考虑优化
    BBox bb;
    for (size_t i = 0; i < primitives.size(); ++i) {
        bb.expand(primitives[i]->get_bbox());
    }
    cout<<"场景包围盒:"<<endl<<bb<<endl;

    //BVH根节点
    bstack.push(BVHBuildData(bb, 0, primitives.size(), &root));

    splitMethod=SPLIT_SAH;
    switch(splitMethod){
        //改进的SPLIT_MIDDLE  以应对极端现象（左子树 1  右子树很大）
        case SPLIT_MIDDLE:
            std::cout<<"改进的SPLIT_MIDDLE方法:"<<endl;
            while(!bstack.empty()){

                BVHBuildData bdata = bstack.top();
                bstack.pop();
                //新节点
                *bdata.node = new BVHNode(bdata.bb, bdata.start, bdata.range);
                //叶节点直接跳出循环 不做处理
                if (bdata.range <= max_leaf_size) {
                    continue;
                }
                //cout<<bdata.bb<<endl;
                //找出最大范围轴向dim 散度最大
                size_t max_dim=bdata.bb.max_dimension();
                //中点划分坐标
                double pmid=(bdata.bb.max[max_dim]+bdata.bb.min[max_dim])/2;
                //cout<<"中点划分坐标"<<pmid<<endl;
                //std:partition lambda表达式  O(N)复杂度
                 auto it=std::partition(primitives.begin()+bdata.start,
                                       primitives.begin()+bdata.start+bdata.range,
                                       [max_dim,pmid](Primitive *iter){
                                           return iter->get_bbox().centroid()[max_dim]>pmid;
                                       }
                        );
                BBox split_Ba,split_Bb;
                //左节点
                size_t startl = bdata.start;
                size_t rangel = std::distance(primitives.begin(),it) - bdata.start;


                //右节点
                size_t startr = startl + rangel;
                size_t ranger = bdata.range - rangel;

                //   cout<<startl<<" "<<rangel<<" "<<startr<<" "<<ranger<<endl;


                //构造对应节点 如果范围为0 则将该片元单独入栈  并且startr++ ranger--
                if(rangel!=0){
                    for(size_t i=startl;i<startl+rangel;++i){
                        split_Ba.expand(primitives[i]->get_bbox());
                    }
                    bstack.push(BVHBuildData(split_Ba,startl,rangel,&(*bdata.node)->l));
                }else{
                    split_Ba.expand(primitives[startl]->get_bbox());
                    bstack.push(BVHBuildData(split_Ba,startl,1,&(*bdata.node)->l));
                    ++startr;
                    --ranger;
                }

                if(ranger!=0){
                    for(size_t j=startr;j<startr+ranger;++j){
                        split_Bb.expand(primitives[j]->get_bbox());
                    }
                    bstack.push(BVHBuildData(split_Bb,startr,ranger,&(*bdata.node)->r));
                }else{
                    split_Bb.expand(primitives[startr]->get_bbox());
                    bstack.push(BVHBuildData(split_Bb,startr,1,&(*bdata.node)->r));
                }
            }

            break;
            //SPLIT_EQUAL_COUNTS
            //按照质心大小将所有片元划分为数量相等的两部分（质心排序）
        case SPLIT_EQUAL_COUNTS:
            std::cout<<"SPLIT_EQUAL_COUNTS方法:"<<endl;
            while(!bstack.empty()){

                BVHBuildData bdata = bstack.top();
                bstack.pop();

                //新节点
                *bdata.node = new BVHNode(bdata.bb, bdata.start, bdata.range);

                //叶节点直接跳出循环 不做处理
                if (bdata.range <= max_leaf_size) {
                    continue;
                }
                //找出最大范围轴向dim
                size_t max_dim=bdata.bb.max_dimension();
                //std:nth_element   nth_element(first,nth,last) 将第n_th 元素放到它该放的位置上，左边元素都小于它，右边元素都大于它.
                //lambda表达式  O(N)复杂度
                std::nth_element(primitives.begin()+bdata.start,
                                 primitives.begin()+bdata.start+(bdata.range)/2,
                                 primitives.begin()+bdata.start+bdata.range,
                                 [max_dim](Primitive *iter1,Primitive *iter2){
                                     return iter1->get_bbox().centroid()[max_dim]<iter2->get_bbox().centroid()[max_dim];
                                 }
                );

                BBox split_Ba,split_Bb;
                //左节点
                size_t startl = bdata.start;
                size_t rangel = bdata.range/2;

                for(size_t i=startl;i<startl+rangel;++i){
                    split_Ba.expand(primitives[i]->get_bbox());
                }

                //右节点
                size_t startr = startl+rangel;
                size_t ranger = bdata.start+bdata.range-startr;

                //  cout<<startl<<" "<<rangel<<" "<<startr<<" "<<ranger<<endl;

                for(size_t i=startr;i<startr+ranger;++i){
                    split_Bb.expand(primitives[i]->get_bbox());
                }

                //构造新节点  入栈
                bstack.push(BVHBuildData(split_Ba, startl, rangel, &(*bdata.node)->l));
                bstack.push(BVHBuildData(split_Bb, startr, ranger, &(*bdata.node)->r));
            }
            break;
            //SAH-BUCKET 贪心
        default:
            std::cout<<"SAH-BUCKET 贪心算法:"<<endl;
            fflush(stdout);

            while (!bstack.empty()) {

                // pop work form build stack
                BVHBuildData bdata = bstack.top();
                bstack.pop();


                // create new node
                *bdata.node = new BVHNode(bdata.bb, bdata.start, bdata.range);

                // done if leaf, or fewer primitives than buckets
                if (bdata.range <= max_leaf_size) {
                    continue;
                }

                //当片元数量数量小于bucket数量时，应更新buicket
                size_t num_buckets = std::min(kMaxNumBuckets, bdata.range);

                SAHBucketData* buckets = new SAHBucketData[num_buckets];

                /**
                /*对三个轴都进行尝试 选取具有最佳结果的轴向
                */
                BBox split_Ba;//左A
                BBox split_Bb;//右B
                int split_dim = -1;//划分轴
                size_t split_idx = 1;
                double split_val = 0;
                //未划分前的SAH代价
                double split_cost = bdata.bb.surface_area() * bdata.range;

                // try all three dimensions and find best split
                for (int dim = 0; dim < 3; dim++) {
                    if (bdata.bb.extent[dim] < EPS_D) {
                        continue; // ignore flat dimension
                    }

                    // 初始化各bucket
                    double bucket_width = bdata.bb.extent[dim] / num_buckets;
                    for (size_t i = 0; i < num_buckets; ++i) {
                        buckets[i].bb = BBox();
                        buckets[i].num_prims = 0;
                    }

                    // compute bucket bboxes
                    for (size_t i = bdata.start; i < bdata.start + bdata.range; ++i) {
                        //通过索引得到三角片元Primitive->triangle
                        Primitive *p = primitives[i];
                        BBox pbb = p->get_bbox();
                        double d = (pbb.centroid()[dim] - bdata.bb.min[dim]) / bucket_width;//d=(pbb.centroid()[dim]-bdata.bb.min[dim])/(bdata.bb.extent[dim])*num_buckets;

                        /**
                        * int b=(pbb.centroid()[dim] - bdata.bb.min[dim]) / bucket_width;
                        * if(b==num_buckets){
                        *  	b=num_buckets-1;
                        * }
                        */

                        //b为最后所在bucket索引  C++ clmap(int x, int low, int high) 若X在[low,high]范围内，则等于X；如果X小于low，则返回low；如果X大于high，则返回high。
                        size_t b = misc::clamp((int)d, 0, ((int)num_buckets) - 1);
                        //更新bucket
                        buckets[b].bb.expand(pbb);
                        buckets[b].num_prims++;
                    }


                    /* 遍历所有bucket  SAH代价 Sa * Na左  Sb * Nb右   寻找分割方法使得sah代价最小
                    * @author Eli
                    * @date
                    for(size_t idx_b=0;idx_b<num_buckets;idx_b++){
                        BBox left,right;
                        size_t leftcount=0,rightcount=0;
                        //左bucket
                        for(size_t i=0;i<idx_b;i++){
                        left=left.expand(buckets[i].bb);
                        leftcount+=buckets[i].num_prims;
                        }
                        //右bucket
                        for(size_t j=idx_b;j<num_buckets;j++){
                      right=right.expand(buckets[j].bb);
                      rightcount+=buckets[j].num_prims;
                        }
                        double temp_SAH=leftcount*left.surface_area()+rightcount*right.surface_area();

                        //记录最小代价
                        if(temp_SAH<split_cost){
                      split_cost=temp_SAH;
                      split_Ba=left;
                      split_Bb=right;
                      split_dim=dim;
                        }
                    }
                    */

                    for (size_t idx = 1; idx < num_buckets; ++idx) {
                        // Sa * Na
                        size_t Na = 0;
                        BBox Ba;
                        for (size_t i = 0; i < idx; ++i) {
                            Ba.expand(buckets[i].bb);
                            Na += buckets[i].num_prims;
                        }

                        // Sb * Nb
                        size_t Nb = 0;
                        BBox Bb;
                        for (size_t i = idx; i < num_buckets; ++i) {
                            Bb.expand(buckets[i].bb);
                            Nb += buckets[i].num_prims;
                        }

                        // sah cost & actual split value
                        double cost = Na * Ba.surface_area() + Nb * Bb.surface_area();
                        //val 实际分割坐标值
                        double val = bdata.bb.min[dim] + idx * bucket_width;
                        if (cost < split_cost) {
                            split_Ba = Ba;
                            split_Bb = Bb;
                            split_dim = dim;
                            split_idx = idx;
                            split_val = val;
                            split_cost = cost;
                        }
                    }
                }

                // done with buckets
                delete buckets;

                // edge case - if all dimensions are flat (all centroids on a single spot)
                // split equally into two build nodes with the same bbox
                if (split_dim == -1) {
                    size_t startl = bdata.start;
                    size_t rangel = bdata.range / 2;
                    size_t startr = startl + rangel;
                    size_t ranger = bdata.range - rangel;
                    bstack.push(BVHBuildData(bdata.bb, startl, rangel, &(*bdata.node)->l));
                    bstack.push(BVHBuildData(bdata.bb, startr, ranger, &(*bdata.node)->r));
                    continue;
                }

                // partition primitives  partition函数最后一个参数接受 comparer函数指针  这里使用了lamada表达式
                //std::partition 时间复杂度O（n） 头尾指针分别向中间遍历，找到不符合的位置 交换位置再次遍历 直到begin==end
                auto it =
                        std::partition(primitives.begin() + bdata.start,
                                       primitives.begin() + bdata.start + bdata.range,
                                       [split_dim, split_val](Primitive *p) {
                                           return p->get_bbox().centroid()[split_dim] < split_val;
                                       });
                size_t startl = bdata.start;
                size_t rangel = std::distance(primitives.begin(),it) - bdata.start;
                size_t startr = startl + rangel;
                size_t ranger = bdata.range - rangel;

                // create new build data
                bstack.push(BVHBuildData(split_Ba, startl, rangel, &(*bdata.node)->l));
                bstack.push(BVHBuildData(split_Bb, startr, ranger, &(*bdata.node)->r));
            }
    }

}

BVHAccel::~BVHAccel() {

}


BBox BVHAccel::get_bbox() const {
    return root->bb;
}


//遍历BVH 返回求交次数
size_t BVHAccel::intersect(const Ray &ray) const {

    double t0 = ray.min_t;
    double t1 = ray.max_t;

    size_t count=0;
    // try early exit
    if (!root->bb.intersect(ray, t0, t1)) return 0;

    // create traversal stack
    stack<BVHNode *> tstack;

    // push initial traversal data
    tstack.push(root);

    // process traversal
    BVHNode *l, *r;
    while (!tstack.empty()) {

        // pop traversal data
        BVHNode *current = tstack.top();
        tstack.pop();

        // get children
        l = current->l;
        r = current->r;

        // if leaf
        if (!(l || r)) {
            for (size_t i = 0; i < current->range; ++i) {
               // if (primitives[current->start + i]->intersect(ray)) return true;
               count++;
            }
        }

        // test bboxes
        double tl0 = ray.min_t;
        double tl1 = ray.max_t;
        double tr0 = ray.min_t;
        double tr1 = ray.max_t;
        bool hitL, hitR;
        hitL = (l != NULL) && l->bb.intersect(ray, tl0, tl1);
        hitR = (r != NULL) && r->bb.intersect(ray, tr0, tr1);
        count+=2;
        // both hit
        if (hitL && hitR) {
            tstack.push(l);
            tstack.push(r);
        } else if (hitL) {
            tstack.push(l);
        } else if (hitR) {
            tstack.push(r);
        }
    }
    return count;
}