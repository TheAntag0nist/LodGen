#ifndef CORE_H
#define CORE_H
#include <global.h>

namespace lod_generator{
    // 1. LOD Generator Core
    class lod_core{
    private:
        std::map<LOD_ALG, lod_func> m_lods_functions;
        static lod_core* m_core;
        double m_error;
        lod_core();
    
    public:
        static lod_core* get_instance();
        int generate_lod(mesh& src_mesh, mesh& dst_mesh, LOD_ALG alg_type);

        // Error Settings
        void set_error(double error);
        double get_error();

    };
}

#endif