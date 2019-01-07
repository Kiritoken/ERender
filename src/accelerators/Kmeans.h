#ifndef KMEANS_H
#define KMEANS_H


#include <vector>
#include <iostream>
#include "../glm/glm.hpp"
#include "../core/bbox.h"
#include "../core/primitive.h"
#include "../utilis/timer.h"

using namespace std;


  
  
  
  
class Cluster{
public:
  BBox representive;
  vector <size_t> indexOfPrimitives;

  //更新中心点
  void updateRepresentive(){
    
    representive=BBox(m_min/=indexOfPrimitives.size(),m_max/=indexOfPrimitives.size());
   // cout<<representive<<endl;
  }  
  
  void reset(){ 
    m_min*=0;
    m_max*=0;
  }
  
  void add(size_t i,BBox bb){ 
    indexOfPrimitives.push_back(i);
    m_min+=bb.min;
    m_max+=bb.max;
  }
  
  
private:
  glm::vec3 m_min;
  glm::vec3 m_max;
};  
  
struct KBVHNode{
  public:
     BBox bb;
     Cluster c;
     KBVHNode* l;
     KBVHNode* r;
     
     inline bool isLeaf() const{ return l == NULL && r == NULL; }
     
     KBVHNode(BBox bb,Cluster c)
              :bb(bb),c(c),l(NULL),r(NULL){}
  };  

  
  
  
class Kmeans{
public:
  Kmeans(){
    cluster=NULL;
    children=NULL;
  }
  
  Kmeans(size_t iterCount,size_t K,size_t P,std::vector<Primitive*> primitives);
 ~Kmeans();
  
  //执行
  void run();
  
  //从上至下构造k叉树
  void constructKaryTree();
  
  //从下之上 凝聚聚类(AC)构造二叉树
  KBVHNode* agglomerativeClustering();
  
  //自底向上递归构造
  void buttom2Top();
  
  //打印结果
  void print();
  
  //迭代次数
  size_t m_iterations;
  //参数K
  size_t m_K;
  //参数P
  size_t m_P;
  
  //聚类体
  Cluster* cluster;
  
  //k_叉树结构
  Kmeans** children;
  
  //本层构造出的二叉树根节点
  KBVHNode* root;

    //输入数据
    vector <Primitive* > primitives;
private:

  //计时用
  Timer timer;
  
 
  //初始化随机点
  vector <glm::vec3> getRandCentroids(glm::vec3 pMin, glm::vec3 pMax, int k,int p);
  
  vector <BBox> getRandCentroidsOnMesh(int k,int p);
  //距离公式
  double calDistance(BBox b1,BBox b2);
  
      
  //合并两个KBVHNode
  KBVHNode* combine(KBVHNode* a,KBVHNode* b);
  
};



#endif//KMEANSBVH_H


