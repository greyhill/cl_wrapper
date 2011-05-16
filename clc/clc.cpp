#include <cl_wrapper/cl_wrapper.hpp>

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

void print_usage(char *progname) { 
  std::cout << "usage: " << progname << " [-l] | [-p ID] [-o OPTS] PATH"
    << std::endl;
}

void list_devices(cl::platform p) {
  std::cout << "\t" << "devices:\n";
  const std::vector<cl::device> &devices = p.devices();
  for(unsigned i=0; i<devices.size(); ++i) {
    std::cout << "\t" << i << ": " << devices[i].name() << "\n";
    std::cout << "\t\t" << "address bits: " << devices[i].address_bits()
      << "\n";
    std::cout << "\t\t" << "max alloc size: " <<
      devices[i].max_mem_alloc_size() << "\n";
    std::cout << "\t\t" << "driver version: " <<
      devices[i].driver_version() << "\n";

    std::cout << "\t\t" << "max work size: ";
    std::vector<size_t> max_work_size =
      devices[i].max_work_item_sizes();
    for(unsigned j=0; j<max_work_size.size(); ++j) {
      std::cout << max_work_size[j] << " ";
    }
    std::cout << "\n";
    std::cout << "\t\t" << "max 2d image size: "
      << devices[i].image2d_max_width() << "(w) x " 
      << devices[i].image2d_max_height() << "(h)\n";
    std::cout << "\t\t" << "max 3d image size: "
      << devices[i].image3d_max_width() << "(w) x "
      << devices[i].image3d_max_height() << "(h) x "
      << devices[i].image3d_max_depth() << "(d)\n";
    std::cout << "\t\t" << "global mem size: "
      << devices[i].global_mem_size() << "\n";
    std::cout << "\t\t" << "max read image args: " 
      << devices[i].max_read_image_args() << "\n";
  }
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
    list_devices(platforms[i]);
  }
}

void build_program(const std::string &path, const std::string &opts, 
    int platform_id) {
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
  const size_t last_slash = path.find_last_of('/');
  const std::string &filename = path.substr(last_slash+1);
  const size_t first_dot = filename.find_first_of('.');
  const std::string &base_name = filename.substr(0, first_dot);
  std::string base_name_cap = base_name;

  for(unsigned i=0; i < base_name_cap.size(); ++i)
    base_name_cap[i] = toupper(base_name_cap[i]);

  std::ifstream infile(path.c_str());
  std::stringstream build_stream;
  // TODO add preliminary stuff to hpp_stream and cpp_stream

  std::stringstream hpp_stream;
  hpp_stream << "#ifndef _" << base_name_cap << "_SOURCE_HPP_\n";
  hpp_stream << "#define _" << base_name_cap << "_SOURCE_HPP_\n";
  hpp_stream << "/* this file is automatically produced by clc */\n";
  hpp_stream << "extern const char *" << base_name << "_source;\n";
  hpp_stream << "#endif\n";

  std::stringstream cpp_stream;
  cpp_stream << "#include \"" << filename << ".hpp\"\n";
  cpp_stream << "const char *" << base_name << "_source = ";

  std::string line;
  while(std::getline(infile, line)) {
    build_stream << line << "\n";
    cpp_stream << "\n  \"";
    for(unsigned i=0; i<line.size(); ++i) {
      if(line[i] == '"') {
        cpp_stream << "\\\"";
      } else if(line[i] == '\\') {
        cpp_stream << "\\\\";
      } else {
        cpp_stream << line[i];
      }
    }
    cpp_stream << "\\n\"";
  }
  cpp_stream << ";\n";
  infile.close();

  std::cout << "attempting to compile " << path << "... ";
  cl::program p(context, build_stream.str());
  try {
    p.build(opts);
    std::cout << "success!" << std::endl;

    const std::string &header_path = path + ".hpp";
    std::ofstream header(header_path.c_str());
    header << hpp_stream.str();
    header.close();

    const std::string &cpp_path = path + ".cpp";
    std::ofstream cpp(cpp_path.c_str());
    cpp << cpp_stream.str();
    cpp.close();

    std::cout << "cpp-ready files written to "
      << header_path << " and " << cpp_path << "\n";
  } catch(const cl::cl_error &err) {
    std::cout << "compilation failed!" << std::endl;
  }
  std::cout << "\nbuild log:\n" << p.build_log(platform.devices()[0])
    << "\n";
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
  std::string opts = "";
  while(arguments.size() > 1) {
    if(arguments.front() == "-p") {
      arguments.pop_front();
      if(arguments.size() < 1) {
        std::cout << "-p switch provided with no platform id\n";
        print_usage(argv[0]);
        return EXIT_FAILURE;
      }
      std::stringstream ss;
      ss << arguments.front();
      ss >> platform_id;
      arguments.pop_front();
    } else if(arguments.front() == "-o") {
      arguments.pop_front();
      if(arguments.size() < 1) {
        std::cout << "-o switch provided with no options\n";
        print_usage(argv[0]);
        return EXIT_FAILURE;
      }
      opts = arguments.front();
      arguments.pop_front();
    }
  }
  if(arguments.size() < 1) {
    std::cout << "no PATH provided\n";
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  try {
    build_program(arguments.front(), opts, platform_id);
    return EXIT_SUCCESS;
  } catch(const std::exception &e) {
    std::cout << "caught exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}

