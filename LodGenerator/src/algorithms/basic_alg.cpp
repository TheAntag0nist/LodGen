#include <algorithms/basic_alg.h>
#include <lod_math.h>

namespace lod_generator{
    int lod_basic_alg(mesh& src_mesh, mesh& dst_mesh, double error){
        // 1. Get vertexes, indexes and count of faces
        std::list<std::pair<uint32_t, uint32_t>> valid_pairs;
        auto& vertexes = src_mesh.get_vertexes_link();
        auto& indexes = src_mesh.get_indexes_link();
        uint64_t cnt_faces = indexes.size() / 3;
        lod_result result = SUCCESS;

        // !: Must be triangulated  
        // 2. Resize normals vector
        std::vector<glm::vec3> normals;
        normals.resize(cnt_faces);

        mesh_data data = {};
        data.vertexes = &vertexes;
        data.indexes = &indexes;
        data.normals = &normals;

        // TODO: Need to configure for calculations on CPU or GPU
        // 3. Get normals for all faces
        result = get_faces_normals(data);
        if(result != SUCCESS)
            return result;

        // 4. Get valid edges
        result = get_valid_pairs(data, valid_pairs);
        if(result != SUCCESS)
            return result;

        

        return SUCCESS;
    }
}