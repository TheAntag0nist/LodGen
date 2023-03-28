#ifndef GPU_CORE_H
#define GPU_CORE_H
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <global.h>
#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#else
    #include <CL/opencl.hpp>
#endif

namespace lod_generator {
    typedef std::pair<uint32_t, uint32_t> program_ranges;

    class gpu_core {
    private:
        std::vector<cl::Platform> m_platforms;
        cl::Platform m_current_platform;

        std::vector<cl::Device> m_devices;
        cl::Device m_current_device; 
        cl::Context m_context;

        std::map<uint32_t, std::string> m_program_name;
        std::map<std::string, uint32_t> m_program_id;
        std::vector<cl::Program::Sources> m_sources;
        
        std::vector<std::list<cl::Buffer>> m_program_buffer;
        std::vector<cl::Program> m_programs;
        cl::CommandQueue m_queue;

        static uint32_t m_programs_counter;
        static gpu_core* m_core;
        gpu_core();

    private:
        int get_platforms(std::vector<cl::Platform>& platforms, bool display = true);
        int find_platform(cl::Platform& platform, std::vector<cl::Platform>& platforms);
        int get_devices(std::vector<cl::Device>& devices, cl::Platform& platform, bool display = true);

    public:
        ~gpu_core();
        static gpu_core& get_instance();
        
        int init();
        int clear_programs();
        int add_program(std::string program_name);
        uint32_t get_program_id(std::string program_name);
        std::string get_program_name(uint32_t program_id);
        int load_source(uint32_t programm_id, std::string src);

        int build_program(uint32_t programm_id);
        int execute_program(uint32_t programm_id);

        template <class T>
        int add_argument(uint32_t programm_id, uint32_t arg_id, std::vector<T>& data){
            auto& list_buffers = m_program_buffer[programm_id];
            auto size = sizeof(T) * data.size();
            auto ptr = data.data();

            list_buffers.push_back(cl::Buffer(m_context, CL_MEM_READ_WRITE, size));
            auto& buffer = list_buffers.back();
            m_queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, size, ptr);
            return SUCCESS;
        }
    };
}

#endif