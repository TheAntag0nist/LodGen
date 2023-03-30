#include <gpu_math/gpu_core.h>
#include <gpu_math/gpu_test.h>
#include <gpu_math/kernel_code.h>
#include <algorithms/algorithms.h>

namespace lod_generator {
    void test_gpu_core(){
        std::vector<float> arrays[3] {
            { 5.0f, 2.0f, 4.0f, 8.0f},
            { 1.0f, 2.0f, 4.0f, 8.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f}
        };

        std::string source = 
            "__kernel void add_kernel(__global float* A, __global float* B, __global float* C) {"
	            "const uint n = get_global_id(0);"
	            "C[n] = A[n]+B[n];"
            "}";

        auto& core = gpu_core::get_instance();
        core.init();
        core.add_program("add_kernel");
        auto id = core.get_program_id("add_kernel");
        core.load_source(id, source);
        core.build_program(id);

        core.set_global_size(4);
        core.set_local_size(1);

        core.add_argument(id, 0, arrays[0]);
        core.add_argument(id, 1, arrays[1]);
        core.add_argument(id, 2, arrays[2]);

        core.execute_program(id);

        core.get_output(id, 2, arrays[2]);
        for(auto& item : arrays[2])
            std::cout << "item = " << item << std::endl;
        core.flush_program_data(id);
        core.flush_queue();
    }

    void test_get_valid_pairs(mesh& src_mesh) {
        // 1. CPU Calculations
        mesh_data data = {};
        prepare_data(src_mesh, data);
        data.algorithm_error = 0.5f;

        qem::get_valid_pairs(data);
        // 2. GPU Calculations
        std::vector<edge_pair> valid_pairs;
        auto indexes_size = data.indexes->size();
        valid_pairs.resize(indexes_size / 3);

        auto& core = gpu_core::get_instance();
        core.init();
        core.add_program("get_valid_pairs");
        auto id = core.get_program_id("get_valid_pairs");
        core.load_source(id, GET_VALID_PAIRS);
        core.build_program(id);

        core.set_global_size(indexes_size / 3);
        core.set_local_size(1);

        size_t v_size = data.vertexes->size();
        size_t i_size = data.indexes->size();

        core.add_argument(id, 0, valid_pairs);
        core.add_argument(id, 1, (*data.vertexes));
        core.add_argument(id, 2, v_size);
        core.add_argument(id, 3, (*data.indexes));
        core.add_argument(id, 4, i_size);

        core.execute_program(id);
        core.get_output(id, 0, valid_pairs);

        std::cout << "CPU First pair is " << (*data.valid_edges)[0].first << " and "<< (*data.valid_edges)[0].second << std::endl;
        std::cout << "Vertexes Size: " << v_size << std::endl;
        std::cout << "Indexes Size: " << i_size << std::endl;
        std::cout << "GPU Valid Edges Size: " << valid_pairs.size() << std::endl;
        std::cout << "CPU Valid Edges Size: " << data.valid_edges->size() << std::endl;

        int edge_id = 0;
        for(size_t i = 0; i < valid_pairs.size(); ++i){
            if(valid_pairs[i].first != 0 || 
                valid_pairs[i].second != 0){
                edge_id = i;
                break;
            }
        }
        std::cout << "First GPU Correct Edge: " << valid_pairs[edge_id].first << " and " << valid_pairs[edge_id].second << std::endl;
        core.flush_program_data(id);
        core.flush_queue();
    }
}