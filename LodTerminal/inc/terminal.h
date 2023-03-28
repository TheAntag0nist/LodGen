#ifndef TERMINAL_H
#define TERMINAL_H
#include <global.h>

const std::string INFO  = "[LodTerminal]:[INFO]:> ";
const std::string ERROR = "[LodTerminal]:[ERROR]:> ";
const std::string START = "[LodTerminal]:[START]:> ";
const std::string END   = "[LodTerminal]:[END]:> ";
const std::string DEBUG = "[LodTerminal]:[DEBUG]> ";
const std::string INPUT = "[LodTerminal]:> ";

void help();
void save_mesh(lod_generator::mesh& dst_mesh, std::string filename);
bool load_mesh(lod_generator::mesh& src_mesh, std::string filename);
void optimize_mesh(lod_generator::mesh& src_mesh, lod_generator::mesh& dst_mesh, std::string filename);

lod_generator::LOD_ALG get_type();
std::string check_command(std::string command);
std::string get_filename();
void launch_tests();

#endif 

