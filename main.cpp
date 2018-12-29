/**
 * ERender #https://github.com/Kiritoken/ERender
 * 该工程主要完成了BVH加速结构的构建，构建方法包括SAH-Based split-mid equal-split
 * 还包括了LBVH 以及Kmeans 构建BVH
 * @author Eli
 * @date
 * @e-mail 357449971@qq.com
 */
#include <iostream>
#include "src/viewer/viewer.h"
#include "src/render/application.h"
#include "src/obj_parser/obj_parser.h"
#include "src/glm/glm.hpp"
using namespace std;
using std::string;

int main(int argc,char** argv){

    //TODO 载入模型信息
    string filePath="../models/buddha.obj";
    auto polymeshInfo=new PolymeshInfo;
    int ret=ObjParser::load(filePath.c_str(),polymeshInfo);

    if(ret){
        exit(1);
    }
    cout<<*polymeshInfo;
    //启动viewer
    Viewer viewer("ERender");
    Application app;

    viewer.set_renderer(&app);
    viewer.init();
    //TODO load obj
    app.load(polymeshInfo);
    viewer.start();

    delete polymeshInfo;
    return 0;
}
