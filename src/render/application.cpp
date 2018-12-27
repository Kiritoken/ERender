//
// Created by eli on 18-12-26.
//

#include "application.h"
#include "../camera/camera_info.h"
#include <GL/glew.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

//构造函数
Application::Application() {
    //TODO 初始化pathtracer参数
}


//析构函数
Application::~Application() {
    return ;
}

void Application::init() {
    leftDown   = false;
    rightDown  = false;
    middleDown = false;

    show_coordinates = true;
    show_hud = true;

    // Lighting needs to be explicitly enabled.
    glEnable(GL_LIGHTING);

    // Enable anti-aliasing and circular points.
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glEnable(GL_POINT_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);



    //初始化为EDIT_MODE
    mode=EDIT_MODE;

    // Make a dummy camera so resize() doesn't crash before the scene has been
    // loaded.
    // NOTE: there's a chicken-and-egg problem here, because loadScene
    // requires init, and init requires init_camera (which is only called by
    // loadScene).
    screenW = screenH = 600; // Default value
    CameraInfo cameraInfo;
    cameraInfo.hFov = 50;
    cameraInfo.vFov = 35;
    cameraInfo.nClip = 0.01;
    cameraInfo.fClip = 100;
    camera.configure(cameraInfo, screenW, screenH);
}


void Application::render() {
    update_gl_camera();
    switch (mode) {
        case EDIT_MODE:
            //画出坐标系
            if (show_coordinates) {
                draw_coordinates();
            }



            //DynamicScene 中的各个obejct调用各自的方法  画mesh
           // scene->render_in_opengl();
           // if (show_hud) draw_hud();
            break;
            //没有break 继续匹配下一个CASE
        case VISUALIZE_MODE:
            if (show_coordinates) draw_coordinates();
        case RENDER_MODE:
           // pathtracer->update_screen();
            break;
    }
}

void Application::update_gl_camera() {

    // Call resize() every time we draw, since it doesn't seem
    // to get called by the Viewer upon initial window creation
    // (this should probably be fixed!).
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    if (view[2] != screenW || view[3] != screenH) {
        resize(view[2], view[3]);
    }

    // Control the camera to look at the mesh.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const glm::dvec3& c = camera.position();
    const glm::dvec3& r = camera.view_point();
    const glm::dvec3& u = camera.up_dir();

    gluLookAt(c.x, c.y, c.z,
              r.x, r.y, r.z,
              u.x, u.y, u.z);
}



void Application::draw_hud() {
    const size_t size = 16;
    const float x0 = use_hdpi ? screenW - 300 * 2 : screenW - 300;
    const float y0 = use_hdpi ? 128 : 64;
    const int inc  = use_hdpi ? 48  : 24;
    float y = y0 + inc - size;



    // -- First draw a lovely black rectangle.

    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, screenW, screenH);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenW, screenH, 0, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, -1);

    // -- Black with opacity .8;

    glColor4f(0.0, 0.0, 0.0, 0.8);

    float min_x = x0 - 32;
    float min_y = y0 - 32;
    float max_x = screenW;
    float max_y = y;

    float z = 0.0;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);

    glVertex3f(min_x, min_y, z);
    glVertex3f(min_x, max_y, z);
    glVertex3f(max_x, max_y, z);
    glVertex3f(max_x, min_y, z);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void Application::resize(size_t w, size_t h) {
    screenW = w;
    screenH = h;
    camera.set_screen_size(w, h);
    set_projection_matrix();
    if (mode != EDIT_MODE) {
        //TODO
      //  pathtracer->set_frame_size(w, h);
    }
}

void Application::set_projection_matrix() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera.v_fov(),
                   camera.aspect_ratio(),
                   camera.near_clip(),
                   camera.far_clip());
}


//TODO 载入obj模型
void Application::load() {
    CameraInfo *c=new CameraInfo;
    glm::dvec3 c_pos;
    glm::dvec3 c_dir;

    c->hFov=49.1343f;
    c->vFov=28.8415f;
    c->nClip=0.1;
    c->fClip=100;

    glm::mat4x4 transform;
    transform=glm::translate(transform,glm::vec3(0,0.75f,3.0f));
    init_camera(*c,transform);

    glm::dvec3 target(0.157789,0.657444,-0.293694);
    c_dir=glm::dvec3(0,0.351124,0.936329);
    double view_distance=3.12274;
    double min_view_distance=0.156137;
    double max_view_distance=31.2274;

    canonical_view_distance=view_distance/2;
    canonicalCamera.place(target,acos(c_dir.y),atan2(c_dir.x,c_dir.z),view_distance,min_view_distance,max_view_distance);
    camera.place(target,acos(c_dir.y),atan2(c_dir.x,c_dir.z),view_distance,min_view_distance,max_view_distance);
    set_scroll_rate();



    //TODO 载入obj模型

}

void Application::init_camera(CameraInfo& cameraInfo,
                              const glm::mat4x4& transform) {
    camera.configure(cameraInfo, screenW, screenH);
    canonicalCamera.configure(cameraInfo, screenW, screenH);
    set_projection_matrix();
}

void Application::reset_camera() {
    camera.copy_placement(canonicalCamera);
}

void Application::set_scroll_rate() {
    scroll_rate = canonical_view_distance / 10;
}


/**
 *
 * 鼠标监听事件 由viewer dispatch
 * @param x 鼠标x坐标
 * @param y 鼠标y坐标
 */
void Application::cursor_event(float x, float y) {
    //鼠标左键
    if (leftDown && !middleDown && !rightDown) {
        mouse1_dragged(x, y);
    }
    //鼠标右键
    else if (!leftDown && !middleDown && rightDown) {
        mouse2_dragged(x, y);
    } else if (!leftDown && !middleDown && !rightDown) {
        mouse_moved(x, y);
    }

    //记录原来的位置
    mouseX = x;
    mouseY = y;
}



/*
  When in edit mode and there is a selection, move the selection.
  When in visualization mode, rotate.
*/
void Application::mouse1_dragged(float x, float y) {
    if (mode == RENDER_MODE) {
        return;
    }
    //增量
    float dx = (x - mouseX);
    float dy = (y - mouseY);

    //TODO
    camera.rotate_by(dy * (PI / screenH), dx * (PI / screenW));
}

/*
  When the mouse is dragged with the right button held down, translate.
*/
void Application::mouse2_dragged(float x, float y) {
    if (mode == RENDER_MODE) return;
    float dx = (x - mouseX);
    float dy = (y - mouseY);

    // don't negate y because up is down.
    camera.move_by(-dx, dy, canonical_view_distance);
}

void Application::mouse_moved(float x, float y) {
    if (mode != EDIT_MODE) return;
    y = screenH - y; // Because up is down.
    // Converts x from [0, w] to [-1, 1], and similarly for y.
}

void Application::scroll_event(float offset_x, float offset_y) {
    switch(mode) {
        case EDIT_MODE:
        case VISUALIZE_MODE:
            camera.move_forward(-offset_y * scroll_rate);
            break;
        case RENDER_MODE:
            break;
    }
}

void Application::mouse_event(int key, int event, unsigned char mods) {
    switch(event) {
        case EVENT_PRESS:
            switch(key) {
                case MOUSE_LEFT:
                    mouse_pressed(LEFT);
                    break;
                case MOUSE_RIGHT:
                    mouse_pressed(RIGHT);
                    break;
                case MOUSE_MIDDLE:
                    mouse_pressed(MIDDLE);
                    break;
            }
            break;
        case EVENT_RELEASE:
            switch(key) {
                case MOUSE_LEFT:
                    mouse_released(LEFT);
                    break;
                case MOUSE_RIGHT:
                    mouse_released(RIGHT);
                    break;
                case MOUSE_MIDDLE:
                    mouse_released(MIDDLE);
                    break;
            }
            break;
    }
}

//实现render keyboard_event虚函数
void Application::keyboard_event(int key, int event, unsigned char mods) {
    switch (mode) {
        case RENDER_MODE:
            if (event == EVENT_PRESS) {
                switch (key) {
                    case 'e': case 'E':
                        to_edit_mode();
                        break;
                    case 'v': case 'V':
                       // pathtracer->stop();
                        //pathtracer->start_visualizing();
                        mode = VISUALIZE_MODE;
                        break;
                    case 's': case 'S':
                      //  pathtracer->save_image();
                        break;
                    case '+': case '=':
                        //pathtracer->stop();
                     //   pathtracer->increase_area_light_sample_count();
                       // pathtracer->start_raytracing();
                        break;
                    case '-': case '_':
                      //  pathtracer->stop();
                      //  pathtracer->decrease_area_light_sample_count();
                      //  pathtracer->start_raytracing();
                        break;
                    case '[': case ']':
                      //  pathtracer->stop();
                      //  pathtracer->key_press(key);
                       // pathtracer->start_raytracing();
                        break;
                }
            }
            break;
        case VISUALIZE_MODE:
            if (event == EVENT_PRESS) {
                switch(key) {
                    case 'e': case 'E':
                        to_edit_mode();
                        break;
                    case 'r': case 'R':
                    //    pathtracer->stop();
                      //  pathtracer->start_raytracing();
                        mode = RENDER_MODE;
                        break;
                        //空格重置相机
                    case ' ':
                        reset_camera();
                        break;
                        //默认传递给pathtracer的键盘事件
                    default:
                        //   pathtracer->key_press(key);
                        break;

                }
            }
            break;
        case EDIT_MODE:
            if (event == EVENT_PRESS) {
                switch(key) {
                    case 'r': case 'R':
                        //set_up_pathtracer();
                      //  pathtracer->start_raytracing();
                        mode = RENDER_MODE;
                        break;
                        //构造BVH  转换模式为VISUALIZE pathtracer->upadte 更新
                    case 'v': case 'V':
                        //初始化 pathtracer* 场景 相机设置
                      //  set_up_pathtracer();
                        //BVH
                      //  pathtracer->start_visualizing();
                        //将模式转换为VISUALIZE_MODE
                        mode = VISUALIZE_MODE;
                        break;
                    case ' ':
                        reset_camera();
                        break;
                    case 'h': case 'H':
                        show_hud = !show_hud;
                        break;
                    case 'u': case 'U':
                      //  scene->upsample_selected_mesh();
                        break;
                    case 'd': case 'D':
                     //   scene->downsample_selected_mesh();
                        break;
                    case 'i': case 'I':
                        // i for isotropic.
                      //  scene->resample_selected_mesh();
                        break;
                    case 'f': case 'F':
                      //  scene->flip_selected_edge();
                        break;
                    case 's': case 'S':
                     //   scene->split_selected_edge();
                        break;
                    case 'c': case 'C':
                     //   scene->collapse_selected_edge();
                        break;
                    default:
                        break;
                }
            }
            break;
    }
}

void Application::mouse_pressed(e_mouse_button b) {
    switch (b) {
        case LEFT:
            if (mode == EDIT_MODE) {
/*                if (scene->has_hover()) {
                    scene->confirm_selection();
                } else {
                    scene->invalidate_selection();
                }*/
            }
            leftDown = true;
            break;
        case RIGHT:
            rightDown = true;
            break;
        case MIDDLE:
            middleDown = true;
            break;
    }
}

void Application::mouse_released(e_mouse_button b) {
    switch (b) {
        case LEFT:
            leftDown = false;
            break;
        case RIGHT:
            rightDown = false;
            break;
        case MIDDLE:
            middleDown = false;
            break;
    }
}

void Application::to_edit_mode() {
    if (mode == EDIT_MODE) return;
  //  pathtracer->stop();
    //pathtracer->clear();
    mode = EDIT_MODE;
    mouse_moved(mouseX, mouseY);
}


//TODO
void Application::set_up_pathtracer() {
    if (mode != EDIT_MODE) return;
   // pathtracer->set_camera(&camera);
   // pathtracer->set_scene(scene->get_static_scene());
  //  pathtracer->set_frame_size(screenW, screenH);

}

void Application::draw_coordinates() {

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    //画出右手坐标系
    glBegin(GL_LINES);
    //x轴红色
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    glVertex3i(0,0,0);
    glVertex3i(1,0,0);

    //y轴绿色
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
    glVertex3i(0,0,0);
    glVertex3i(0,1,0);

    //z轴蓝色
    glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    glVertex3i(0,0,0);
    glVertex3i(0,0,1);

    //XOZ平面网格坐标 8*8
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    for (int x = 0; x <= 8; ++x) {
        glVertex3i(x - 4, 0, -4);
        glVertex3i(x - 4, 0,  4);
    }
    for (int z = 0; z <= 8; ++z) {
        glVertex3i(-4, 0, z - 4);
        glVertex3i( 4, 0, z - 4);
    }
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

}