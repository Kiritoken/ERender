//
// Created by eli on 18-12-26.
//

#include "viewer.h"
#include <iostream>


using namespace std;
using namespace chrono;

//默认窗口大小
#define DEFAULT_W 960
#define DEFAULT_H 640

// HDPI display
bool Viewer::HDPI;
Renderer* Viewer::renderer;

// framecount & related timeers
int Viewer::framecount;
time_point<system_clock> Viewer::sys_last;
time_point<system_clock> Viewer::sys_curr;

// draw toggles
bool Viewer::showInfo = true;

// window properties
GLFWwindow* Viewer::window;
size_t Viewer::buffer_w;
size_t Viewer::buffer_h;


Viewer::Viewer(std::string title):_title(title){}


//析构函数
Viewer::~Viewer() {
    glfwDestroyWindow(window);
    glfwTerminate();

    //TODO
    // free resources
    delete renderer;
}


void Viewer::set_renderer(Renderer *renderer) {
    this->renderer=renderer;
}

void Viewer::init() {
    // initialize glfw
    glfwSetErrorCallback( err_callback );
    if( !glfwInit() ) {
        cerr<<"Error: could not initialize GLFW!"<<endl;
        exit( 1 );
    }

    // create window
    //string title = renderer ? _title + " " + renderer->name() : _title;
    window = glfwCreateWindow( DEFAULT_W, DEFAULT_H, _title.c_str(), NULL, NULL );
    if (!window) {
        cerr<<"Error: could not create window!"<<endl;
        glfwTerminate();
        exit( 1 );
    }

    // set context
    glfwMakeContextCurrent( window );
    glfwSwapInterval(1);

    //注册回调事件
    // framebuffer event callbacks
    glfwSetFramebufferSizeCallback( window, resize_callback );

    // key event callbacks 键盘回调函数
    glfwSetKeyCallback( window, key_callback );

    // cursor event callbacks
    glfwSetCursorPosCallback( window, cursor_callback );

    // wheel event callbacks
    glfwSetScrollCallback(window, scroll_callback);

    // mouse button callbacks
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // initialize glew
    if (glewInit() != GLEW_OK) {
        cerr<<"Error: could not initialize GLEW!"<<endl;
        glfwTerminate();
        exit( 1 );
    }

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // resize components to current window size, get DPI
    glfwGetFramebufferSize(window, (int*) &buffer_w, (int*) &buffer_h );
    if( buffer_w > DEFAULT_W ) HDPI = true;

    //TODO
    // initialize renderer if already set
    if (renderer){
        if (HDPI) renderer->use_hdpi_reneder_target();
        renderer->init();
    }

    // resize elements to current size
    resize_callback(window, buffer_w, buffer_h);

}

void Viewer::start() {

    // start timer
    sys_last = system_clock::now();

    // run update loop
    while( !glfwWindowShouldClose( window ) ) {
        update();
    }
}



//viewer 每帧更新画面
void Viewer::update() {


    // clear frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // run user renderer
    if (renderer) {
        renderer->render();
    }

    // swap buffers
    glfwSwapBuffers(window);

    // poll events
    glfwPollEvents();
}



void Viewer::err_callback( int error, const char* description ) {
    cerr<<"GLFW Error: "<<description<<endl;
}

void Viewer::resize_callback( GLFWwindow* window, int width, int height ) {

    // get framebuffer size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h );

    // update buffer size
    buffer_w = w; buffer_h = h;
    glViewport( 0, 0, buffer_w, buffer_h );


/*    // resize render if there is a user space renderer
    if (renderer) renderer->resize( buffer_w, buffer_h );*/
}

void Viewer::cursor_callback( GLFWwindow* window, double xpos, double ypos ) {

    // forward pan event to renderer
    if( HDPI ) {
        float cursor_x = 2 * xpos;
        float cursor_y = 2 * ypos;
        //TODO
        renderer->cursor_event(cursor_x, cursor_y);
    } else {
        float cursor_x = xpos;
        float cursor_y = ypos;
        //TODO
        renderer->cursor_event(cursor_x, cursor_y);
    }

}

void Viewer::scroll_callback( GLFWwindow* window, double xoffset, double yoffset) {

    //TODO render scroll
    renderer->scroll_event(xoffset, yoffset);

}


void Viewer::mouse_button_callback( GLFWwindow* window, int button, int action, int mods ) {

    //TODO render mouse event
    renderer->mouse_event( button, action, mods );

}



/**
 * @param window
 * @param key 按键
 * @param scancode
 * @param action GLFW_RELEASE 松开 GLFW_PRESS 按下 GLFW_REPEAT 重复，一直按着
 * @param mods GLFW_MOD_SHIFT 按下Shift键 GLFW_MOD_CONTROL 按下Ctrl键
 */
void Viewer::key_callback( GLFWwindow* window,
                           int key, int scancode, int action, int mods ) {

    //键盘按下
    if (action == GLFW_PRESS) {
        if( key == GLFW_KEY_ESCAPE ) {
            glfwSetWindowShouldClose( window, true );
        }
            //'~'  关闭info显示
        else if( key == GLFW_KEY_GRAVE_ACCENT ){
            showInfo = !showInfo;
        }
    }

    //TODO render
    renderer->keyboard_event( key, action, mods );
}