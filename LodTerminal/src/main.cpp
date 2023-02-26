#include <iostream>
#include <core.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

// Глобальные переменные и объекты для облегчения передачи данных по функциям
std::string command;
std::string filename;
lod_generator::mesh src_mesh;
lod_generator::mesh dst_mesh;

// Текстовые константы, выводимые перед текстом
// пример std::cout << INFO << "Информация" << std::endl
const std::string INFO =    "[LodTerminal]:[INFO]:> ";      // Общая нейтральная информация
const std::string ERROR =   "[LodTerminal]:[ERROR]:> ";     // Обработанная ошибка
const std::string START =   "[LodTerminal]:[START]:> ";     // Начало выполнение процесса
const std::string END =     "[LodTerminal]:[END]:> ";       // Конец выполнения процесса
const std::string DEBUG =   "[LodTerminal]:[DEBUG]> ";      // Дебаг инфрмация
const std::string INPUT =   "[LodTerminal]:> ";             // Приглашение к вводу

// Вывод помощи
void help() {
    std::cout << INFO << "enter -read to read the.obj file" << std::endl;
    std::cout << INFO << "enter -write to write mesh to .obj file" << std::endl;
}

// Запись в файл
void memToObj() {
    std::cout << INFO << "writing to file " << filename << std::endl;

    std::vector<uint32_t> indexes = src_mesh.get_indexes();
    std::vector<double> vertexes = src_mesh.get_vertexes();
    std::vector<double> normals = src_mesh.get_normals();

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << ERROR << "failed to open file: " << filename << std::endl;
        return;
    }

    file << std::fixed << std::setprecision(6);

    std::cout << START << "started writing into a file" << std::endl;
    // Записываем вертексы
    for (size_t i = 0; i < vertexes.size(); i += 3) {
        file << "v " << vertexes[i] << " " << vertexes[i + 1] << " " << vertexes[i + 2] << "\n";
    }

    // Записываем нормали
    for (size_t i = 0; i < normals.size(); i += 3) {
        file << "vn " << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << "\n";
    }

    // Записываем индексы
    for (size_t i = 0; i < indexes.size(); i += 3) {
        file << "f " << indexes[i] + 1 << "/" << indexes[i] + 1 << " "
            << indexes[i + 1] + 1 << "/" << indexes[i + 1] + 1 << " "
            << indexes[i + 2] + 1 << "/" << indexes[i + 2] + 1 << "\n";
    }

    std::cout << END << "ended writing into a file" << std::endl;

}

// Чтение из файла
// !!! НЕВЕРНО ЧИТАЕТ ИНДЕКСЫ !!! 
void objToMem() {
    std::cout << INFO << "reading from file " << filename << std::endl;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << ERROR << "failed to open file: " << filename << std::endl;
        return;
    }

    std::cout << START << "started reading file" << std::endl;
    std::string line;
    std::vector<uint32_t> indexes;
    std::vector<double> vertexes;
    std::vector<double> normals;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {
            double x, y, z;
            iss >> x >> y >> z;
            vertexes.push_back(x);
            vertexes.push_back(y);
            vertexes.push_back(z);
        }
        else if (token == "vn") {
            double x, y, z;
            iss >> x >> y >> z;
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        }
        else if (token == "f") {
            std::string v1, v2, v3;
            uint32_t i1, i2, i3;
            iss >> v1 >> v2 >> v3;

            std::istringstream v1ss(v1);
            std::getline(v1ss, token, '/');
            i1 = std::stoi(token) - 1;

            std::istringstream v2ss(v2);
            std::getline(v2ss, token, '/');
            i2 = std::stoi(token) - 1;

            std::istringstream v3ss(v3);
            std::getline(v3ss, token, '/');
            i3 = std::stoi(token) - 1;

            indexes.push_back(i1);
            indexes.push_back(i2);
            indexes.push_back(i3);
        }
    }

    src_mesh.set_indexes(indexes);
    src_mesh.set_normals(normals);
    src_mesh.set_vertexes(vertexes);
    
    std::cout << END << "ended reading file" << std::endl;
}

// Получение имени файла
void getFilename() {
    // пока только добавляет .obj в конец
    // (??) TODO: подключить библиотеку algorithm и сделать валидацию имени файла...
    // ...(без пробелов и иных символов) но нужна ли она в научке?
    std::cout << INFO << "enter name of file (without '.obj')" << std::endl;
    std::cout << INPUT;
    std::cin >> filename;
    filename = filename + ".obj";
}

int main(){
    // Закомменчено чтобы не сломать
    // auto core = lod_generator::lod_core::get_instance();

    // Текст при запуске
    std::cout << INFO << "enter -help for more information"<< std::endl;
    std::cout << INFO << "enter -exit to exit" << std::endl;

    // Цикл работы с пользователем
    // Завершается при вводе команды -exit
    while (true) {
        std::cout << INPUT;
        std::cin >> command;

        if (command == "exit" || command == "-exit") {
            break;
        }
        else if (command == "help" || command == "-help") {
            help();
        }
        else if (command == "read" || command == "-read") {
            getFilename();
            objToMem();
        }
        else if (command == "write" || command == "-write") {
            getFilename();
            memToObj();
        }
        else {
            std::cout << ERROR << "unknown command, try again" << std::endl;
        }
    }

    std::cout << INFO << "exit program" << std::endl;
  
    // Закомменчено чтобы не сломать
    // core->generate_lod(src_mesh, dst_mesh, lod_generator::BASIC_ALG);
    return 0;
}