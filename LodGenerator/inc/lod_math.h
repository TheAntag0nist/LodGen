#ifndef LOD_MATH_H
#define LOD_MATH_H
// 1. GLM Headers
#include <glm/glm.hpp>
// 2. Inner Headers
#include <global.h>

namespace lod_generator {
///////////////////////////////////////////////////////////////////////////
// 3. Global Constants
    // TODO: Rewrite as editable parameter
    const double edge_threshold = 0.01f;
///////////////////////////////////////////////////////////////////////////
// 4. Global Structures
    typedef struct mesh_data{
        // 4.1. Global Data
        std::vector<glm::vec3>* normals; 
        std::vector<uint32_t>* indexes;
        std::vector<double>* vertexes;

        // 4.2. Valid Edges and Faces
        std::vector<std::pair<uint32_t, uint32_t>>* valid_pairs;
        std::vector<uint32_t>* valid_face_ids;

        // 4.3. Quadric Costs Data
        std::vector<glm::mat4x4>* face_quadric_errors;
    } mesh_data;

    typedef struct triangle_data{
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 v3;
    } tr_data;

    typedef struct face_data{
        uint32_t v1_id;
        uint32_t v2_id;
        uint32_t v3_id;
    } face_data;

    typedef struct face_args{
        double A;
        double B;
        double C;
        double D;
    } face_args;

///////////////////////////////////////////////////////////////////////////
// 5. Global Functions
    int get_faces_normals_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data);
    glm::vec3 get_face_normal(const tr_data& data);
    int get_faces_normals(const mesh_data data);

    tr_data get_triangle_data(mesh_data data, face_data f_data);
    face_data get_face_data(mesh_data data, uint32_t face_id);
///////////////////////////////////////////////////////////////////////////
// 6. Basic Algorithm
    int get_valid_pairs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<std::pair<uint32_t, uint32_t>>* valid_pairs);
    int get_valid_pairs(mesh_data data);

    int compute_faces_errors_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<glm::mat4x4>* errors);
    int compute_faces_errors(mesh_data data); 
///////////////////////////////////////////////////////////////////////////
}

#endif