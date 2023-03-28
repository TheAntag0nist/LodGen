#include <gpu_math/gpu_core.h>
#include <gpu_math/gpu_test.h>

namespace lod_generator {
    void test_gpu_core(){
        std::vector<float> arrays[3] {
            { 1.0f, 2.0f, 4.0f, 8.0f},
            { 1.0f, 2.0f, 4.0f, 8.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f}
        };
        std::string source = 
            "__kernel void add_kernel(__global float* A, __global float* B, __global float* C) {"
	            "const uint n = get_global_id(0);"
	            "C[n] = A[n]+B[n];"
            "}";

        std::cout << "&Arr[0] = " << arrays[0].data() << std::endl;
        std::cout << "&Arr[1] = " << arrays[1].data() << std::endl;
        std::cout << "&Arr[2] = " << arrays[2].data() << std::endl;

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
    }
}