#include <algorithms/vertex_cluster.h>

namespace lod_generator{
    int lod_vertex_cluster(mesh& src_mesh, mesh& dst_mesh, double error){
        auto vertexes = src_mesh.get_vertexes();
        auto indexes = src_mesh.get_indexes();
        uint64_t cnt_faces = indexes.size() / 3;
        lod_result result = SUCCESS;

        std::vector<glm::dvec3> normals;
        normals.resize(cnt_faces);

        mesh_data data = {};
        data.vertexes = std::shared_ptr<std::vector<double>>(&vertexes, [](std::vector<double>*) {});
        data.indexes = std::shared_ptr<std::vector<uint32_t>>(&indexes, [](std::vector<uint32_t>*) {});
        data.normals = std::shared_ptr<std::vector<glm::dvec3>>(&normals, [](std::vector<glm::dvec3>*) {});
        
        data.face_quadric_errors = nullptr;
        data.edge_vertexes = nullptr;
        data.valid_edges = nullptr;
        data.algorithm_error = error;

        vertex_cluster::optimize_mesh(data);

        dst_mesh.set_vertexes(*data.vertexes);
        dst_mesh.set_indexes(*data.indexes);
        return SUCCESS;
    }
}