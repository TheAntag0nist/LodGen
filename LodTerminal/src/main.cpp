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

        if (command == "exit")
            break;
        else if (command == "help")
            help();
        else if (command == "read") {
            filename = get_filename();
            load_mesh(src_mesh, filename);
        }
        else if (command == "write") {
            filename = get_filename();
            save_mesh(dst_mesh,filename);
        }
        else if (command == "optimize") {
            filename = get_filename();
            optimize_mesh(src_mesh, dst_mesh, filename);
        }
        else 
            std::cout << ERROR << "unknown command, try again" << std::endl;
    }

    std::cout << INFO << "exit program" << std::endl;
    return 0;
}