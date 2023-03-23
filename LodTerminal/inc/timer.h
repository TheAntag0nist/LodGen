#ifndef TIMER_H
#define TIMER_H
#include <iostream>
#include <sstream>
#include <chrono>

class debug_timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
    std::chrono::duration<double, std::milli> m_elapsed;

public:
    debug_timer();
    ~debug_timer();

    void start();
    void stop();

    std::string to_string();
    double get_delta();
};

#endif