#include "KmeansBvh.h"
#include "Kmeans.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <queue>
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





BBox KmeansBvh::get_bbox() const
{
  
}


KmeansBvh::~KmeansBvh()
{

}

  
