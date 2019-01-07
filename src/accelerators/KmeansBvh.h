#ifndef KMEANSBVH_H
#define KMEANSBVH_H


#include "Kmeans.h"
#include "../core/aggregate.h"
#include <vector>

using namespace std;



   
  /**
   * Kmeans聚类实现BHV构建
   * 参考论文: Parallel BVH Construction using k-means Clustering
   * 
   * @author Eli
   * @date 2018/11/22
   */
class KmeansBvh : public Aggregate{
  public:
     
    
    KmeansBvh(){}
    
    //构造函数
    KmeansBvh(const std::vector<Primitive*>& primitives, size_t max_leaf_size);
    //析构函数
    ~KmeansBvh();
  
    BBox get_bbox() const;

  
    void draw(const Color& c) const { }

    void drawOutline(const Color& c) const { }
    
    Kmeans* getKmeans(){ return k; }
    
    KBVHNode* getRoot(){return root;}
    
private:
    KBVHNode* root;
    Kmeans* k;
    //计时用
    Timer timer;
  };



#endif//KMEANSBVH_H


