#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H
#include <string>

namespace lod_generator{
    enum COMPUTE_ARCHITECTURE {
        CPU_ARCHITECTURE,
        GPU_ARCHITECTURE
    };
    
    class core_config{
    private:
        COMPUTE_ARCHITECTURE m_architecture = CPU_ARCHITECTURE;
        double m_default_error = 0.5f;

    private:
        COMPUTE_ARCHITECTURE parse_architecture(std::string str);
        double parse_double_by_str(std::string str);
        int parse_int_by_str(std::string str);

    public:
        core_config();
        ~core_config();  

        void read_config(std::string config_filepath);

        COMPUTE_ARCHITECTURE get_architecture();

        void set_default_error(double value);
        double get_default_error();
    };
}


#endif