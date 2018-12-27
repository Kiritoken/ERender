//
// Created by eli on 18-12-26.
//

#ifndef ERENDER_VIEWER_H
#define ERENDER_VIEWER_H

#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "../render/render.h"

class Viewer {
public:

    /**
     * 构造函数
     */
    explicit Viewer(std::string title);

    /**
     * 析构函数
     */
    ~Viewer();

    void init();

    void start();

    void set_renderer(Renderer* renderer);
private:
    //标题
    std::string _title;

    static Renderer* renderer;
    // HDPI display
    static bool HDPI;

    // framerate related timeers
    //帧率显示相关
    static int framecount;
    static std::chrono::time_point<std::chrono::system_clock> sys_last;
    static std::chrono::time_point<std::chrono::system_clock> sys_curr;

    // info toggle
    static bool showInfo;

    // window properties
    static GLFWwindow* window;
    static size_t buffer_w;
    static size_t buffer_h;


    /**
  * Main update loop.
  */
    static void update(  );


    // window event callbacks windows事件处理
    static void err_callback( int error, const char* description );
    static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
    static void resize_callback( GLFWwindow* window, int width, int height );
    static void cursor_callback( GLFWwindow* window, double xpos, double ypos );
    static void scroll_callback( GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback( GLFWwindow* window, int button, int action, int mods );
};


#endif //ERENDER_VIEWER_H
