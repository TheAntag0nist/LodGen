#include <gpu_math/gpu_core.h>

namespace lod_generator {
    uint32_t gpu_core::m_programs_counter = 0;
    gpu_core* gpu_core::m_core = nullptr;

    gpu_core::gpu_core() {}
    gpu_core::~gpu_core() {}

    gpu_core& gpu_core::get_instance(){
        if(m_core == nullptr)
            m_core = new gpu_core();

        return *m_core;
    }

    int gpu_core::get_platforms(std::vector<cl::Platform>& platforms, bool display){
        cl::Platform::get(&platforms);
        
        if(platforms.size() == 0){
            std::cout <<"[GPU_CORE]:> No platforms found. Check OpenCL installation!\n";
            return ERR_NO_PLATFORMS;
        }

        if(display)
            for(auto& platform : platforms)
                std::cout << "[GPU_CORE]:> Platform: " 
                          << platform.getInfo<CL_PLATFORM_NAME>()
                          << std::endl;

        return SUCCESS;
    }

    int gpu_core::find_platform(cl::Platform& platform, std::vector<cl::Platform>& platforms){
        std::string enviroment = "";
        
        for (auto &p : platforms) {
            enviroment = p.getInfo<CL_PLATFORM_VERSION>();
            
            if (enviroment.find("OpenCL 2.") != std::string::npos ||
                enviroment.find("OpenCL 3.") != std::string::npos) {
                // Note: an OpenCL 3.x platform may not support all required features!
                platform = p;
            }
        }

        return SUCCESS;
    }

    int gpu_core::get_devices(std::vector<cl::Device>& devices, cl::Platform& platform, bool display){
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if(devices.size() == 0){
            std::cout<<"[GPU_CORE]:> No GPU devices found.\n";
            return ERR_NO_DEVICES;
        }

        if(display)
            for(auto& device : devices)
                std::cout << "[GPU_CORE]:> Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

        return SUCCESS;
    }

    int gpu_core::init() {
        int op_result = SUCCESS;

        // 1. Get Platform
        op_result = get_platforms(m_platforms);
        if(op_result != SUCCESS)
            return op_result;

        op_result = find_platform(m_current_platform, m_platforms);
        if(op_result != SUCCESS)
            return op_result;
        
        std::cout << "[GPU_CORE]:> Using platform: "<< m_current_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

        // 2. Get Device
        op_result = get_devices(m_devices, m_current_platform);
        if(op_result != SUCCESS)
            return op_result;
        
        m_current_device = m_devices[0];
        std::cout << "c Using device: " << m_current_device.getInfo<CL_DEVICE_NAME>() << std::endl;

        // 3. Create context
        m_context = cl::Context(m_current_device);
        m_queue = cl::CommandQueue(m_context, m_current_device);

        return SUCCESS;
    }

    int gpu_core::add_program(std::string program_name){
        m_program_buffer.push_back(std::map<uint32_t, cl::Buffer>());
        m_program_args.push_back(std::map<uint32_t, argument>());
        m_sources.push_back(cl::Program::Sources());

        m_program_name[m_programs_counter] = program_name;
        m_program_id[program_name] = m_programs_counter;
        ++m_programs_counter;
        return SUCCESS;
    }

    uint32_t gpu_core::get_program_id(std::string program_name){
        if(m_program_id.find(program_name) != m_program_id.end())
            return m_program_id[program_name];

        return -1;
    }

    std::string gpu_core::get_program_name(uint32_t program_id){
        if(m_program_name.find(program_id) != m_program_name.end())
            return m_program_name[program_id];

        return "";
    }

    int gpu_core::clear_programs(){
        m_program_buffer.clear();
        m_program_id.clear();
        m_programs.clear();
        m_sources.clear();
        return SUCCESS;
    }

    int gpu_core::load_source(uint32_t programm_id, std::string src){
#ifdef __UNIX__
        m_sources[programm_id].push_back(src);
#elif WIN32
        m_sources[programm_id].push_back({ src.c_str(), src.size()});
#endif
        return SUCCESS;
    }

    int gpu_core::build_program(uint32_t programm_id) {
        auto& program_src = m_sources[programm_id];
        m_programs.push_back(cl::Program(m_context, program_src));
        cl::Program program = m_programs[programm_id];

#ifdef unix
        auto result = program.build(m_current_device);
#elif WIN32
        auto result = program.build({m_current_device});
#endif
        if (result != CL_SUCCESS) {
            std::cout << "[GPU_CORE]:> Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_current_device, &result) << std::endl;
            if(result == CL_BUILD_PROGRAM_FAILURE){
                int status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(m_current_device);
                if (status != CL_BUILD_ERROR)
                    return ERR_CANT_BUILD_PROG;

                std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_current_device);
                std::string name = m_current_device.getInfo<CL_DEVICE_NAME>();
                
                std::cout << "[GPU_CORE]:> Build log for " << name << ":" << std::endl
                          << buildlog << std::endl;
            }
            return ERR_CANT_BUILD_PROG;
        }
        else
            std::cout << "[GPU_CORE]:> Build was SUCCESS" << std::endl;

        return SUCCESS;
    }

    int gpu_core::execute_program(uint32_t programm_id){
        auto name = get_program_name(programm_id);
        auto programm = m_programs[programm_id];

        cl::Kernel kernel_programm = cl::Kernel(programm, name.c_str());
        auto& map_args = m_program_args[programm_id];
        auto& map_bf = m_program_buffer[programm_id];

        // 1. Set Buffers
        for(auto& bf : map_bf)
            kernel_programm.setArg(bf.first, bf.second);    
        // 2. Set Arguments
        for(auto& arg : map_args){
            auto id = arg.first;
            auto size = arg.second.first;
            auto ptr = arg.second.second;
            kernel_programm.setArg(id, size, ptr);
        }

        m_queue.enqueueNDRangeKernel(kernel_programm, cl::NullRange, cl::NDRange(m_global_size), cl::NDRange(m_local_size));
        m_queue.finish();
        return SUCCESS;
    }

    int gpu_core::flush_program_data(uint32_t programm_id){
        m_program_buffer[programm_id].clear();
        auto& args = m_program_args[programm_id];
        for(auto& item : args)
            delete item.second.second;
        args.clear();
        return SUCCESS;
    }

    int gpu_core::flush_queue(){
        m_queue.flush();
        return SUCCESS;
    }

    void gpu_core::set_local_size(uint32_t local_size){
        m_local_size = local_size;
    }

    void gpu_core::set_global_size(uint32_t global_size){
        m_global_size = global_size;
    }
}