#include <core_config.h>
#include <json/json.hpp>

using json = nlohmann::json;

namespace lod_generator {
    core_config::core_config() {}
    core_config::~core_config() {}

    COMPUTE_ARCHITECTURE core_config::parse_architecture(std::string str){
        COMPUTE_ARCHITECTURE result = CPU_ARCHITECTURE;

        if(str == "CPU")
            result = CPU_ARCHITECTURE;
        if(str == "GPU")
            result = GPU_ARCHITECTURE;

        return result;
    }

    double core_config::parse_double_by_str(std::string str){
        return std::stof(str);
    }

    int core_config::parse_int_by_str(std::string str){
        return std::stoi(str);
    }

    void core_config::read_config(std::string config_filepath){
        // 1. Read file
        FILE* fl = fopen(config_filepath.c_str(), "rb");
        std::string file_content = "";
        // TODO: Add logging
        if(!fl)
            return;

        // 2. Get the file size
        fseek(fl, 0, SEEK_END);
        size_t file_size = ftell(fl);
        rewind(fl);

        // 3. Allocate memory for file content
        char* content = new char[file_size];
        size_t bytes_read = fread(content, 1, file_size, fl);

        // TODO: Add logging
        if(bytes_read != file_size)
            return;
        file_content = content;

        delete[] content;
        fclose(fl);

        // 4. Parse configuration
        json js = json::parse(file_content);
        
        // 5. Read fields from configuration
        m_architecture = parse_architecture(js["compute_architecture"]);
        m_default_error = parse_double_by_str(js["default_error"]);
    }

    COMPUTE_ARCHITECTURE core_config::get_architecture(){
        return m_architecture;
    }

    void core_config::set_default_error(double value){
        m_default_error = value;
    }

    double core_config::get_default_error(){
        return m_default_error;
    }
}