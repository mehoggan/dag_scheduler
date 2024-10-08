#include <dag_scheduler/dag_serialization.h>

#include <yaml-cpp/yaml.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  std::string help_string =
    (std::string("\t--dag_yaml should point to a ") +
     std::string("file with contents similar to:\n") +
     com::dag_scheduler::YAMLDagDeserializer::full_sample_output());

  boost::program_options::options_description desc("Allowed options");
  desc
    .add_options()("dag_yaml", boost::program_options::value<std::string>()->required(), "Absolute path to dag.yml (dag.yaml) file.")(
      "help", help_string.c_str()
    );

  boost::program_options::variables_map vm;
  boost::program_options::store(
    boost::program_options::parse_command_line(argc, argv, desc), vm
  );

  int ret = EXIT_FAILURE;
  bool help = false;
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    ret = EXIT_SUCCESS;
    help = true;
  } else {
    try {
      std::cout << "Loading YAML file..." << std::endl;
      const auto service_yaml_file = vm["dag_yaml"].as<std::string>();
      YAML::Node yaml_node = YAML::LoadFile(service_yaml_file);
      std::cout << "Loaded YAML file." << std::endl;
      auto ci = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      std::cout << "Deserialized YAML file." << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      ret = EXIT_FAILURE;
    }
  }
  if (not help) {
    boost::program_options::notify(vm);
  }
  return ret;
}
