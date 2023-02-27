#include <lod_math.h>

namespace lod_generator {
    tr_data get_triangle_data(mesh_data data, face_data f_data){
        auto& vertexes = *(data.vertexes);
        tr_data result = {};

        auto ind_1 = f_data.v1_id * 3;
        auto ind_2 = f_data.v2_id * 3;
        auto ind_3 = f_data.v3_id * 3; 

        result.v1 = glm::vec3(vertexes[ind_1], vertexes[ind_1 + 1], vertexes[ind_1 + 2]);
        result.v2 = glm::vec3(vertexes[ind_2], vertexes[ind_2 + 1], vertexes[ind_2 + 2]);
        result.v3 = glm::vec3(vertexes[ind_3], vertexes[ind_3 + 1], vertexes[ind_3 + 2]);

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
        glm::vec3 face_normal = {0,0,0};

        glm::vec3 tmp_1 = data.v2 - data.v1;
        glm::vec3 tmp_2 = data.v3 - data.v1;

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
        auto end_block = temp_block > indexes->size() ? temp_block : indexes->size();

        // 1. Calculate normals
        for(int index_id = start_block; index_id < end_block; index_id += 3){
            face_data f_data = get_face_data(data, index_id);
            tr_data t_data = get_triangle_data(data, f_data);

            face_args result = {};
            get_face_normal(t_data, result);
            glm::vec3 norm = glm::normalize(glm::vec3(result.A, result.B, result.C));
            (*normals)[index_id / 3] = norm;
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
            uint32_t split_size = data.normals->size() / num_threads;
            if(split_size % 3 == 1)
                split_size = split_size + 1;
            else if(split_size % 3 == 2)
                split_size = split_size + 2;

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
        valid_lists.resize(num_threads); 

        // 2. Check threads count
        if (num_threads == 0){
            std::cout << "No threads" << std::endl;    
            return ERR_THREAD_NUMBER;
        }
        else {
            // Calculate split size for threads
            uint32_t split_size = data.indexes->size() / num_threads;
            if(split_size % 3 == 1)
                split_size = split_size + 1;
            else if(split_size % 3 == 2)
                split_size = split_size + 2;

            // Create threads
            for(int i = 0; i < num_threads; ++i){
                std::shared_ptr<std::thread> th(new std::thread(get_valid_pairs_cpu, i, split_size, data, &valid_lists[i]));
                threads.push_back(th);
            }

            // Join all threads
            for(int i = 0; i < num_threads; ++i)
                threads[i]->join();

            // Collect all lists
            for(int i = 0; i < num_threads; ++i)
                data.valid_pairs->insert(data.valid_pairs->end(), valid_lists[i].begin(), valid_lists[i].end());
        }

        return SUCCESS;
    }

    int get_valid_pairs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<std::pair<uint32_t, uint32_t>>* valid_pairs){
        // 0. Data Zone
        auto start_block = thread_id * split_size;
        auto& vertexes_ptr = data.vertexes;
        auto& indexes_ptr = data.indexes;
        auto& normals_ptr = data.normals;

        auto temp_block = start_block + split_size;
        auto end_block = temp_block > indexes_ptr->size() ? temp_block : indexes_ptr->size();

        // 1. Calculations
        for(int i = start_block; i < end_block; i += 3) {
            face_data f_data = get_face_data(data, i);
            tr_data t_data = get_triangle_data(data, f_data);

            auto v1_index = f_data.v1_id;
            auto v2_index = f_data.v2_id;
            auto v3_index = f_data.v3_id;

            glm::vec3 v_ab = t_data.v1 - t_data.v2;
            glm::vec3 v_ac = t_data.v1 - t_data.v3;
            glm::vec3 v_bc = t_data.v2 - t_data.v3;

            // Add valid edge
            bool add_valid_face = false;
            if(glm::length(v_ab) < edge_threshold){
                valid_pairs->push_back(std::make_pair(v1_index, v2_index));
                add_valid_face = true;
            }
            
            // Add valid edge
            if(glm::length(v_ac) < edge_threshold){
                valid_pairs->push_back(std::make_pair(v1_index, v3_index));
                add_valid_face = true;
            }

            // Add valid edge
            if(glm::length(v_bc) < edge_threshold){
                valid_pairs->push_back(std::make_pair(v2_index, v3_index));
                add_valid_face = true;
            }

            // Add valid face id
            if(add_valid_face)
                data.valid_face_ids->push_back(i / 3);
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
            uint32_t split_size = data.indexes->size() / num_threads;
            if(split_size % 3 == 1)
                split_size = split_size + 1;
            else if(split_size % 3 == 2)
                split_size = split_size + 2;

            // Create threads
            for(int i = 0; i < num_threads; ++i){
                std::shared_ptr<std::thread> th(new std::thread(compute_faces_errors_cpu, i, split_size, data, &errors_lists[i]));
                threads.push_back(th);
            }

            // Join all threads
            for(int i = 0; i < num_threads; ++i)
                threads[i]->join();

            // Collect all lists
            for(int i = 0; i < num_threads; ++i)
                data.face_quadric_errors->insert(data.face_quadric_errors->end(), errors_lists[i].begin(), errors_lists[i].end());
        
        }

        return SUCCESS;
    }

    int compute_faces_errors_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<glm::mat4x4>* errors){
        // 0. Data Zone
        auto start_block = thread_id * split_size;
        auto& valid_faces = data.valid_face_ids;
        auto& vertexes_ptr = data.vertexes;
        auto& indexes_ptr = data.indexes;

        auto temp_block = start_block + split_size;
        auto end_block = temp_block < valid_faces->size() ? temp_block : valid_faces->size();

        // 1. Compute faces errors
        for(int i = start_block; i < end_block; ++i){
            uint32_t face_id = (*valid_faces)[i];
            face_data face_data = get_face_data(data, face_id);
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
}