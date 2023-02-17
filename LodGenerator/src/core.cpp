#include <core.h>

namespace lod_generator{
    lod_core* lod_core::m_core = nullptr;
    
    lod_core* lod_core::get_instance(){
        if(m_core == nullptr)
            m_core = new lod_core();
        return m_core;
    }    
}