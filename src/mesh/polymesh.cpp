//
// Created by eli on 18-12-28.
//
#include "polymesh.h"


std::ostream& operator<<(std::ostream& os,const PolymeshInfo& polymeshInfo ){
    return os<<"Vertices: "<<polymeshInfo.vertices.size()<<std::endl
             <<"Faces: "<<polymeshInfo.polygons.size()<<std::endl
             <<"Normals: "<<polymeshInfo.normals.size()<<std::endl;
}