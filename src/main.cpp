#include "config.h"
#include "utility.h"
#include <cstring>

constexpr auto config_file {"config/udpknocker.conf"};

int main(int argc, char* argv[]){
  if (argc < 2){
    std::cerr << "Missing parameter(s)" << std::endl;
    return 1;
  }

  if(std::strcmp(argv[1], "validate") == 0){
    if (argc != 3) {
      std::cerr << "Missing config file path or to many parameters provided" << std::endl;
      return 1;
    }
    try{
      Config cfg(argv[2]); // argv[2] is config file path
    } 
    catch (const std::invalid_argument &e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
  } else {
    std::cerr << "Unknown parameter" << std::endl;
    return 1;
  }
  return 0;
}