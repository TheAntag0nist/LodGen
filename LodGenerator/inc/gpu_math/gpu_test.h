#ifndef GPU_TEST_H
#define GPU_TEST_H
#include <mesh.h>
#include <string>

namespace lod_generator{
    void test_gpu_core();
    void test_get_valid_pairs(mesh& src_mesh);
}

#endif