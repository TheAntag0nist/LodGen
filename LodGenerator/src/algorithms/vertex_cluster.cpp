#include <algorithms/vertex_cluster.h>

namespace lod_generator{
    int lod_vertex_cluster(mesh& src_mesh, mesh& dst_mesh, double error){
        auto& vertexes = src_mesh.get_vertexes_link();
        auto& indexes = src_mesh.get_indexes_link();
        uint64_t cnt_faces = indexes.size() / 3;
        lod_result result = SUCCESS;

        std::vector<glm::dvec3> normals;
        normals.resize(cnt_faces);

        mesh_data data = {};
        data.vertexes = std::shared_ptr<std::vector<double>>(&vertexes, [](std::vector<double>*) {});
        data.indexes = std::shared_ptr<std::vector<uint32_t>>(&indexes, [](std::vector<uint32_t>*) {});
        data.normals = std::shared_ptr<std::vector<glm::dvec3>>(&normals, [](std::vector<glm::dvec3>*) {});
        
        data.vertexes_weights = std::make_shared<std::vector<v_and_w>>();
        data.used_vertexes = std::make_shared<std::set<uint32_t>>();
        data.clusters = std::make_shared<std::list<cluster>>();

        data.max_clusters_cnt = std::ceil(data.indexes->size() / 100);
        data.max_iterations = 3 * (indexes.size() / 10.0f);
        data.algorithm_error = error;

        vertex_cluster::optimize_mesh(data);

        dst_mesh.set_vertexes(*data.vertexes);
        dst_mesh.set_indexes(*data.indexes);
        return result;
    }
}