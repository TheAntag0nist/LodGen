#include <terminal.h>

void help() {
    std::cout << INFO << "enter 'read' to read the.obj file" << std::endl;
    std::cout << INFO << "enter 'write' to write mesh to.obj file" << std::endl;
    std::cout << INFO << "enter 'optimize' to optimize the obj file" << std::endl;
}

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
    for (size_t i = 0; i < vertexes.size(); i += 3) {
        file << "v " << vertexes[i] << " " << vertexes[i + 1] << " " << vertexes[i + 2] << "\n";
    }

    for (size_t i = 0; i < indexes.size(); i += 3) {
        file << "f " << indexes[i] + 1 << "//" << " "
            << indexes[i + 1] + 1 << "//" << " "
            << indexes[i + 2] + 1 << "//" << "\n";
    }

    std::cout << END << "ended writing into a file" << std::endl;

}

bool load_mesh(lod_generator::mesh& src_mesh, std::string filename) {
    std::cout << INFO << "reading from file " << filename << std::endl;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << ERROR << "failed to open file: " << filename << std::endl;
        return false;
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
            std::string indexes_str = "";
            int triangulation = 0;
            
            while(iss >> indexes_str){
                uint32_t index = 0;
            
                std::istringstream vss(indexes_str);
                std::getline(vss, token, '/');
                index = std::stoi(token) - 1;

                if(triangulation > 2){
                    indexes.push_back(indexes[indexes.size() - 1]);
                    indexes.push_back(indexes[indexes.size() - 4]);
                }

                indexes.push_back(index);
                ++triangulation;
            }
        }
    }

    src_mesh.set_indexes(indexes);
    src_mesh.set_normals(normals);
    src_mesh.set_vertexes(vertexes);

    std::cout << END << "ended reading file" << std::endl;
    return true;
}

void optimize_mesh(lod_generator::mesh& src_mesh, lod_generator::mesh& dst_mesh, std::string filename) {
    debug_timer local_timer;
    double err_value = 0.0f;
    
    // 1. Load Mesh
    local_timer.start();
    if(!load_mesh(src_mesh, filename))
        return;
    local_timer.stop();
    // 1.1. Load Mesh Elapsed Time
    std::cout << "Read Elapsed Time: " << local_timer.to_string() << " ms" << std::endl;

    // Choose algorithm type
    auto type = get_type();
    std::cout << "Algorithm Error: ";
    std::cin >> err_value;
    if(err_value < 0.000001)
        err_value = 0.000001;

    // 2. LOD Generating
    local_timer.start();
    auto instance = lod_generator::lod_core::get_instance();
    instance->set_error(err_value);
    instance->generate_lod(src_mesh, dst_mesh, type);
    local_timer.stop();
    // 2.1. Generating Elapsed Time
    std::cout << "Optimization Elapsed Time: " << local_timer.to_string() << " ms" << std::endl;

    // 3. Get result filename for save
    filename = get_filename();

    // 4. Save Mesh
    local_timer.start();
    save_mesh(dst_mesh, filename);
    local_timer.stop();
    // 4.1. Save Mesh Elapsed time
    std::cout << "Write Elapsed Time: " << local_timer.to_string() << " ms" << std::endl;
}

std::string get_filename() {
    std::cout << INFO << "enter name of file (without '.obj')" << std::endl;
    std::cout << INPUT;
    std::string filename;
    std::cin >> filename;
    filename = filename + ".obj";
    return filename;
}

lod_generator::LOD_ALG get_type(){
    lod_generator::LOD_ALG type = lod_generator::HYBRID_QEM;
    std::string type_str = "";
    
    std::cout << "Types: " << std::endl
              << "\t1) BASIC_QEM" << std::endl
              << "\t2) HYBRID_QEM" << std::endl
              << "\t3) ITERATIVE_QEM" << std::endl
              << "\t4) VERTEX_CLUSTER" << std::endl;
    std::cout << "Default type is HYBRID_QEM" << std::endl;
    std::cout << "Enter type: " << type_str;
    std::cin >> type_str;
    if(type_str == "BASIC_QEM" || type_str == "1")
        type = lod_generator::BASIC_QEM;
    if(type_str == "HYBRID_QEM" || type_str == "2")
        type = lod_generator::HYBRID_QEM;
    if(type_str == "ITERATIVE_QEM" || type_str == "3")
        type = lod_generator::ITERATIVE_QEM;
    if(type_str == "VERTEX_CLUSTER" || type_str == "4")
        type = lod_generator::VERTEX_CLUSTER;

    return type;
}

std::string check_command(std::string command) {
    while (command.find("-") == 0) {
        command.erase(0, 1);
    }
    std::cout << DEBUG << command << std::endl;
    return command;
}
