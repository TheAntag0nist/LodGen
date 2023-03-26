#ifndef VERTEX_CLUSTER_H
#define VERTEX_CLUSTER_H
#include <math/lod_math.h>
#include <global.h>
#include <errors.h>
#include <mesh.h>

namespace lod_generator{
///////////////////////////////////////////////////////////////////////////
    int lod_vertex_cluster(mesh& src_mesh, mesh& dst_mesh, double error);
///////////////////////////////////////////////////////////////////////////
}

#endif