#include <LodGenerator.h>
#include <terminal.h>

std::string command;

int main(){
    lod_generator::mesh src_mesh;
    lod_generator::mesh dst_mesh;
    std::string filename;

    std::cout << INFO << "enter -help for more information"<< std::endl;
    std::cout << INFO << "enter -exit to exit" << std::endl;

    while (true) {
        std::cout << INPUT;
        std::cin >> command;

        command = check_command(command);

        // 1. Exit Command
        if (command == "exit")
            break;
        // 2. Help Command
        if (command == "help"){
            help();
            continue;
        }
        // 3. Read Mesh Command
        if (command == "read") {
            filename = get_filename();
            load_mesh(src_mesh, filename);
            continue;
        }
        // 4. Write Mesh Command
        if (command == "write") {
            filename = get_filename();
            save_mesh(dst_mesh,filename);
            continue;
        }
        // 5. Optimize Mesh Command
        if (command == "optimize") {
            filename = get_filename();
            optimize_mesh(src_mesh, dst_mesh, filename);
            continue;
        }
        // 6. Launch Tests
        if(command == "tests"){
            launch_tests();
            continue;
        }
         
        std::cout << ERROR << "unknown command, try again" << std::endl;
    }

    std::cout << INFO << "exit program" << std::endl;
    return 0;
}