#ifndef LOD_FUNC_H
#define LOD_FUNC_H
#include <global.h>
#include <mesh.h>

namespace lod_generator{
    // 1. Enumerations with Library Algorithms
    enum LOD_ALG{
        BASIC_ALG
    };

    // 2. LOD's Algorithms Function
    typedef void (*lod_func)(mesh* src, mesh* dest, float error);
}   

#endif