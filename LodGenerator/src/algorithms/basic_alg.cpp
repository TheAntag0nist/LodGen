#include <algorithms/basic_alg.h>

namespace lod_generator{
    int lod_QEM_iterative(mesh& src_mesh, mesh& dst_mesh, double error){
        lod_result result = SUCCESS;
        
        auto data = prepare_data(src_mesh);
        data.algorithm_error = error;
        qem::optimize_mesh_iterative(data);

        dst_mesh.set_vertexes(*data.vertexes);
        dst_mesh.set_indexes(*data.indexes);
        return result;
    }

    int lod_QEM_hybrid(mesh& src_mesh, mesh& dst_mesh, double error) {
        lod_result result = SUCCESS;

        auto data = prepare_data(src_mesh);
        data.algorithm_error = error;
        qem::optimize_mesh_hybrid(data);

        // 5. Set vectors
        dst_mesh.set_vertexes(*data.vertexes);
        dst_mesh.set_indexes(*data.indexes);
        return result;
    }

    int lod_QEM(mesh& src_mesh, mesh& dst_mesh, double error) {
        lod_result result = SUCCESS;

        auto data = prepare_data(src_mesh);
        data.algorithm_error = error;
        qem::optimize_mesh(data);

        dst_mesh.set_vertexes(*data.vertexes);
        dst_mesh.set_indexes(*data.indexes);
        return result;
    }

    mesh_data prepare_data(mesh& src_mesh){
        auto vertexes = src_mesh.get_vertexes();
        auto indexes = src_mesh.get_indexes();
        uint64_t cnt_faces = indexes.size() / 3;

        std::vector<glm::dvec3> normals;
        normals.resize(cnt_faces);

        mesh_data data = {};
        data.vertexes = std::shared_ptr<std::vector<double>>(&vertexes, [](std::vector<double>*) {});
        data.indexes = std::shared_ptr<std::vector<uint32_t>>(&indexes, [](std::vector<uint32_t>*) {});
        data.normals = std::shared_ptr<std::vector<glm::dvec3>>(&normals, [](std::vector<glm::dvec3>*) {});
        
        data.edge_vertexes = std::make_shared<std::list<std::pair<vertex_and_cost, edge_pair>>>();
        data.valid_edges = std::make_shared<std::vector<std::pair<uint32_t, uint32_t>>>();
        data.face_quadric_errors = std::make_shared<std::vector<glm::mat4x4>>();
#ifdef DEBUG
        data.valid_face_ids = std::make_shared<std::vector<uint32_t>>();
#endif
        return data;
    }
}