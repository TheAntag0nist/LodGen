#ifndef LOD_MATH_H
#define LOD_MATH_H
// 1. GLM Headers
#include <glm/glm.hpp>
// 2. Inner Headers
#include <global.h>

namespace lod_generator {
    // TODO: Rewrite as editable parameter
    const double edge_threshold = 0.01f;

    typedef struct mesh_data{
        std::vector<glm::vec3>* normals; 
        std::vector<uint32_t>* indexes;
        std::vector<double>* vertexes;

        std::list<std::pair<uint32_t, uint32_t>>* valid_pairs;
        std::list<uint32_t>* valid_face_ids;
    } mesh_data;

    int get_faces_normals_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data);
    glm::vec3 get_face_normal(const glm::vec3,const glm::vec3,const glm::vec3);
    int get_faces_normals(const mesh_data data);

    int get_valid_pairs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<std::pair<uint32_t, uint32_t>>* valid_pairs);
    int get_valid_pairs(mesh_data data);

    int compute_quadric_faces_errors(mesh_data data);
}

#endif