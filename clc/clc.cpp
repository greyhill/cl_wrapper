#include <cl_wrapper/cl_wrapper.hpp>

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

void print_usage(char *progname) { 
  std::cout << "usage: " << progname << " [-l] | [-p ID] PATH"
    << std::endl;
}

void list_platforms() {
  const std::vector<cl::platform> &platforms =
    cl::platform::platforms();
  std::cout << "available OpenCL platforms:\n";
  for(unsigned i=0; i<platforms.size(); ++i) {
    std::cout << i << ": " << platforms[i].name() << "\n";
    std::cout << "\t" << "vendor: " << platforms[i].vendor() << "\n";
    std::cout << "\t" << "version: " << platforms[i].version() << "\n";
    std::cout << "\t" << "profile: " << platforms[i].profile() << "\n";
    std::cout << "\t" << "extensions: " 
      << platforms[i].extensions() << "\n";
  }
}

void build_program(const std::string &path, int platform_id) {
  const std::vector<cl::platform> &platforms =
    cl::platform::platforms();
  cl::platform platform = platforms[platform_id];
  std::cout << "building to platform " << platform_id << ": "
    << platform.name() << "\n";

  std::vector<cl::device> devices = platform.devices();
  std::cout << "building on the following devices:\n";
  for(unsigned i=0; i<devices.size(); ++i) {
    std::cout << i << ": " << devices[i].name() << ", ";
    std::cout << "driver version: " <<
      devices[i].driver_version() << "\n";
  }

  cl::context context(platform, devices.size(), &devices[0]);

  // FIXME add support for windows paths?
  // TODO get file path, captilize it

  std::ifstream infile(path.c_str());
  std::stringstream build_stream;
  // TODO add preliminary stuff to hpp_stream and cpp_stream
  std::stringstream hpp_stream;
  std::stringstream cpp_stream;
  std::string line;
  while(std::getline(infile, line)) {
    build_stream << line << "\n";
  }
  infile.close();
  std::cout << build_stream.str() << std::endl;
}

int main(int argc, char *argv[]) {
  if(argc < 2) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  std::list<std::string> arguments;
  for(int i=1; i<argc; ++i) arguments.push_back(argv[i]);

  if(arguments.front() == "-l") {
    try {
      list_platforms();
      return EXIT_SUCCESS;
    } catch(const std::exception &e) {
      std::cout << "caught exception: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  int platform_id = 0;
  if(arguments.front() == "-p") {
    arguments.pop_front();
    if(arguments.size() < 2) {
      std::cout << "not enough arguments provided after -p switch\n";
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
    std::stringstream ss;
    ss << arguments.front();
    ss >> platform_id;
    arguments.pop_front();
  }
  if(arguments.size() < 1) {
    std::cout << "no PATH provided\n";
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  try {
    build_program(arguments.front(), platform_id);
    return EXIT_SUCCESS;
  } catch(const std::exception &e) {
    std::cout << "caught exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}

