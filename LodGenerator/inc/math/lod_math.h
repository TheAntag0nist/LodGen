#ifndef LOD_MATH_H
#define LOD_MATH_H
// 1. GLM Headers
#include <glm/glm.hpp>
// 2. Inner Headers
#include <global.h>

namespace lod_generator {
///////////////////////////////////////////////////////////////////////////
    // 3. Global Type Definitions
    typedef struct vertex_and_cost;
    typedef struct edge_pair;
    typedef struct v_and_w;
    typedef struct cluster;
///////////////////////////////////////////////////////////////////////////
    // 4. Global Structures
    // 4.1. mesh_data - it's optimization metadata
    //         used in all optimization algorithms
    typedef struct mesh_data{
        // 4.1.1. Global Data
        std::shared_ptr<std::vector<glm::dvec3>> normals; 
        std::shared_ptr<std::vector<uint32_t>> indexes;
        std::shared_ptr<std::vector<double>> vertexes;
        // 4.1.2 Error
        uint32_t max_iterations;
        double algorithm_error;
///////////////////////////////////////////////////////////////////////////
    // 4.2. QEM Algorithm Data
        // 4.2.1. Valid Edges and Faces
        std::shared_ptr<std::vector<edge_pair>> valid_edges;
#ifdef _DEBUG
        std::shared_ptr<std::vector<uint32_t>> valid_face_ids;
#endif
        // 4.2.2. Quadric Costs Data
        std::shared_ptr<std::vector<glm::mat4x4>> face_quadric_errors;
        // 4.2.3. Edges Costs and Vertexes
        std::shared_ptr<std::list<std::pair<vertex_and_cost, edge_pair>>> edge_vertexes;
///////////////////////////////////////////////////////////////////////////
        // 4.3. Vertexes Clustering Algorithm Data
        uint32_t max_k_means = 10;
        uint32_t max_clusters_cnt;

        std::shared_ptr<std::vector<v_and_w>> vertexes_weights;
        std::shared_ptr<std::set<uint32_t>> used_vertexes;
        std::shared_ptr<std::list<cluster>> clusters;
///////////////////////////////////////////////////////////////////////////
    } mesh_data;

    // TODO: Need upgrade search valid pairs algorithm
    // 4.2. valid_edges_data  - lists with valid edges and faces ID's
    typedef struct valid_edges_data {
        std::list<edge_pair>* valid_edges;
        std::list<uint32_t>* valid_faces_ids;
    } valid_edges_data;

    // 4.3. min_vertexes  - List with vertexes that will be replaced
    typedef struct min_vertexes {
        std::list<std::pair<vertex_and_cost, edge_pair>>* min_vertex_for_edge;
    } min_vertexes;

    // 4.4. triangle_data - Triangle vertexes 
    typedef struct triangle_data {
        glm::dvec3 v1;
        glm::dvec3 v2;
        glm::dvec3 v3;
    } tr_data;

    // 4.5. face_data - Surface vertexes ID's
    typedef struct face_data {
        uint32_t v1_id;
        uint32_t v2_id;
        uint32_t v3_id;
    } face_data;

    // 4.6. face_args - Values of the arguments of the plane function
    typedef struct face_args {
        double A;
        double B;
        double C;
        double D;
    } face_args;

    // 4.7. cluster - Representation of vertex cluster
    typedef struct cluster {
        std::list<uint32_t> vertexes_ind;
        glm::dvec3 center;
    } cluster;

    // 4.8. Edge Pair - Pair of two indexes of edge
    typedef struct edge_pair {
        uint32_t v1;
        uint32_t v2;
    } edge_pair;

    // 4.9. Vertex and Cost - Pair of vertex and their cost
    typedef struct vertex_and_cost {
        glm::vec3 vertex;
        double cost;
    } vertex_and_cost;

    // 4.10. Vertex and Weight - Pair of vertex and their weight
    typedef struct v_and_w {
        uint32_t vertex_id;
        double weight;
    } v_and_w;

///////////////////////////////////////////////////////////////////////////
// 5. Global Functions
    int get_vertex_surfaces(uint32_t vertex_id, mesh_data data, std::list<uint32_t>& faces_ids);
    std::set<uint32_t> get_nearest_vertexes(mesh_data data, uint32_t vertex_id);
    double vertex_weight(mesh_data data, uint32_t v_id);
    int get_vertex_weights(mesh_data data);

    glm::dvec3 get_triangle_center(tr_data& data);

    int get_faces_normals_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data);
    glm::dvec3 get_face_normal(const tr_data& data);
    int get_faces_normals(mesh_data data);

    glm::vec3 get_vertex_data(mesh_data data, uint32_t vertex_id);
    tr_data get_triangle_data(mesh_data data, face_data f_data);
    face_data get_face_data(mesh_data data, uint32_t face_id);
///////////////////////////////////////////////////////////////////////////
    // 6. Basic Algorithm
    namespace qem {
        int get_valid_pairs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, valid_edges_data result_data);
        int get_valid_pairs(mesh_data data);

        int compute_faces_errors_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<glm::mat4x4>* errors);
        int compute_faces_errors(mesh_data data); 

        int compute_costs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, min_vertexes result_data);
        int compute_costs(mesh_data data);

        double get_cost(glm::vec4 v, glm::mat4x4 Q);

        lod_result optimize_mesh_iterative(mesh_data data);
        lod_result optimize_mesh_hybrid(mesh_data data);
        lod_result optimize_mesh(mesh_data data);

        uint32_t update_mesh(mesh_data data);
        lod_result qem_cycle(mesh_data data);
    }
///////////////////////////////////////////////////////////////////////////
    // 7. Vertex Clustering Algorithm
    namespace vertex_cluster {
        int search_vertex_clusters(mesh_data data);
        uint32_t update_mesh(mesh_data data);
        int get_centroids(mesh_data data);

        lod_result optimize_mesh(mesh_data data);
    }
///////////////////////////////////////////////////////////////////////////
}

#endif