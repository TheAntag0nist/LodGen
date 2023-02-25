#include <lod_math.h>

namespace lod_generator {
    glm::vec3 get_face_normal(const glm::vec3 v1,const glm::vec3 v2,const glm::vec3 v3){
        glm::vec3 face_normal = {0,0,0};

        glm::vec3 ab = v1 - v2;
        glm::vec3 ac = v1 - v3;

        face_normal = glm::cross(ab, ac);
        return glm::normalize(face_normal);
    }

    int get_faces_normals_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data){
        // 0. Data Section
        uint32_t start_block = thread_id * split_size;
        auto vertexes = data.vertexes;
        auto indexes = data.indexes;
        auto normals = data.normals;

        // 1. Calculate normals
        for(int index_id = start_block; index_id < start_block + split_size; index_id += 3){
            uint32_t ind_arr[] = { 
                (*indexes)[index_id] * 3,
                (*indexes)[index_id + 1] * 3,
                (*indexes)[index_id + 2] * 3
            };

            glm::vec3 v1 = glm::vec3((*vertexes)[ind_arr[0]], (*vertexes)[ind_arr[0] + 1], (*vertexes)[ind_arr[0] + 2]);
            glm::vec3 v2 = glm::vec3((*vertexes)[ind_arr[1]], (*vertexes)[ind_arr[1] + 1], (*vertexes)[ind_arr[1] + 2]);
            glm::vec3 v3 = glm::vec3((*vertexes)[ind_arr[2]], (*vertexes)[ind_arr[2] + 1], (*vertexes)[ind_arr[2] + 2]);

            auto norm = get_face_normal(v1, v2, v3);
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

    int get_valid_pairs(const mesh_data data, std::list<std::pair<uint32_t, uint32_t>>& valid_pairs){
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
                std::shared_ptr<std::thread> th(new std::thread(get_valid_pairs_cpu, i, split_size, data, valid_lists[i]));
                threads.push_back(th);
            }

            // Join all threads
            for(int i = 0; i < num_threads; ++i)
                threads[i]->join();

            // Collect all lists
            for(int i = 0; i < num_threads; ++i)
                valid_pairs.insert(valid_pairs.end(), valid_lists[i].begin(), valid_lists[i].end());
        }

        return SUCCESS;
    }

    int get_valid_pairs_cpu(uint32_t thread_id, uint32_t split_size, mesh_data data, std::list<std::pair<uint32_t, uint32_t>>& valid_pairs){
        // 0. Data Zone
        auto start_block = thread_id * split_size;
        auto vertexes_ptr = data.vertexes;
        auto indexes_ptr = data.indexes;
        auto normals_ptr = data.normals;

        // 1. Calculations
        for(int i = start_block; i < start_block + split_size; i += 3) {
            auto v1_index = (*indexes_ptr)[i];
            auto v2_index = (*indexes_ptr)[i + 1];
            auto v3_index = (*indexes_ptr)[i + 2];

            glm::vec3 v1 = glm::vec3((*vertexes_ptr)[v1_index], (*vertexes_ptr)[v1_index + 1], (*vertexes_ptr)[v1_index + 2]);
            glm::vec3 v2 = glm::vec3((*vertexes_ptr)[v2_index], (*vertexes_ptr)[v2_index + 1], (*vertexes_ptr)[v2_index + 2]);
            glm::vec3 v3 = glm::vec3((*vertexes_ptr)[v3_index], (*vertexes_ptr)[v3_index + 1], (*vertexes_ptr)[v3_index + 2]);

            glm::vec3 v_ab = v1 - v2;
            glm::vec3 v_ac = v1 - v3;
            glm::vec3 v_bc = v2 - v3;

            if(glm::length(v_ab) < edge_threshold)
                valid_pairs.push_back(std::make_pair(v1_index, v2_index));
            if(glm::length(v_ac) < edge_threshold)
                valid_pairs.push_back(std::make_pair(v1_index, v3_index));
            if(glm::length(v_bc) < edge_threshold)
                valid_pairs.push_back(std::make_pair(v2_index, v3_index));
        }

        return SUCCESS;
    }
}