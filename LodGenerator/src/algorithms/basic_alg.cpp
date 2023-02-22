#include <algorithms/basic_alg.h>
#include <lod_math.h>

namespace lod_generator{
    int lod_basic_alg(mesh& src_mesh, mesh& dst_mesh, double error){
        // 1. Get vertexes, indexes and count of faces
        auto& vertexes = src_mesh.get_vertexes_link();
        auto& indexes = src_mesh.get_indexes_link();
        uint64_t cnt_faces = indexes.size() / 3;
        
        // !: Must be triangulated  
        // 2. Resize normals vector
        std::vector<glm::vec3> normals;
        normals.resize(cnt_faces);

        // 3. Get normals for all faces
        get_faces_normals(vertexes, indexes, normals);

        return SUCCESS;
    }
}