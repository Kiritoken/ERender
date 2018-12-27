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
#include "src/glm/glm.hpp"
using namespace std;

int main(int argc,char** argv){

    //TODO 载入模型信息

    //启动viewer
    Viewer viewer("ERender");
    Application app;

    viewer.set_renderer(&app);

    viewer.init();

    //TODO load obj
    app.load();
    viewer.start();

    return 0;
}
