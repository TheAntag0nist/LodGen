#include <terminal.h>

// Вывод помощи
void help() {
    std::cout << INFO << "enter 'read' to read the.obj file" << std::endl;
    std::cout << INFO << "enter 'write' to write mesh to.obj file" << std::endl;
    std::cout << INFO << "enter 'optimize' to optimize the obj file" << std::endl;
}

// Запись в файл
void save_mesh(lod_generator::mesh& dst_mesh, std::string filename) {
    std::cout << INFO << "writing to file " << filename << std::endl;

    std::vector<uint32_t> indexes = dst_mesh.get_indexes_link();
    std::vector<double> vertexes = dst_mesh.get_vertexes_link();
    std::vector<double> normals = dst_mesh.get_normals_link();

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << ERROR << "failed to open file: " << filename << std::endl;
        return;
    }

    file << std::fixed << std::setprecision(10);

    std::cout << START << "started writing into a file" << std::endl;
    // Записываем вертексы
    for (size_t i = 0; i < vertexes.size(); i += 3) {
        file << "v " << vertexes[i] << " " << vertexes[i + 1] << " " << vertexes[i + 2] << "\n";
    }

    // Записываем нормали
    //for (size_t i = 0; i < normals.size(); i += 3) {
    //    file << "vn " << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << "\n";
    //}

    // Записываем индексы
    for (size_t i = 0; i < indexes.size(); i += 3) {
        file << "f " << indexes[i] + 1 << "//" << " "
            << indexes[i + 1] + 1 << "//" << " "
            << indexes[i + 2] + 1 << "//" << "\n";
    }

    std::cout << END << "ended writing into a file" << std::endl;

}

// Чтение из файла
// !!! НЕВЕРНО ЧИТАЕТ ИНДЕКСЫ !!! 
void load_mesh(lod_generator::mesh& src_mesh, std::string filename) {
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

void optimize_mesh(lod_generator::mesh& src_mesh, lod_generator::mesh& dst_mesh, std::string filename) {
    // TODO: Create Timer class
    auto start_read = std::chrono::high_resolution_clock::now();
    load_mesh(src_mesh, filename);
    auto finish_read = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_read = finish_read - start_read;
    std::cout << "Read Elapsed Time: " << elapsed_read.count() << " ms" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    auto instance = lod_generator::lod_core::get_instance();
    instance->generate_lod(src_mesh, dst_mesh, lod_generator::BASIC_ALG);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    std::cout << "Optimization Elapsed Time: " << elapsed.count() << " ms" << std::endl;

    filename = get_filename();

    auto start_write = std::chrono::high_resolution_clock::now();
    save_mesh(dst_mesh, filename);
    auto finish_write = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_write = finish_write - start_write;
    std::cout << "Write Elapsed Time: " << elapsed_write.count() << " ms" << std::endl;
}


// Получение имени файла
std::string get_filename() {
    // пока только добавляет .obj в конец
    // (??) TODO: подключить библиотеку algorithm и сделать валидацию имени файла...
    // ...(без пробелов и иных символов) но нужна ли она в научке?
    std::cout << INFO << "enter name of file (without '.obj')" << std::endl;
    std::cout << INPUT;
    std::string filename;
    std::cin >> filename;
    filename = filename + ".obj";
    return filename;
}

std::string check_command(std::string command) {
    while (command.find("-") == 0) {
        command.erase(0, 1);
    }
    std::cout << DEBUG << command << std::endl;
    return command;
}
