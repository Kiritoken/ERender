//
// Created by eli on 18-12-27.
//

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include "obj_parser.h"
#include <iostream>

Timer ObjParser::timer;


/**
 * 载入obj 使用tinyobj
 * @param filename  文件名
 * @param polymeshInfo polymesh
 * @return 状态 0成功 1失败
 */
int ObjParser::load(const char *filename, PolymeshInfo *polymeshInfo) {
    timer.start();
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, nullptr,true);

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
       return 1;
    }
  //  std::cout<<shapes[0].mesh.num_face_vertices.size()<<std::endl;
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            Polygon polygon;
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];


                polygon.normal_indices.push_back(idx.normal_index);
                polygon.vertex_indices.push_back(idx.vertex_index);
                polygon.texcoord_indices.push_back(idx.texcoord_index);

            }
            polymeshInfo->polygons.push_back(polygon);
            index_offset += fv;

            // per-face material
            // shapes[s].mesh.material_ids[f];
        }
    }


    if(attrib.vertices.size()%3==0) {
        polymeshInfo->vertices.reserve(attrib.vertices.size());
        for (size_t i = 0; i < attrib.vertices.size(); ++i) {

            polymeshInfo->vertices.emplace_back(
                    glm::dvec3(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]));
            i += 2;
        }
    }else{
        std::cerr<<"顶点数目不正确!"<<std::endl;
        return 1;
    }

    if(attrib.normals.size()%3==0) {
        polymeshInfo->normals.reserve(attrib.normals.size());
        for (size_t i = 0; i < attrib.normals.size(); ++i) {
            polymeshInfo->normals.emplace_back(
                    glm::dvec3(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]));
            i += 2;
        }
    }else{
        std::cerr<<"法向量数目不正确!"<<std::endl;
        return 1;
    }

    if(attrib.texcoords.size()%2==0){
        polymeshInfo->texcoords.reserve(attrib.texcoords.size());
        for(size_t i=0;i<attrib.texcoords.size();++i){

            polymeshInfo->texcoords.emplace_back(glm::dvec2(attrib.texcoords[i],attrib.texcoords[i+1]));
            i+=1;
        }
    }else{
        std::cerr<<"贴图数目不正确!"<<std::endl;
        return 1;
    }
    timer.stop();
    std::cout<<"解析OBJ模型共耗时(s):"<<timer.duration()<<std::endl;
    return 0;
}