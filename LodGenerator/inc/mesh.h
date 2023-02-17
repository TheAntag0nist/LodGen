#ifndef MESH_H
#define MESH_H
#include <global.h>

namespace lod_generator{
    class mesh{
        private:
            std::vector<uint32_t> m_indexes;
            std::vector<double> m_vertexes;
            std::vector<double> m_normals;

        public:
            mesh();
            ~mesh();

            // ����������: ��� vector � ������ � �������� 
            void set_indexes(std::vector<uint32_t> indexes_in);
            void set_indexes(uint32_t* ind_arr, uint32_t size);
            
            void set_vertexes(std::vector<double> vertexes_in);
            void set_vertexes(double* vert_arr, uint32_t size);
            
            void set_normals(std::vector<double> normals_in);
            void set_normals(double* norm_arr, uint32_t size);

            std::vector<uint32_t> get_vertexes();
            std::vector<double> get_indexes();
            std::vector<double> get_normals();
    };
}

#endif