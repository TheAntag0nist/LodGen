#ifndef CORE_H
#define CORE_H
#include <lod_func.h>
#include <global.h>
#include <mesh.h>

namespace lod_generator{
    // 1. LOD Generator Core
    class lod_core{
    private:
        static lod_core* m_core;
        lod_core();
    
    public:
        static lod_core* get_instance();
        int generate_lod(mesh& src_mesh, mesh& dst_mesh, LOD_ALG alg_type);

    };
}

#endif