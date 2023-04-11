#include <timer.h>

debug_timer::debug_timer() {}
debug_timer::~debug_timer() {}

void debug_timer::start(){
    m_start = std::chrono::high_resolution_clock::now();
}

void debug_timer::stop(){
    m_end = std::chrono::high_resolution_clock::now();
    m_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start);
}

std::chrono::microseconds debug_timer::get_delta(){
    return m_elapsed;
}

std::string debug_timer::to_string(TIME_TYPE tm_type){
    std::stringstream ss;

    switch(tm_type){
        case MICROSEC:{
            auto value_micro = std::chrono::duration_cast<std::chrono::microseconds>(m_elapsed);
            ss << value_micro.count();
            break;
        }
        case MILISEC:{
            ss << m_elapsed.count();
            break;
        }
        case SEC:{
            auto value_sec = std::chrono::duration_cast<std::chrono::seconds>(m_elapsed);
            ss << value_sec.count();
            break;
        }
        default:{
            ss << m_elapsed.count();
            break;
        }
    }

    return ss.str();
}