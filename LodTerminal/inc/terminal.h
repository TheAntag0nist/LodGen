#ifndef TERMINAL_H
#define TERMINAL_H
#include <iostream>
#include <core.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

// Текстовые константы, выводимые перед текстом
// пример std::cout << INFO << "Информация" << std::endl
const std::string INFO = "[LodTerminal]:[INFO]:> ";      // Общая нейтральная информация
const std::string ERROR = "[LodTerminal]:[ERROR]:> ";     // Обработанная ошибка
const std::string START = "[LodTerminal]:[START]:> ";     // Начало выполнение процесса
const std::string END = "[LodTerminal]:[END]:> ";       // Конец выполнения процесса
const std::string DEBUG = "[LodTerminal]:[DEBUG]> ";      // Дебаг инфрмация
const std::string INPUT = "[LodTerminal]:> ";             // Приглашение к вводу

void help();
void save_mesh(lod_generator::mesh& dst_mesh, std::string filename);
void load_mesh(lod_generator::mesh& src_mesh, std::string filename);
void optimize_mesh(lod_generator::mesh& src_mesh, lod_generator::mesh& dst_mesh, std::string filename);
std::string get_filename();
std::string check_command(std::string command);

#endif 

