#include <iostream>
#include <core.h>

int main(){
    std::cout << "[LodTerminal]:> enter -help for more information" << std::endl;
    auto core = lod_generator::lod_core::get_instance();
    lod_generator::mesh src_mesh;
    lod_generator::mesh dst_mesh;
    
    core->generate_lod(src_mesh, dst_mesh, lod_generator::BASIC_ALG);
    return 0;
}