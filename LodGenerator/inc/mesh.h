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

            void set_vertexes();
            void set_indesxes();
            void set_normals();
    };
}

#endif