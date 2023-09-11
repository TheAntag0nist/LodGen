#include <math/lod_math.h>

namespace lod_generator {
///////////////////////////////////////////////////////////////////////////
    int get_vertex_surfaces(uint32_t vertex_id, mesh_data data, std::list<uint32_t>& faces_ids) {
        auto& indexes_ptr = data.indexes;

        for (size_t i = 0; i < indexes_ptr->size(); i += 3) {
            if (vertex_id == (*indexes_ptr)[i] ||
                vertex_id == (*indexes_ptr)[i + 1] ||
                vertex_id == (*indexes_ptr)[i + 2])
                faces_ids.push_back(i / 3);
        }

        return SUCCESS;
    }

    glm::vec3 get_vertex_data(mesh_data data, uint32_t vertex_id) {
        auto real_vertex_id = vertex_id * 3;
        auto& vertexes_ptr = data.vertexes;

        auto result = glm::dvec3(
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
        auto& normals = data.normals;

        auto temp_block = start_block + split_size;
        size_t end_block = temp_block < normals->size() ? temp_block : normals->size();

        // 1. Calculate normals
        for(size_t index_id = start_block; index_id < end_block; ++index_id){
            face_data f_data = get_face_data(data, index_id);
            tr_data t_data = get_triangle_data(data, f_data);

            face_args result = {};
            get_face_normal(t_data, result);
            glm::dvec3 norm = glm::normalize(glm::dvec3(result.A, result.B, result.C));
            (*normals)[index_id] = norm;
        }

        return SUCCESS;
    }

    int get_faces_normals(mesh_data data){
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();
        
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
                std::thread th(get_faces_normals_cpu, i, split_size, data);
                th.join();
            }
        }

        return SUCCESS;
    }

    glm::dvec3 get_triangle_center(tr_data& data) {
        glm::dvec3 temp = (data.v1 + data.v2 + data.v3);
        return glm::dvec3(temp.x / 3.0f, temp.y / 3.0f, temp.z / 3.0f);
    }

    std::set<uint32_t> get_nearest_vertexes(mesh_data data, uint32_t vertex_id) {
        auto& indexes = data.indexes;
        std::set<uint32_t> result;

        for (int i = 0; i < indexes->size(); i += 3) {
            auto ind_1 = (*indexes)[i];
            auto ind_2 = (*indexes)[i + 1];
            auto ind_3 = (*indexes)[i + 2];

            if (ind_1 == vertex_id) {
                result.insert(ind_2);
                result.insert(ind_3);
            }
            else if (ind_2 == vertex_id) {
                result.insert(ind_1);
                result.insert(ind_3);
            }
            else if (ind_3 == vertex_id) {
                result.insert(ind_1);
                result.insert(ind_2);
            }
        }

        return result;
    }

    double vertex_weight(mesh_data data, uint32_t v_id) {
        auto nearest_vertexes = get_nearest_vertexes(data, v_id);
        double accumulated_distance = 0.0f;
        double angle_distance = 0.0f;

        auto a = get_vertex_data(data, v_id);
        for (auto id : nearest_vertexes) {
            auto b = get_vertex_data(data, id);
            angle_distance += glm::length(a - b) * (glm::dot(a, b) / (glm::length(a) * glm::length(b)));
            accumulated_distance += glm::length(a - b);
        }

        return angle_distance / accumulated_distance;
    }

    int get_vertex_weights(mesh_data data) {
        auto& vertexes_w = data.vertexes_weights;
        int vertexes_cnt = data.vertexes->size() / 3;
        auto& vertexes = data.vertexes;

        if(!vertexes_w->empty())
            vertexes_w->clear();
        vertexes_w->resize(vertexes_cnt);

        for (uint32_t i = 0; i < vertexes_cnt; ++i)
            (*vertexes_w)[i] = v_and_w{  i, vertex_weight(data, i)};

        return SUCCESS;
    }
///////////////////////////////////////////////////////////////////////////
namespace qem {
    int get_valid_pairs(const mesh_data data){
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();

        // 1. Create result vector for list of valid edges
        std::vector<std::list<edge_pair>> valid_lists;
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
            auto size = data.indexes->size() / 3;
            auto split_size = size / num_threads;

            while (split_size * num_threads < size)
                ++split_size;

            // Create threads
            for(int i = 0; i < num_threads; ++i){
                valid_edges_data result_data = { &valid_lists[i], &valid_faces_ids_lists[i]}; 
                std::thread th(get_valid_pairs_cpu, i, split_size, data, result_data);
                th.join();
            }

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
        auto& indexes_ptr = data.indexes;

        // Result Struct Pointers
        auto valid_edges = result_data.valid_edges;

        // Calculate size
        auto faces_cnt = indexes_ptr->size() / 3;
        auto temp_block = start_block + split_size;
        size_t end_block = temp_block < faces_cnt ? temp_block : faces_cnt;

        // 1. Calculations
        for(size_t i = start_block; i < end_block; ++i) {
            face_data f_data = get_face_data(data, i);
            tr_data t_data = get_triangle_data(data, f_data);

            auto v1_index = f_data.v1_id;
            auto v2_index = f_data.v2_id;
            auto v3_index = f_data.v3_id;

            glm::dvec3 v_ab = t_data.v1 - t_data.v2;
            glm::dvec3 v_ac = t_data.v1 - t_data.v3;
            glm::dvec3 v_bc = t_data.v2 - t_data.v3;

            auto len_ab = glm::length(v_ab);
            auto len_ac = glm::length(v_ac);
            auto len_bc = glm::length(v_bc);

            if(len_ab < data.algorithm_error){
                valid_edges->push_back(edge_pair{ v1_index, v2_index });
                continue;
            }
            
            // Add valid edge
            if(len_ac < data.algorithm_error){
                valid_edges->push_back(edge_pair{ v1_index, v3_index });
                continue;
            }

            // Add valid edge
            if(len_bc < data.algorithm_error){
                valid_edges->push_back(edge_pair{v2_index, v3_index});
                continue;;
            }
        }

        return SUCCESS;
    }

    int compute_faces_errors(mesh_data data){
        // 0. Data zone section
        int num_threads = std::thread::hardware_concurrency();

        std::vector<std::list<glm::mat4x4>> errors_lists;
        errors_lists.resize(num_threads);

        // 1. Create threads
        if (num_threads == 0){
            std::cout << "No threads" << std::endl;    
            return ERR_THREAD_NUMBER;
        }
        else {
            // Calculate split size for threads
            auto size = data.indexes->size() / 3;
            auto split_size = size / num_threads;

            while (split_size * num_threads < size)
                ++split_size;

            // Create threads
            for(int i = 0; i < num_threads; ++i){
                std::thread th(compute_faces_errors_cpu, i, split_size, data, &errors_lists[i]);
                th.join();
            }

            // Collect all lists
            for (int i = 0; i < num_threads; ++i)
                data.face_quadric_errors->insert(data.face_quadric_errors->end(), errors_lists[i].begin(), errors_lists[i].end());
        }

        return SUCCESS;
    }

    int compute_faces_errors_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<glm::mat4x4>* errors){
        // 0. Data Zone
        auto start_block = thread_id * split_size;
        auto& indexes_ptr = data.indexes;

        auto faces_cnt = indexes_ptr->size() / 3;
        auto temp_block = start_block + split_size;
        size_t end_block = temp_block < faces_cnt ? temp_block : faces_cnt;

        // 1. Compute faces errors
        for(size_t i = start_block; i < end_block; ++i){
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
        size_t end_block = temp_block < valid_edges_cnt ? temp_block : valid_edges_cnt;

        // 1. Compute costs
        for (size_t i = start_block; i < end_block; ++i) {
            edge_pair& edge = (*valid_edges)[i];
            std::list<uint32_t> faces_ids_v1;
            std::list<uint32_t> faces_ids_v2;

            // Get Vertexes Surfaces
            get_vertex_surfaces(edge.v1, data, faces_ids_v1);
            get_vertex_surfaces(edge.v2, data, faces_ids_v2);
            
            glm::mat4x4 v1_Q(0);
            glm::mat4x4 v2_Q(0);

            // Calculate accum Q errors
            for (auto item : faces_ids_v1)
                v1_Q += (*data.face_quadric_errors)[item];

            for (auto item : faces_ids_v2)
                v2_Q += (*data.face_quadric_errors)[item];

            glm::mat4x4 Q = v1_Q + v2_Q;

            // Get Vertexes
            auto v1 = get_vertex_data(data, edge.v1);
            auto v2 = get_vertex_data(data, edge.v2);
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
                min_vert_list->push_back(std::make_pair(vertex_and_cost{ v1, cost_v1 }, edge));
            else if (cost_v2 <= cost_v1 && cost_v2 <= cost_v3)
                min_vert_list->push_back(std::make_pair(vertex_and_cost{ v2, cost_v2 }, edge));
            else if (cost_v3 <= cost_v1 && cost_v3 <= cost_v2)
                min_vert_list->push_back(std::make_pair(vertex_and_cost{ v3, cost_v3 }, edge));
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


    lod_result optimize_mesh_iterative(mesh_data data) {
        // 0. Data Zone
        lod_result result = SUCCESS;
        uint32_t iter = 0;
        
        // 1. Call Basic Cycle
        result = qem_cycle(data);
        if (result != SUCCESS)
            return result;

        if (data.edge_vertexes->size() == 0)
            return result;

        // 2. While can optimize -> try to optimize
        while (data.edge_vertexes->size() > 0 && iter < data.max_iterations) {
            // 3. Update vector with vertexes and indexes
            update_mesh(data);

            // 4. Clear old metadata
            data.face_quadric_errors->clear();
#ifdef _DEBUG
            data.valid_face_ids->clear();
#endif
            data.edge_vertexes->clear();
            data.valid_edges->clear();

            // 5. Call Basic Cycle
            result = qem_cycle(data);
            if (result != SUCCESS)
                break;

            if (data.edge_vertexes->size() == 0)
                break;

            ++iter;
        }

        return result;
    }

    lod_result optimize_mesh_hybrid(mesh_data data) {
        // 0. Data Zone
        lod_result result = SUCCESS;
        bool change_flag = true;
        uint32_t iter = 0;

        while (change_flag && iter < data.max_iterations) {
            std::set<uint32_t> used_indexes;
            change_flag = false;
            // 1. Call Basic Cycle
            result = qem_cycle(data);
            if (result != SUCCESS)
                return result;

            // If no need optimize then stop executing algorithm
            if (data.edge_vertexes->size() == 0)
                break;

            // 2. While can optimize -> try to optimize
            while (data.edge_vertexes->size() > 0) {
                // 3. Find Min Elem
                auto min_elem = std::min_element(data.edge_vertexes->begin(), data.edge_vertexes->end(),
                    [](const std::pair<vertex_and_cost, edge_pair>& a, const std::pair<vertex_and_cost, edge_pair>& b) 
                    { return a.first.cost < b.first.cost; });
                
                auto& edge = (*min_elem).second;
                uint32_t min_index = edge.v1 < edge.v2 ? edge.v1 : edge.v2;
                uint32_t max_index = edge.v1 > edge.v2 ? edge.v1 : edge.v2;

                // If not process that node or linked node then optimize mesh
                if (used_indexes.find(min_index) == used_indexes.end() &&
                    used_indexes.find(max_index) == used_indexes.end()) {
                    // 4. Update vector with vertexes and indexes
                    update_mesh(data);
                    change_flag = true;
                    ++iter;

                    // 5. Update edges
                    for (auto& item : (*data.edge_vertexes)) {
                        if (item.second.v1 > max_index)
                            --item.second.v1;
                        if (item.second.v2 > max_index)
                            --item.second.v2;
                    }

                    // TODO: Maybe rewrite using <algorithm>
                    std::list temp_list(used_indexes.begin(), used_indexes.end());
                    for (auto& item : temp_list)
                        if (item > max_index)
                            --item;
                    used_indexes.clear();
                    used_indexes.insert(temp_list.begin(), temp_list.end());

                    used_indexes.insert(max_index);
                    used_indexes.insert(min_index);
                }
                else
                    data.edge_vertexes->erase(min_elem);
            }

            // 6. Clear old metadata
            data.face_quadric_errors->clear();
#ifdef _DEBUG
            data.valid_face_ids->clear();
#endif
            data.edge_vertexes->clear();
            data.valid_edges->clear();
        }

        return result;
    }

    lod_result optimize_mesh(mesh_data data) {
        // 0. Data Zone
        std::set<uint32_t> used_indexes;
        lod_result result = SUCCESS;
        uint32_t iter = 0;

        // 1. Call Basic Cycle
        result = qem_cycle(data);
        if (result != SUCCESS)
            return result;

        if (data.edge_vertexes->size() == 0)
            return result;

        // 2. Sort by costs
        data.edge_vertexes->sort(
            [](const std::pair<vertex_and_cost, edge_pair>& a, const std::pair<vertex_and_cost, edge_pair>& b) 
            { return a.first.cost < b.first.cost; });

        // 3. While can optimize -> try to optimize
        while (data.edge_vertexes->size() > 0 && iter < data.max_iterations) {
            auto& edge = data.edge_vertexes->front().second;
            uint32_t min_index = edge.v1 < edge.v2 ? edge.v1 : edge.v2;
            uint32_t max_index = edge.v1 > edge.v2 ? edge.v1 : edge.v2;

            if (used_indexes.find(min_index) == used_indexes.end() &&
                used_indexes.find(max_index) == used_indexes.end()) {
                // 4. Update vector with vertexes and indexes
                update_mesh(data);

                // 5. Update edges
                for (auto& item : (*data.edge_vertexes)) {
                    if(item.second.v1 > max_index)
                        --item.second.v1;
                    if(item.second.v2 > max_index)
                        --item.second.v2;
                }

                // TODO: Maybe rewrite using <algorithm>
                std::list temp_list(used_indexes.begin(), used_indexes.end());
                for (auto& item : temp_list)
                    if(item > max_index)
                        --item;
                used_indexes.clear();
                used_indexes.insert(temp_list.begin(), temp_list.end());
                
                used_indexes.insert(max_index);
                used_indexes.insert(min_index);
            } else
                data.edge_vertexes->pop_front();
            ++iter;
        }

        return result;
    }

    lod_result qem_cycle(mesh_data data) {
        lod_result result = SUCCESS;
        // TODO: Think about thread guard
        // TODO: Need to configure for calculations on CPU or GPU
        // 1. Get valid edges
        result = get_valid_pairs(data);
        if (result != SUCCESS)
            return result;

        // 2. Compute Quadric Errors Matrixes 
        result = compute_faces_errors(data);
        if (result != SUCCESS)
            return result;

        // 3. Compute vertexes for replace with min costs
        result = compute_costs(data);
        if (result != SUCCESS)
            return result;

        return result;
    }

    uint32_t update_mesh(mesh_data data) {
        // 1. Get vertex with minimal cost
        auto iter = std::min_element(data.edge_vertexes->begin(), data.edge_vertexes->end(),
                [](const std::pair<vertex_and_cost, edge_pair>& a, const std::pair<vertex_and_cost, edge_pair>& b) 
                { return a.first.cost < b.first.cost; });

        std::pair<vertex_and_cost, edge_pair> replace_vertex = *iter;
        data.edge_vertexes->erase(iter);
        uint32_t deleted_faces = 0;

        // 2. Get Data
        auto vertex = replace_vertex.first.vertex;
        auto edge = replace_vertex.second;

        uint32_t min_index = edge.v1 < edge.v2 ? edge.v1 : edge.v2;
        uint32_t max_index = edge.v1 > edge.v2 ? edge.v1 : edge.v2;

        // 3. Update vertexes
        // 3.1. Update v1 (set new value)
        (*data.vertexes)[min_index * 3] = vertex.x;
        (*data.vertexes)[min_index * 3 + 1] = vertex.y;
        (*data.vertexes)[min_index * 3 + 2] = vertex.z;

        // 3.2. Delete v2 (remove from vector)
        data.vertexes->erase(data.vertexes->begin() + max_index * 3);
        data.vertexes->erase(data.vertexes->begin() + max_index * 3);
        data.vertexes->erase(data.vertexes->begin() + max_index * 3);

        // 4. Update indexes
        // 4.1. Replace max index on min index
        std::for_each(data.indexes->begin(), data.indexes->end(),
            [max_index, min_index](uint32_t& value) { if (value == max_index) value = min_index; });
        // 4.2. Update indexes greater that max index
        std::for_each(data.indexes->begin(), data.indexes->end(),
            [max_index](uint32_t& value) { if (value > max_index) --value; });
        // 4.3. Remove degenerate faces
        auto& shr_ptr = data.indexes;
        for (size_t i = 0; i < data.indexes->size(); i += 3) {
            int cnt = 0;
            if ((*shr_ptr)[i] == min_index)
                ++cnt;

            if ((*shr_ptr)[i + 1] == min_index)
                ++cnt;

            if ((*shr_ptr)[i + 2] == min_index)
                ++cnt;

            if (cnt >= 2) {
                (*shr_ptr)[i] = UINT_MAX;
                (*shr_ptr)[i + 1] = UINT_MAX;
                (*shr_ptr)[i + 2] = UINT_MAX;
                ++deleted_faces;
            }
        }

        std::list<uint32_t> temp_list;
        for (size_t i = 0; i < data.indexes->size(); ++i) {
            if ((*shr_ptr)[i] != UINT_MAX)
                temp_list.push_back((*shr_ptr)[i]);
        }

        // 5. Rewrite indexes
        data.indexes->clear();
        data.indexes->insert(data.indexes->end(), temp_list.begin(), temp_list.end());
        temp_list.clear();

        return deleted_faces;
    }
}
///////////////////////////////////////////////////////////////////////////
namespace vertex_cluster {
    uint32_t update_mesh(mesh_data data){
        // 1. Search vertex clusters
        search_vertex_clusters(data);

        // 2. Optimize Geometry By Deleting Clusters 
        for (auto& cluster_item : *data.clusters) {
            uint32_t first_index = *cluster_item.vertexes_ind.begin();
            

        }

        // 3. Clear previous stage
        data.used_vertexes->clear();
        data.clusters->clear();

        return SUCCESS;
    }

    int get_centroids(mesh_data data) {
        auto& indexes = data.indexes;
        std::vector<uint32_t> temp_indexes(*indexes);
        
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(temp_indexes.begin(), temp_indexes.end(), g);
        std::vector<uint32_t> random_ids(temp_indexes.begin(), temp_indexes.begin() + data.max_clusters_cnt);

        for (uint32_t center_id : random_ids) {
            glm::dvec3 center = get_vertex_data(data, center_id);
            std::list<uint32_t> cluster_vertex_ind = { center_id };
            data.clusters->push_back(cluster{ cluster_vertex_ind, center });
        }
        
        return SUCCESS;
    }

    int search_vertex_clusters(mesh_data data) {
        auto clusters_cnt = data.max_clusters_cnt;
        auto& clusters = data.clusters;
        auto& indexes = data.indexes;
        bool k_means_flag = true;
        
        // 1. Get k centers
        get_centroids(data);

        // 2. k-means algorithm
        while (k_means_flag) {
            k_means_flag = false;

            // Update cluster
            for (cluster& item : (*data.clusters)) {
                if (item.vertexes_ind.size() < data.max_k_means) {
                    // Create max distance
                    constexpr auto inf = std::numeric_limits<double>::infinity();
                    double distance = inf;
                    int v_id = -1;

                    // Find nearest vertexes
                    for (size_t i = 0; i < data.vertexes->size() / 3.0f; ++i) {
                        auto vertex = (glm::dvec3) get_vertex_data(data, i);
                        auto temp_dist = glm::length(item.center - vertex);
                        bool already_used = std::find_if(item.vertexes_ind.begin(), item.vertexes_ind.end(), 
                            [i](const uint32_t item) { return item == i; }) != item.vertexes_ind.end();

                        // By lowest distance
                        if (temp_dist < distance && !already_used) {
                            distance = temp_dist;
                            v_id = i;
                        }
                    }

                    // Add vertex to cluster
                    if (v_id != -1 && item.vertexes_ind.size() < data.max_k_means) {
                        glm::dvec3 new_center = glm::dvec3(0, 0, 0);
                        item.vertexes_ind.push_back(v_id);
                        for (auto& id : item.vertexes_ind)
                            new_center += get_vertex_data(data, id);

                        item.center.x = new_center.x / item.vertexes_ind.size();
                        item.center.y = new_center.y / item.vertexes_ind.size();
                        item.center.z = new_center.z / item.vertexes_ind.size();
                        k_means_flag = true;
                    }
                }
            }
        }

        return SUCCESS;
    }
    
    lod_result optimize_mesh(mesh_data data){
        // 0. Data Section
        uint32_t iter = 0;

        // 1. Optimize geometry
        while (iter < data.max_iterations) {
            update_mesh(data);
            ++iter;
        }

        return SUCCESS;
    }
}
///////////////////////////////////////////////////////////////////////////
}