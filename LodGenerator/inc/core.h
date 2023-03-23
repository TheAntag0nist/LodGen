#ifndef CORE_H
#define CORE_H
#include <core_config.h>
#include <lod_func.h>
#include <global.h>
#include <map>

namespace lod_generator{
    // 1. LOD Generator Core
    class lod_core{
    private:
        std::map<LOD_ALG, lod_func> m_lods_functions;
        core_config m_core_config;
        
        static lod_core* m_core;
        lod_core();
    
    public:
        int generate_lod(mesh& src_mesh, mesh& dst_mesh, LOD_ALG alg_type);
        void read_config_file(std::string config_file);
        static lod_core* get_instance();
    };
}

#endif