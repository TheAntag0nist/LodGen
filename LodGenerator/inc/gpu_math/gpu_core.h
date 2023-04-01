#ifndef GPU_CORE_H
#define GPU_CORE_H
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <global.h>
#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#elif _WIN32
    #include <CL/cl.hpp>
#elif __linux__
    #include <CL/opencl.hpp>
#endif

namespace lod_generator {
    typedef std::pair<uint32_t, void*> argument;

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
        
        std::vector<std::map<uint32_t, cl::Buffer>> m_program_buffer;
        std::vector<std::map<uint32_t, argument>> m_program_args;
        std::vector<cl::Program> m_programs;
        cl::CommandQueue m_queue;
        uint32_t m_global_size;
        uint32_t m_local_size;

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
        int flush_program_data(uint32_t programm_id);
        int flush_queue();

        void set_local_size(uint32_t local_size);
        void set_global_size(uint32_t global_size);

        template <class T>
        int get_output(uint32_t programm_id, uint32_t buff_id, std::vector<T>& output){
            auto& map_buffers = m_program_buffer[programm_id];
            auto& bf = map_buffers[buff_id];
            
            m_queue.enqueueReadBuffer(bf, CL_TRUE, 0,
                output.size() * sizeof(T), output.data());
            return SUCCESS;
        }

        template <class T>
        int add_argument(uint32_t programm_id, uint32_t arg_id, std::vector<T>& data){
            auto& map_buffers = m_program_buffer[programm_id];
            auto size = sizeof(T) * data.size();
            auto ptr = data.data();

            map_buffers[arg_id] = cl::Buffer(m_context, CL_MEM_READ_WRITE, size);
            auto& buffer = map_buffers[arg_id];
            m_queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, size, ptr);
            return SUCCESS;
        }

        template <class T>
        int add_argument(uint32_t programm_id, uint32_t arg_id, T& data){
            auto& map_args = m_program_args[programm_id];
            auto size = sizeof(T);
            auto ptr = new T;
            *ptr = data;

            map_args[arg_id] = argument( size, ptr);
            return SUCCESS;
        }
    };
}

#endif