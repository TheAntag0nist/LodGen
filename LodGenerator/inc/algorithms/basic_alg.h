#ifndef BASIC_ALG_H
#define BASIC_ALG_H
#include <lod_math.h>
#include <global.h>
#include <errors.h>
#include <mesh.h>

namespace lod_generator {
    int lod_QEM_iterative(mesh& src_mesh, mesh& dst_mesh, double error);
    int lod_QEM_hybrid(mesh& src_mesh, mesh& dst_mesh, double error);
    int lod_QEM(mesh& src_mesh, mesh& dst_mesh, double error);

    lod_result faces_normals_thread(mesh_data data);
    lod_result valid_pairs_thread(mesh_data data);
}

#endif