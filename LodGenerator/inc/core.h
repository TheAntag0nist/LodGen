#ifndef CORE_H
#define CORE_H
#include <global.h>

namespace lod_generator{
    class lod_core{
        private:
            static lod_core* m_core;
            lod_core();

        public:
            static lod_core* get_instance();
            

    };
}

#endif