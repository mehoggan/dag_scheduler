#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"
#include <dag_scheduler/workflow_service.h>

#include <yaml-cpp/yaml.h>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    (
      "server_yaml",
      boost::program_options::value<std::string>()->required(),
      "Absolute path to server.yml file."
    )
    (
      "help",
      "Help menu."
    );

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(
      argc, argv, desc), vm);
  boost::program_options::notify(vm);

  int ret = EXIT_FAILURE;
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    ret = EXIT_SUCCESS;
  } else {
    try {
      std::cout << "Loading YAML file..." << std::endl;
      const auto service_yaml_file = vm["server_yaml"].as<std::string>();
      YAML::Node yaml_node = YAML::LoadFile(service_yaml_file);
      std::cout << "Loaded YAML file." << std::endl;
      auto ci = yaml_node.as<
        com::dag_scheduler::workflow_service::connection_info>();
      std::cout << "Deserialized YAML file." << std::endl;
      com::dag_scheduler::workflow_service ws(ci);
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      ret = EXIT_FAILURE;
    }
  }
  return ret;
}
