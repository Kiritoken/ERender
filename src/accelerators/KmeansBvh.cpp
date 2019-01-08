#include "KmeansBvh.h"
#include "Kmeans.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <queue>
#include <stack>
#include <vector>
using namespace std;

  
  //聚类数
static const size_t K=8;  

struct KmeansBuildData {
  KmeansBuildData(BBox bb,KBVHNode **dst)
      : bb(bb), node(dst) {}
  BBox bb;         ///  包围片元的包围盒
  KBVHNode **node;  ///  对应的BVHNODE
};


KmeansBvh::KmeansBvh(const std::vector< Primitive* >& primitives, size_t max_leaf_size)
{
  
  //i k p
  k=new Kmeans(2,8,5,primitives);
  timer.start();
  k->constructKaryTree();
  //k->run();
  timer.stop();
  cout<<"K-aryTree 自顶向下构建共耗时(sec):  "<<timer.duration()<<endl;

  timer.start();
  k->buttom2Top();
  root=k->root;
  timer.stop();
  cout<<"自底向上构建共耗时(sec):  "<<timer.duration()<<endl;
  
  
  
  
 /* 
  for(int i=0;i<8;++i){
    KBVHNode* child=k->children[i]->agglomerativeClustering();
    cout<<child->c.indexOfPrimitives.size()<<endl;
  }*/
}





BBox KmeansBvh::get_bbox() const{
  
}


KmeansBvh::~KmeansBvh(){

}


size_t KmeansBvh::intersect(const Ray &ray) const {
  double t0 = ray.min_t;
  double t1 = ray.max_t;

  size_t count=1;
  // try early exit
  if (!root->bb.intersect(ray, t0, t1)) return 1;

  stack<Kmeans*> kstack;
  kstack.push(k);
  while(!kstack.empty()){

    Kmeans *temp=kstack.top();
    kstack.pop();
    if(temp!=NULL){

      stack<KBVHNode*> bstack;
      bstack.push(temp->root);
      // process traversal
      KBVHNode *l, *r;
      while(!bstack.empty()){
        KBVHNode *current=bstack.top();
        bstack.pop();

        l=current->l;
        r=current->r;
        //判断当前包围盒是否相交
        // test bboxes
        double t0 = ray.min_t;
        double t1 = ray.max_t;

        count++;
        if(!current->bb.intersect(ray,t0,t1)){
          continue;
        }

        double tl0 = ray.min_t;
        double tl1 = ray.max_t;
        double tr0 = ray.min_t;
        double tr1 = ray.max_t;
        bool hitL, hitR;
        hitL = (l != NULL) && l->bb.intersect(ray, tl0, tl1);
        hitR = (r != NULL) && r->bb.intersect(ray, tr0, tr1);
        count+=2;
        //判断左右子树是否相交
        if (hitL && hitR) {
          bstack.push(l);
          bstack.push(r);
        } else if (hitL) {
          bstack.push(l);
        } else if (hitR) {
          bstack.push(r);
        }
      }
      //入栈子节点
      for(size_t i=0;i<temp->m_K;++i)
        kstack.push(temp->children[i]);
    }
  }
  return count;
}
  
