#include <lod_math.h>

namespace lod_generator {
    int get_vertex_surfaces(uint32_t vertex_id, mesh_data data, std::list<uint32_t>& faces_ids) {
        auto& indexes_ptr = data.indexes;

        for (int i = 0; i < indexes_ptr->size(); i += 3) {
            if (vertex_id == (*indexes_ptr)[i] ||
                vertex_id == (*indexes_ptr)[i + 1] ||
                vertex_id == (*indexes_ptr)[i + 2])
                faces_ids.push_back(i / 3);
        }

        return SUCCESS;
    }

    double get_cost(glm::vec4 v, glm::mat4x4 Q) {
        double result = 0.0f;

        glm::vec4 temp = {
            v.x * Q[0][0] + v.y * Q[1][0] + v.z * Q[2][0] + v.w * Q[3][0],
            v.x * Q[0][1] + v.y * Q[1][1] + v.z * Q[2][1] + v.w * Q[3][1],
            v.x * Q[0][2] + v.y * Q[1][2] + v.z * Q[2][2] + v.w * Q[3][2],
            v.x * Q[0][3] + v.y * Q[1][3] + v.z * Q[2][3] + v.w * Q[3][3]
        };

        result = temp.x * v.x + temp.y * v.y + temp.z * v.z + temp.w * v.w;

        return result;
    }

    glm::vec3 get_vertex_data(mesh_data data, uint32_t vertex_id) {
        auto real_vertex_id = vertex_id * 3;
        auto& vertexes_ptr = data.vertexes;

        auto result = glm::vec3(
            (*vertexes_ptr)[real_vertex_id],
            (*vertexes_ptr)[real_vertex_id + 1],
            (*vertexes_ptr)[real_vertex_id + 2]
        );
        return result;
    }

    tr_data get_triangle_data(mesh_data data, face_data f_data){
        auto& vertexes = *(data.vertexes);
        tr_data result = {};

        auto ind_1 = f_data.v1_id * 3;
        auto ind_2 = f_data.v2_id * 3;
        auto ind_3 = f_data.v3_id * 3; 

        result.v1 = glm::dvec3(vertexes[ind_1], vertexes[ind_1 + 1], vertexes[ind_1 + 2]);
        result.v2 = glm::dvec3(vertexes[ind_2], vertexes[ind_2 + 1], vertexes[ind_2 + 2]);
        result.v3 = glm::dvec3(vertexes[ind_3], vertexes[ind_3 + 1], vertexes[ind_3 + 2]);

        return result;
    }

    face_data get_face_data(mesh_data data, uint32_t face_id){
        auto& indexes = *(data.indexes);
        face_data result = {};

        // TODO: rewrite on logging
        if (face_id > indexes.size() / 3)
            assert(false);

        result.v1_id = indexes[face_id * 3];
        result.v2_id = indexes[face_id * 3 + 1];
        result.v3_id = indexes[face_id * 3 + 2];

        return result;
    }

    int get_face_normal(const tr_data& data, face_args& args){
        glm::dvec3 face_normal = {0,0,0};

        glm::dvec3 tmp_1 = data.v2 - data.v1;
        glm::dvec3 tmp_2 = data.v3 - data.v1;

        args.A = tmp_1.y * tmp_2.z - tmp_2.y * tmp_1.z;
        args.B = tmp_2.x * tmp_1.z - tmp_1.x * tmp_2.z;
        args.C = tmp_1.x * tmp_2.y - tmp_1.y * tmp_2.x;
        args.D = (- args.A * data.v1.x - args.B * data.v1.y - args.C * data.v1.z);

        return SUCCESS;
    }

    int get_faces_normals_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data){
        // 0. Data Section
        uint32_t start_block = thread_id * split_size;
        auto& vertexes = data.vertexes;
        auto& indexes = data.indexes;
        auto& normals = data.normals;

        auto temp_block = start_block + split_size;
        auto end_block = temp_block < normals->size() ? temp_block : normals->size();

        // 1. Calculate normals
        for(int index_id = start_block; index_id < end_block; ++index_id){
            face_data f_data = get_face_data(data, index_id);
            tr_data t_data = get_triangle_data(data, f_data);

            face_args result = {};
            get_face_normal(t_data, result);
            glm::dvec3 norm = glm::normalize(glm::dvec3(result.A, result.B, result.C));
            (*normals)[index_id] = norm;
        }

        return SUCCESS;
    }

    int get_faces_normals(const mesh_data data){
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();
        std::vector<std::shared_ptr<std::thread>> threads;
        
        // 1. Check threads count
        if (num_threads == 0){
            std::cout << "No threads" << std::endl;    
            return ERR_THREAD_NUMBER;
        }
        else {
            auto norm_size = data.normals->size();
            auto split_size = norm_size / num_threads;
            
            while (split_size * num_threads < norm_size)
                ++split_size;

            for(int i = 0; i < num_threads; ++i){
                std::shared_ptr<std::thread> th(new std::thread(get_faces_normals_cpu, i, split_size, data));
                threads.push_back(th);
            }

            for(int i = 0; i < num_threads; ++i)
                threads[i]->join();
        }

        return SUCCESS;
    }

    int get_valid_pairs(const mesh_data data){
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();
        std::vector<std::shared_ptr<std::thread>> threads;

        // 1. Create result vector for list of valid edges
        std::vector<std::list<std::pair<uint32_t, uint32_t>>> valid_lists;
        std::vector<std::list<uint32_t>> valid_faces_ids_lists;
        valid_faces_ids_lists.resize(num_threads);
        valid_lists.resize(num_threads); 

        // 2. Check threads count
        if (num_threads == 0){
            std::cout << "No threads" << std::endl;    
            return ERR_THREAD_NUMBER;
        }
        else {
            // Calculate split size for threads
            auto norm_size = data.normals->size();
            auto split_size = norm_size / num_threads;

            while (split_size * num_threads < norm_size)
                ++split_size;

            // Create threads
            for(int i = 0; i < num_threads; ++i){
                valid_edges_data result_data = { &valid_lists[i], &valid_faces_ids_lists[i]}; 
                std::shared_ptr<std::thread> th(new std::thread(get_valid_pairs_cpu, i, split_size, data, result_data));
                threads.push_back(th);
            }

            // Join all threads
            for(int i = 0; i < num_threads; ++i)
                threads[i]->join();

            // Collect all lists
            for(int i = 0; i < num_threads; ++i){
                data.valid_edges->insert(data.valid_edges->end(), valid_lists[i].begin(), valid_lists[i].end());
#if _DEBUG
                data.valid_face_ids->insert(data.valid_face_ids->end(), valid_faces_ids_lists[i].begin(), valid_faces_ids_lists[i].end());
#endif
            }
        }

        return SUCCESS;
    }

    int get_valid_pairs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, valid_edges_data result_data){
        // 0. Data Zone
        auto start_block = thread_id * split_size;
        auto& vertexes_ptr = data.vertexes;
        auto& indexes_ptr = data.indexes;
        auto& normals_ptr = data.normals;

        // Result Struct Pointers
        auto valid_faces_ids = result_data.valid_faces_ids;
        auto valid_edges = result_data.valid_edges;

        // Calculate size
        auto temp_block = start_block + split_size;
        auto end_block = temp_block < normals_ptr->size() ? temp_block : normals_ptr->size();

        // 1. Calculations
        for(int i = start_block; i < end_block; ++i) {
            face_data f_data = get_face_data(data, i);
            tr_data t_data = get_triangle_data(data, f_data);

            auto v1_index = f_data.v1_id;
            auto v2_index = f_data.v2_id;
            auto v3_index = f_data.v3_id;

            glm::dvec3 v_ab = t_data.v1 - t_data.v2;
            glm::dvec3 v_ac = t_data.v1 - t_data.v3;
            glm::dvec3 v_bc = t_data.v2 - t_data.v3;

            // Add valid edge
            bool add_valid_face = false;

            auto len_ab = glm::length(v_ab);
            auto len_ac = glm::length(v_ac);
            auto len_bc = glm::length(v_bc);

            if(len_ab < edge_threshold){
                valid_edges->push_back(std::make_pair(v1_index, v2_index));
                add_valid_face = true;
            }
            
            // Add valid edge
            if(len_ac < edge_threshold){
                valid_edges->push_back(std::make_pair(v1_index, v3_index));
                add_valid_face = true;
            }

            // Add valid edge
            if(len_bc < edge_threshold){
                valid_edges->push_back(std::make_pair(v2_index, v3_index));
                add_valid_face = true;
            }

#ifdef _DEBUG
            // Add valid face id
            // TODO: Need to rewrite for multithreading
            if(add_valid_face)
                valid_faces_ids->push_back(i);
#endif
        }

        return SUCCESS;
    }

    int compute_faces_errors(mesh_data data){
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();
        std::vector<std::shared_ptr<std::thread>> threads;

        std::vector<std::list<glm::mat4x4>> errors_lists;
        errors_lists.resize(num_threads);

        // 1. Create threads
        if (num_threads == 0){
            std::cout << "No threads" << std::endl;    
            return ERR_THREAD_NUMBER;
        }
        else {
            // Calculate split size for threads
            auto norm_size = data.normals->size();
            auto split_size = norm_size / num_threads;

            while (split_size * num_threads < norm_size)
                ++split_size;

            // Create threads
            for(int i = 0; i < num_threads; ++i){
                std::shared_ptr<std::thread> th(new std::thread(compute_faces_errors_cpu, i, split_size, data, &errors_lists[i]));
                threads.push_back(th);
            }

            // Join all threads
            for(int i = 0; i < num_threads; ++i)
                threads[i]->join();

            // Collect all lists
            for (int i = 0; i < num_threads; ++i)
                data.face_quadric_errors->insert(data.face_quadric_errors->end(), errors_lists[i].begin(), errors_lists[i].end());
        }

        return SUCCESS;
    }

    int compute_faces_errors_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<glm::mat4x4>* errors){
        // 0. Data Zone
        auto start_block = thread_id * split_size;
        auto& vertexes_ptr = data.vertexes;
        auto& indexes_ptr = data.indexes;
        auto& normals_ptr = data.normals;

        auto faces_cnt = normals_ptr->size();
        auto temp_block = start_block + split_size;
        auto end_block = temp_block < faces_cnt ? temp_block : faces_cnt;

        // 1. Compute faces errors
        for(int i = start_block; i < end_block; ++i){
            face_data face_data = get_face_data(data, i);
            tr_data t_data = get_triangle_data(data, face_data);

            face_args result = {};
            get_face_normal(t_data, result);

            auto a = result.A;
            auto b = result.B;
            auto c = result.C;
            auto d = result.D;

            glm::mat4x4 error_matrix = {
                a * a, a * b, a * c, a * d,
                a * b, b * b, b * c, b * d,
                a * c, c * b, c * c, c * d,
                a * d, b * d, c * d, d * d
            };

            errors->push_back(error_matrix);
        }

        return SUCCESS;
    }

    int compute_costs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, min_vertexes result_data) {
        // 0. Data Zone
        auto min_vert_list = result_data.min_vertex_for_edge;
        auto start_block = thread_id * split_size;
        auto& valid_edges = data.valid_edges;

        // Calculate size
        auto valid_edges_cnt = valid_edges->size();
        auto temp_block = start_block + split_size;
        auto end_block = temp_block < valid_edges_cnt ? temp_block : valid_edges_cnt;

        // 1. Compute costs
        for (int i = start_block; i < end_block; ++i) {
            edge_pair& edge = (*valid_edges)[i];
            std::list<uint32_t> faces_ids_v1;
            std::list<uint32_t> faces_ids_v2;

            // Get Vertexes Surfaces
            get_vertex_surfaces(edge.first, data, faces_ids_v1);
            get_vertex_surfaces(edge.second, data, faces_ids_v2);
            
            glm::mat4x4 v1_Q(0);
            glm::mat4x4 v2_Q(0);

            // Calculate accum Q errors
            for (auto item : faces_ids_v1)
                v1_Q += (*data.face_quadric_errors)[item];

            for (auto item : faces_ids_v2)
                v2_Q += (*data.face_quadric_errors)[item];

            glm::mat4x4 Q = v1_Q + v2_Q;

            // Get Vertexes
            auto v1 = get_vertex_data(data, edge.first);
            auto v2 = get_vertex_data(data, edge.second);
            auto v3 = (v1 + v2) / 2.0f;

            // Create vertexes for calculations
            glm::vec4 v1_res = glm::vec4(v1, 1);
            glm::vec4 v2_res = glm::vec4(v2, 1);
            glm::vec4 v3_res = glm::vec4(v3, 1);

            // Compute costs for every vertex
            auto cost_v1 = get_cost(v1_res, Q);
            auto cost_v2 = get_cost(v2_res, Q);
            auto cost_v3 = get_cost(v3_res, Q);

            if(cost_v1 <= cost_v2 && cost_v1 <= cost_v3)
                min_vert_list->push_back(std::make_pair(std::make_pair(v1, cost_v1), edge));
            else if (cost_v2 <= cost_v1 && cost_v2 <= cost_v3)
                min_vert_list->push_back(std::make_pair(std::make_pair(v2, cost_v2), edge));
            else if (cost_v3 <= cost_v1 && cost_v3 <= cost_v2)
                min_vert_list->push_back(std::make_pair(std::make_pair(v3, cost_v3), edge));
        }

        return SUCCESS;
    }
    
    int compute_costs(mesh_data data) {
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();
        std::vector<std::shared_ptr<std::thread>> threads;

        std::vector<std::list<std::pair<vertex_and_cost, edge_pair>>> thread_results;
        thread_results.resize(num_threads);

        // 1. Create threads
        if (num_threads == 0) {
            std::cout << "No threads" << std::endl;
            return ERR_THREAD_NUMBER;
        } else {
            // Calculate split size for threads
            auto valid_edges_size = data.valid_edges->size();
            uint32_t split_size = valid_edges_size / num_threads;
            
            while (split_size * num_threads < valid_edges_size)
                ++split_size;

            // Create threads
            for (int i = 0; i < num_threads; ++i) {
                min_vertexes result_data = { &thread_results[i] };
                std::shared_ptr<std::thread> th(new std::thread(compute_costs_cpu, i, split_size, data, result_data));
                threads.push_back(th);
            }

            // Join all threads
            for (int i = 0; i < num_threads; ++i)
                threads[i]->join();

            // Collect All Data
            for (int i = 0; i < num_threads; ++i)
                data.edge_vertexes->insert( data.edge_vertexes->end(), thread_results[i].begin(), thread_results[i].end());
        }

        return SUCCESS;
    }


    int optimize_mesh(mesh_data data) {
        std::list<uint32_t> temp_indexes;
        std::list<double> temp_vertexes;

        temp_indexes.insert(temp_indexes.begin(), data.indexes->begin(), data.indexes->end());
        temp_vertexes.insert(temp_vertexes.begin(), data.vertexes->begin(), data.vertexes->end());

        while (data.edge_vertexes->size() > 0) {
            std::pair<vertex_and_cost, edge_pair> replace_vertex = data.edge_vertexes->front();
            data.edge_vertexes->pop_front();


        }

        return SUCCESS;
    }
}