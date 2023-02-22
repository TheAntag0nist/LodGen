#include <algorithms/algorithms.h>
#include <core.h>

namespace lod_generator{
    lod_core* lod_core::m_core = nullptr;
    
    lod_core::lod_core(){
        m_lods_functions[BASIC_ALG] = lod_basic_alg;
    }

    lod_core* lod_core::get_instance(){
        if(m_core == nullptr)
            m_core = new lod_core();
        return m_core;
    }    

    int lod_core::generate_lod(mesh& src_mesh, mesh& dst_mesh, LOD_ALG alg_type){
        if(m_lods_functions.find(alg_type) != m_lods_functions.end()){
            auto func = m_lods_functions[alg_type];
            return func(src_mesh, dst_mesh, m_error);
        }
        else
            // TODO: Add Logging
            return ERR_BASIC;
    }

    void lod_core::set_error(double error){
        m_error = error;
    }

    double lod_core::get_error(){
        return m_error;
    }
}