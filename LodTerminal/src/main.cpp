#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

#include <LodGenerator.h>

// ���������� ���������� � ������� ��� ���������� �������� ������ �� ��������
std::string command;
std::string filename;
lod_generator::mesh src_mesh;
lod_generator::mesh dst_mesh;

// ��������� ���������, ��������� ����� �������
// ������ std::cout << INFO << "����������" << std::endl
const std::string INFO =    "[LodTerminal]:[INFO]:> ";      // ����� ����������� ����������
const std::string ERROR =   "[LodTerminal]:[ERROR]:> ";     // ������������ ������
const std::string START =   "[LodTerminal]:[START]:> ";     // ������ ���������� ��������
const std::string END =     "[LodTerminal]:[END]:> ";       // ����� ���������� ��������
const std::string DEBUG =   "[LodTerminal]:[DEBUG]> ";      // ����� ���������
const std::string INPUT =   "[LodTerminal]:> ";             // ����������� � �����

// ����� ������
void help() {
    std::cout << INFO << "enter -read to read the.obj file" << std::endl;
    std::cout << INFO << "enter -write to write mesh to .obj file" << std::endl;
}

// ������ � ����
void memToObj() {
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
    // ���������� ��������
    for (size_t i = 0; i < vertexes.size(); i += 3) {
        file << "v " << vertexes[i] << " " << vertexes[i + 1] << " " << vertexes[i + 2] << "\n";
    }

    // ���������� �������
    //for (size_t i = 0; i < normals.size(); i += 3) {
    //    file << "vn " << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << "\n";
    //}

    // ���������� �������
    for (size_t i = 0; i < indexes.size(); i += 3) {
        file << "f " << indexes[i] + 1 << "//" << " "
            << indexes[i + 1] + 1 << "//" << " "
            << indexes[i + 2] + 1 << "//" << "\n";
    }

    std::cout << END << "ended writing into a file" << std::endl;

}

// ������ �� �����
// !!! ������� ������ ������� !!! 
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

// ��������� ����� �����
void getFilename() {
    // ���� ������ ��������� .obj � �����
    // (??) TODO: ���������� ���������� algorithm � ������� ��������� ����� �����...
    // ...(��� �������� � ���� ��������) �� ����� �� ��� � ������?
    std::cout << INFO << "enter name of file (without '.obj')" << std::endl;
    std::cout << INPUT;
    std::cin >> filename;
    filename = filename + ".obj";
}

int main(){
    // ������������ ����� �� �������
    // auto core = lod_generator::lod_core::get_instance();

    // ����� ��� �������
    std::cout << INFO << "enter -help for more information"<< std::endl;
    std::cout << INFO << "enter -exit to exit" << std::endl;

    // ���� ������ � �������������
    // ����������� ��� ����� ������� -exit
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
        else if (command == "optimize" || command == "-optimize") {
            getFilename();

            // TODO: Create Timer class
            auto start_read = std::chrono::high_resolution_clock::now();
            objToMem();
            auto finish_read = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed_read = finish_read - start_read;
            std::cout << "Read Elapsed Time: " << elapsed_read.count() << " ms" << std::endl;

            auto start = std::chrono::high_resolution_clock::now();

            auto instance = lod_generator::lod_core::get_instance();
            instance->generate_lod(src_mesh, dst_mesh, lod_generator::HYBRID_QEM);

            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = finish - start;
            std::cout << "Optimization Elapsed Time: " << elapsed.count() << " ms" << std::endl;

            getFilename();

            auto start_write = std::chrono::high_resolution_clock::now();
            memToObj();
            auto finish_write = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed_write = finish_write - start_write;
            std::cout << "Write Elapsed Time: " << elapsed_write.count() << " ms" << std::endl;
        }
        else {
            std::cout << ERROR << "unknown command, try again" << std::endl;
        }
    }

    std::cout << INFO << "exit program" << std::endl;
    return 0;
}