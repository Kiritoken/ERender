#include "Kmeans.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <queue>
#include <vector>
using namespace std;
#define maxLeafNum 4


  
//构造函数  
Kmeans::Kmeans(size_t iterCount, size_t K, size_t P,vector< Primitive* > primitives){
   m_iterations=iterCount;
   m_K=K;
   m_P=P;
   this->primitives=primitives;
   cluster=new Cluster[m_K];
   children=new Kmeans* [m_K];
   BBox world;
   for(size_t i=0;i<primitives.size();++i){
      world.expand(primitives[i]->get_bbox());
    } 
   
   /*
   //初始点随机选取
   vector <Vector3D> kCentroids=getRandCentroids(world.min,world.max,m_K,5);
    for(size_t i=0;i<m_K;++i){
      cluster[i].representive=BBox(kCentroids[i]);
  }*/
  
  
   vector <BBox> kCentroids=getRandCentroidsOnMesh(m_K,m_P);
   for(size_t i=0;i<m_K;++i){
     cluster[i].representive=kCentroids[i];
  }
}

//析够函数
Kmeans::~Kmeans(){
     delete []cluster;
}

//随机在空间内选点
vector< glm::vec3 > Kmeans::getRandCentroids(glm::vec3  pMin, glm::vec3  pMax, int k,int p){
    //初始随机k个representive
   vector<glm::vec3 > kCentroid;
   double randX,randY,randZ;
   //初始点随机选取 
   srand((unsigned)time(NULL));
   randX=pMin.x+(rand()/double(RAND_MAX))*(pMax.x-pMin.x);   
   
   srand((unsigned)time(NULL));
   randY=pMin.y+(rand()/double(RAND_MAX))*(pMax.y-pMin.y);
   
   srand((unsigned)time(NULL));
   randZ=pMin.z+(rand()/double(RAND_MAX))*(pMax.z-pMin.z);
   
   //第一个随机点
   kCentroid.push_back(glm::vec3 (randX,randY,randZ));
   
   
   srand((unsigned)time(NULL));
   //选取之后k-1个点
   for(int i=1;i<k;++i){
     //随机p个点
      vector<glm::vec3 > tempP;
      for(int j=0;j<p;j++){
	randX=pMin.x+(rand()/double(RAND_MAX))*(pMax.x-pMin.x);
	randY=pMin.y+(rand()/double(RAND_MAX))*(pMax.y-pMin.y);
	randZ=pMin.z+(rand()/double(RAND_MAX))*(pMax.z-pMin.z);
	
        tempP.push_back(glm::vec3 (randX,randY,randZ));
      }

      //选取与之前算出的点距离最远的点作为下一个representive
      int index=0;
      double maxDistance=-1.0f;
      
      for(int k=0;k<p;k++)
	for(int q=0;q<kCentroid.size();q++){
	    double distance=glm::length(tempP[k]-kCentroid[q]);
	    cout<<tempP[k]<<" 与 "<<kCentroid[q]<<" 距离相差: "<<distance<<endl;
	    if(distance>maxDistance){
	       maxDistance=distance;
	       index=k;
	    }
	}
	//cout<<index<<endl;
      kCentroid.push_back(tempP[index]);
  }
    for(auto i:kCentroid)
      cout<<i<<endl;
   return kCentroid;
}
  

//随机选择在模型上的点而不是空间内的点
vector< BBox > Kmeans::getRandCentroidsOnMesh(int k, int p){
   vector <BBox> kCentroids;
   size_t idx_primitive;
   
   //第一个随机点
   srand((unsigned)time(NULL));
   idx_primitive=rand()%primitives.size();
   kCentroids.push_back(primitives[idx_primitive]->get_bbox());
   
    srand((unsigned)time(NULL));
   //选取之后k-1个点
   for(int i=1;i<k;++i){
     //随机p个点
      vector<BBox> tempP;
      for(int j=0;j<p;j++){
        idx_primitive=rand()%primitives.size();
        tempP.push_back(primitives[idx_primitive]->get_bbox());	
      }
      //选取与之前算出的点距离最远的点作为下一个representive
      int index=0;
      double maxDistance=-1.0f;
      
      for(int k=0;k<p;k++)
	for(int q=0;q<kCentroids.size();q++){
	   BBox bb;
	   bb.expand(tempP[k]);
	   bb.expand(kCentroids[q]);
	    double distance=glm::length(bb.extent);
	    if(distance>maxDistance){
	       maxDistance=distance;
	       index=k;
	    }
	}	
      kCentroids.push_back(tempP[index]);
  }
  return kCentroids;
}
  
  
  
  
/**
 * 距离定义  论文中的定义：
 * D（b1,b2）=||b1min-b2min||^2+||b1max-b2max||^2
 * 
 * @author Eli
 * @param b1 AABB
 * @param b2 AABB
 * @return D(b1,b2) double
 */
double Kmeans::calDistance(BBox b1, BBox b2){
  
 
    double min_value=glm::length(b1.min-b2.min);
    double max_value=glm::length(b1.max-b2.max);
    double res=min_value+max_value;
    return res;
    
    
 //  return (b1.centroid()-b2.centroid()).norm2();
}
  

/**
 *Kmeans聚类主要方法 
 * 用到一个递归的过程
 * DFS
 * @author eli
 * @date 2018/12/7
 */
void Kmeans::constructKaryTree()
{
  //构造本层结构
  this->run();
  //对本层中的每个cluster循环构造下层
  for(size_t i=0;i<m_K;i++){
     //叶子cluster 该cluster[i]对应的children为NULL
     if(cluster[i].indexOfPrimitives.size()<maxLeafNum*m_K){
       children[i]=NULL;
       continue;
     }
     //否则DFS
     vector <Primitive* > pTemp;
     for(size_t p=0;p<cluster[i].indexOfPrimitives.size();++p){
          pTemp.push_back(primitives[cluster[i].indexOfPrimitives[p]]);
     }
     children[i]=new Kmeans(m_iterations,m_K,m_P,pTemp);
     children[i]->constructKaryTree();
  }     
}



/**
 * 中序建立二叉树的思想  自底向上构建出以Kmeans* k为根节点的二叉树结构
 * @author eli
 * @date 2018/12/10
 * @param k Kmeans*
 */
void Kmeans::buttom2Top(){
    this->root=agglomerativeClustering();
     for(size_t i=0;i<this->m_K;++i) {
       if(children[i]!=NULL)
	this->children[i]->buttom2Top();
     }
}







/**
 * 参照论文Fast Agglomerative Clustering for Rendering 
 * 实现的自底向上聚类方法
 * @author eli
 * @date 2018/12/7
 * 
 */
KBVHNode* Kmeans::agglomerativeClustering(){
   vector <KBVHNode*> v;
   //初始化
   //cout<<"开始初始化"<<endl;
   for(size_t i=0;i<m_K;++i){
      BBox bb;
      
     if(cluster[i].indexOfPrimitives.size()>0){
      for(size_t j=0;j<cluster[i].indexOfPrimitives.size();++j)
	bb.expand(primitives[cluster[i].indexOfPrimitives[j]]->get_bbox());
      
       v.push_back(new KBVHNode(bb,cluster[i]));
      }
  }
  while(v.size()>1){
      //sah代价最小
      double minCost=numeric_limits< double >::max();
      size_t left,right;
      for(size_t i=0;i<v.size();++i)
	for(size_t j=0;j<v.size();++j){
	    if(i==j)
	      continue;
	  //sa*na+sb*nb
	  double cost=v[i]->c.indexOfPrimitives.size()*v[i]->bb.surface_area()+
	              v[j]->c.indexOfPrimitives.size()*v[j]->bb.surface_area();
          if(cost<minCost){	  
	    minCost=cost;
	    left=i;
	    right=j;
	  }	  
	}
      if(left>right)
      {
	size_t temp=left;
	left=right;
	right=temp;
      }
      
      KBVHNode* newNode=combine(v[left],v[right]);
      //cout<<"*********************"<<endl;
      //cout<<newNode->l->c.indexOfPrimitives.size()<<endl;      
      //cout<<newNode->r->c.indexOfPrimitives.size()<<endl;
      //cout<<"*********************"<<endl;
      v.erase(v.begin()+left);
      v.erase(v.begin()+right-1);
      //add new node 
      v.push_back(newNode);
      //cout<<v.size()<<endl;    
  }
    // cout<<v[0]->l->c.indexOfPrimitives.size()<<endl;      
     //cout<<v[0]->r->c.indexOfPrimitives.size()<<endl;  
  return v[0];
}



KBVHNode* Kmeans::combine(KBVHNode* a,KBVHNode* b){
    Cluster c;
    BBox bb;
    for(size_t i=0;i<a->c.indexOfPrimitives.size();++i){
      c.indexOfPrimitives.push_back(a->c.indexOfPrimitives[i]);
      bb.expand(primitives[a->c.indexOfPrimitives[i]]->get_bbox());
    }
    
    for(size_t i=0;i<b->c.indexOfPrimitives.size();++i){
      c.indexOfPrimitives.push_back(b->c.indexOfPrimitives[i]);
      bb.expand(primitives[b->c.indexOfPrimitives[i]]->get_bbox());
    }
    KBVHNode* res=new KBVHNode(bb,c);
    res->l=a;
    res->r=b;
    return res;
}





void Kmeans::run(){
  for(size_t iter=0;iter<m_iterations;++iter){
    //更新representives cluster重新分配
    for(size_t i=0;i<m_K;++i){
      
      if(iter!=0){
       cluster[i].updateRepresentive(); 
      }
      cluster[i].reset();
      cluster[i].indexOfPrimitives.clear();
    }
        
    //计算距离 分配至最近的cluster
    for(size_t idx_primitives=0;idx_primitives<primitives.size();++idx_primitives){
         //用以记录最近的cluster
         size_t index=0;
	 double minDistance=numeric_limits< double >::max();
	 BBox temp=primitives[idx_primitives]->get_bbox();
	 for(size_t idx_clusters=0;idx_clusters<m_K;++idx_clusters){	    
	     double dist=calDistance(temp,cluster[idx_clusters].representive);
	     if(dist<minDistance){
	       minDistance=dist;
	       index=idx_clusters;
	    }
	}
	//allocate to correct cluster;
	cluster[index].add(idx_primitives,primitives[idx_primitives]->get_bbox());
    }    
  }
// print();
}

void Kmeans::print(){
  cout<<"**********************************"<<endl;
   for(size_t i=0;i<m_K;++i){
     cout<<"#"<<i<<" Cluster: "<<cluster[i].indexOfPrimitives.size()<<endl;
    // cout<<" "<<cluster[i].representive<<endl;
  }
  cout<<"**********************************"<<endl;
  cout<<endl;
}
