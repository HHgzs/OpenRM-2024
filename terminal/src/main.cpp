#include "terminal.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unistd.h>


int main(int argc, char** argv) {
    int option;
    bool oscilloscope_flag = false;
    bool monitor_flag = false;
    std::vector<std::string> arg_strs;
    std::vector<std::string> key_name{"autoaim", "camsense", "radar"};
    
    while ((option = getopt(argc, argv, "dhimov")) != -1) {
        switch (option) {
            case 'd':
                rm::term_init();
                rm::dashboard(key_name);
                break;
            case 'h':
                std::cout << "Usage: " << argv[0] << " [-d] [-h] [-i] [-v] [-o <arg> <arg> ... ]" << std::endl;
                break;
            case 'i':
                std::cout << "Hello, World!" << std::endl;
                break;
            case 'm':
                rm::term_init();
                rm::monitor(key_name);
                break;
            case 'v':
                std::cout << "OpenRM version: " << OPENRM_VERSION << std::endl;
                break;
            case 'o':
                oscilloscope_flag = true;
                break;
        }
    }

    if (optind < argc) {
        for (int i = optind; i < argc; i++) {
            arg_strs.push_back(argv[i]);
        }
    }

    if (oscilloscope_flag) {
        rm::term_init();
        rm::oscilloscope(key_name, arg_strs);
    }

    return 0;

}