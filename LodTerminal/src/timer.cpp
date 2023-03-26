#include <timer.h>

debug_timer::debug_timer() {}
debug_timer::~debug_timer() {}

void debug_timer::start(){
    m_start = std::chrono::high_resolution_clock::now();
}

void debug_timer::stop(){
    m_end = std::chrono::high_resolution_clock::now();
}

double debug_timer::get_delta(){
    m_elapsed = m_end - m_start;
    return m_elapsed.count();
}

std::string debug_timer::to_string(){
    std::stringstream ss;
    ss << m_elapsed.count();
    return ss.str();
}