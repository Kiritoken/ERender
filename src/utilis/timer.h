//
// Created by eli on 18-12-27.
//

#ifndef ERENDER_TIMER_H
#define ERENDER_TIMER_H
#include <chrono>


class Timer{
private:
    std::chrono::time_point<std::chrono::steady_clock> t0;
    std::chrono::time_point<std::chrono::steady_clock> t1;
public:
    inline void start(){ t0=std::chrono::steady_clock::now(); }
    inline void stop() { t1=std::chrono::steady_clock::now(); }
    inline double duration(){ return (std::chrono::duration<double> (t1-t0)).count(); }
};


#endif //ERENDER_TIMER_H
