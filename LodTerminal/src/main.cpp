#include <terminal.h>

// Глобальные переменные и объекты для облегчения передачи данных по функциям
std::string command;


int main(){
    // Закомменчено чтобы не сломать
    // auto core = lod_generator::lod_core::get_instance();

    lod_generator::mesh src_mesh;
    lod_generator::mesh dst_mesh;

    std::string filename;
    // Текст при запуске
    std::cout << INFO << "enter -help for more information"<< std::endl;
    std::cout << INFO << "enter -exit to exit" << std::endl;

    // Цикл работы с пользователем
    // Завершается при вводе команды -exit
    while (true) {
        std::cout << INPUT;
        std::cin >> command;

        command = check_command(command);


        if (command == "exit") {
            break;
        }
        else if (command == "help") {
            help();
        }
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
        else {
            std::cout << ERROR << "unknown command, try again" << std::endl;
        }
    }

    std::cout << INFO << "exit program" << std::endl;
  
    // Закомменчено чтобы не сломать
    // core->generate_lod(src_mesh, dst_mesh, lod_generator::BASIC_ALG);
    return 0;
}