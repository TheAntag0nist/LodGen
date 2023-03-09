#include <algorithms/algorithms.h>
#include <core.h>

namespace lod_generator{
    lod_core* lod_core::m_core = nullptr;
    
    lod_core::lod_core(){
        m_lods_functions[ITERATIVE_QEM] = lod_QEM_iterative;
        m_lods_functions[HYBRID_QEM] = lod_QEM_hybrid;
        m_lods_functions[BASIC_QEM] = lod_QEM;
    }

    lod_core* lod_core::get_instance(){
        if(m_core == nullptr)
            m_core = new lod_core();
        return m_core;
    }    

    int lod_core::generate_lod(mesh& src_mesh, mesh& dst_mesh, LOD_ALG alg_type){
        auto error = m_core_config.get_default_error();
        
        if(m_lods_functions.find(alg_type) != m_lods_functions.end()){
            auto func = m_lods_functions[alg_type];
            return func(src_mesh, dst_mesh, error);
        }
        else
            // TODO: Add Logging
            return ERR_BASIC;
    }

    void lod_core::read_config_file(std::string config_file){
        m_core_config.read_config(config_file);
    }
}