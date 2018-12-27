//
// Created by eli on 18-12-26.
//

#ifndef ERENDER_APPLICATION_H
#define ERENDER_APPLICATION_H
#include <iostream>
#include <vector>
#include "../utilis/misc.h"
#include "render.h"
#include "../camera/camera.h"
using namespace std;

class Application: public Renderer {

public:

    Application();

    ~Application();

    //初始化
    void init();

    //渲染
    void render();
    //改变大小
    void resize(size_t w, size_t h);

    //IO事件
    void cursor_event( float x, float y );
    void scroll_event( float offset_x, float offset_y );
    void mouse_event( int key, int event, unsigned char mods );
    void keyboard_event( int key, int event, unsigned char mods  );
    //载入场景信息
    void load();

private:
//模式
    enum Mode {
        EDIT_MODE,
        RENDER_MODE,
        VISUALIZE_MODE
    };
    Mode mode;


    float mouseX, mouseY;
    enum e_mouse_button {
        LEFT   = MOUSE_LEFT,
        RIGHT  = MOUSE_RIGHT,
        MIDDLE = MOUSE_MIDDLE
    };
    bool leftDown;
    bool rightDown;
    bool middleDown;

    void mouse_pressed(e_mouse_button b);   // Mouse pressed.
    void mouse_released(e_mouse_button b);  // Mouse Released.
    void mouse1_dragged(float x, float y);  // Left Mouse Dragged.
    void mouse2_dragged(float x, float y);  // Right Mouse Dragged.
    void mouse_moved(float x, float y);     // Mouse Moved.



    bool show_hud;
    void draw_hud();

    // View Frustrum Variables.
    // On resize, the aspect ratio is changed. On reset_camera, the position and
    // orientation are reset but NOT the aspect ratio.
    Camera camera;
    Camera canonicalCamera;

    void set_projection_matrix();
    void init_camera(CameraInfo& camera, const glm::mat4x4& transform);
    // Resets the camera to the canonical initial view position.
    void reset_camera();
    void set_scroll_rate();

    void to_edit_mode();
    void set_up_pathtracer();
    size_t screenW;
    size_t screenH;

    // Coordinate System //
    bool show_coordinates;
    void draw_coordinates();

    // Length of diagonal of bounding box for the mesh.
    // Guranteed to not have the camera occlude with the mes.
    double canonical_view_distance;

    // Rate of translation on scrolling.
    double scroll_rate;


    // Rendering functions.
    void update_gl_camera();
};


#endif //ERENDER_APPLICATION_H
