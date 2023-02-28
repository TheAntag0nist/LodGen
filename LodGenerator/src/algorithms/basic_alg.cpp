#include <algorithms/basic_alg.h>

namespace lod_generator{
    int lod_basic_alg(mesh& src_mesh, mesh& dst_mesh, double error){
        // 1. Get vertexes, indexes and count of faces
        auto& vertexes = src_mesh.get_vertexes_link();
        auto& indexes = src_mesh.get_indexes_link();
        uint64_t cnt_faces = indexes.size() / 3;
        lod_result result = SUCCESS;

        // !: Must be triangulated  
        // 2. Resize normals vector
        std::vector<glm::dvec3> normals;
        normals.resize(cnt_faces);

        // 3. Init Mesh Data
        mesh_data data = {};
        data.vertexes = std::shared_ptr<std::vector<double>>(&vertexes, [](std::vector<double>*) {});
        data.indexes = std::shared_ptr<std::vector<uint32_t>>(&indexes, [](std::vector<uint32_t>*) {});
        data.normals = std::shared_ptr<std::vector<glm::dvec3>>(&normals, [](std::vector<glm::dvec3>*) {});
        
        data.valid_pairs = std::make_shared<std::vector<std::pair<uint32_t, uint32_t>>>();
        data.face_quadric_errors = std::make_shared<std::vector<glm::mat4x4>>();
        data.valid_face_ids = std::make_shared<std::vector<uint32_t>>();

        // TODO: Think about thread guard
        // TODO: Need to configure for calculations on CPU or GPU
        // 4. Get normals for all faces && Get valid edges
        std::thread faces_thread(faces_normals_thread, data);
        std::thread edges_thread(valid_pairs_thread, data);
        faces_thread.join();
        edges_thread.join();

        // 5. Compute Quadric Errors Matrixes 
        result = compute_faces_errors(data);
        if(result != SUCCESS)
            return result;

        return result;
    }

    lod_result faces_normals_thread(mesh_data data){
        return get_faces_normals(data);
    }

    lod_result valid_pairs_thread(mesh_data data){
        return get_valid_pairs(data);
    }
}