//
// Created by eli on 18-12-27.
//

#ifndef ERENDER_POLYMESH_H
#define ERENDER_POLYMESH_H
#include "../glm/glm.hpp"
#include <vector>
#include <iostream>

//多边形结构体
struct Polygon {

    std::vector<size_t> vertex_indices;   /// 顶点索引
    std::vector<size_t> normal_indices;   ///法向量索引
    std::vector<size_t> texcoord_indices; ///纹理坐标矩阵索引

}; // struct Polygon

typedef std::vector<Polygon> PolyList;
typedef PolyList::iterator PolyListIter;

struct PolymeshInfo  {

    //Vector3D double x,y,z
    std::vector<glm::vec3> vertices;   ///< polygon vertex array 顶点
    std::vector<glm::vec3> normals;    ///< polygon normal array 法向量
    std::vector<glm::vec2> texcoords;  ///< texture coordinate array 纹理坐标

    std::vector<Polygon>  polygons;   ///< polygons
};

std::ostream& operator<<(std::ostream& os,const PolymeshInfo& polymeshInfo );


#endif //ERENDER_POLYMESH_H
