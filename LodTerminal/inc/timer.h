#ifndef TIMER_H
#define TIMER_H
#include <iostream>
#include <sstream>
#include <chrono>

enum TIME_TYPE {
    MICROSEC,
    MILISEC,
    SEC
};

class debug_timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
    std::chrono::milliseconds m_elapsed;

public:
    debug_timer();
    ~debug_timer();

    void start();
    void stop();

    std::chrono::microseconds get_delta();
    std::string to_string(TIME_TYPE type = MILISEC);
};

#endif