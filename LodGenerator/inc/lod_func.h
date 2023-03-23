#ifndef LOD_FUNC_H
#define LOD_FUNC_H
#include <mesh.h>

namespace lod_generator{
    // 1. Enumerations with Library Algorithms
    enum LOD_ALG{
        // 1. Quadric Error Metric
        ITERATIVE_QEM,
        HYBRID_QEM,
        BASIC_QEM,
        // 2. Vertex Cluster 
        VERTEX_CLUSTER
    };

    // 2. LOD's Algorithms Function
    typedef int (*lod_func)(mesh&, mesh&, double);
}   

#endif