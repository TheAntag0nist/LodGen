#ifndef BASIC_ALG_H
#define BASIC_ALG_H
#include <global.h>
#include <errors.h>
#include <mesh.h>

namespace lod_generator {
    int lod_basic_alg(mesh* src_mesh, mesh* dst_mesh, double error);
}

#endif