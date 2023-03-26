#ifndef GPU_CORE_H
#define GPU_CORE_H
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <global.h>
#include <CL/opencl.hpp>

namespace lod_generator {
    class gpu_core {
    private:
        std::vector<cl::Platform> m_platforms;
        cl::Platform m_current_platform;

        std::vector<cl::Device> m_devices;
        cl::Device m_current_device; 
        cl::Context m_context;

        cl::Program::Sources m_sources;
        cl::Program m_program;

        cl::CommandQueue m_queue;
        cl::Buffer m_buffer;

        static gpu_core* m_core;
        gpu_core();

    private:
        int get_platforms(std::vector<cl::Platform>& platforms, bool display = true);
        int find_platform(cl::Platform& platform, std::vector<cl::Platform>& platforms);

        int get_devices(std::vector<cl::Device>& devices, cl::Platform& platform, bool display = true);

    public:
        ~gpu_core();
        gpu_core& get_instance();
        
        int init();
        int clear_source();
        int load_source(std::string src);

        int build_program();
        int set_program_data();
        int execute_program();
    };
}

#endif