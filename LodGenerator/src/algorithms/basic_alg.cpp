#include <algorithms/basic_alg.h>
#include <lod_math.h>

namespace lod_generator{
    int lod_basic_alg(mesh& src_mesh, mesh& dst_mesh, double error){
        // 1. Get vertexes, indexes and count of faces
        auto& vertexes = src_mesh.get_vertexes_link();
        auto& indexes = src_mesh.get_indexes_link();
        uint64_t cnt_faces = indexes.size() / 3;
        lod_result result = SUCCESS;

        // !: Must be triangulated  
        // 2. Resize normals vector
        std::vector<glm::vec3> normals;
        normals.resize(cnt_faces);

        // Init Mesh Data
        mesh_data data = {};
        data.vertexes = &vertexes;
        data.indexes = &indexes;
        data.normals = &normals;
        
        data.valid_pairs = new std::vector<std::pair<uint32_t, uint32_t>>;
        data.face_quadric_errors = new std::vector<glm::mat4x4>;
        data.valid_face_ids = new std::vector<uint32_t>;

        // TODO: Need to configure for calculations on CPU or GPU
        // 3. Get normals for all faces
        result = get_faces_normals(data);
        if(result != SUCCESS)
            return result;

        // 4. Get valid edges
        result = get_valid_pairs(data);
        if(result != SUCCESS)
            return result;

        // 5. Compute Quadric Errors Matrixes 
        result = compute_faces_errors(data);
        if(result != SUCCESS)
            return result;

        return result;
    }
}