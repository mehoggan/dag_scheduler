////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <dag_scheduler/WorkflowService.h>
#include <yaml-cpp/yaml.h>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()(
            "server_yaml",
            boost::program_options::value<std::string>()->required(),
            "Absolute path to server.yml file.")("help", "Help menu.");

    boost::program_options::variables_map variable_map;
    boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc),
            variable_map);
    boost::program_options::notify(variable_map);

    int ret_value = EXIT_FAILURE;
    if (variable_map.count("help")) {
        std::cout << desc << std::endl;
        ret_value = EXIT_SUCCESS;
    } else {
        try {
            std::cout << "Loading YAML file..." << std::endl;
            const auto service_yaml_file =
                    variable_map["server_yaml"].as<std::string>();
            YAML::Node yaml_node = YAML::LoadFile(service_yaml_file);
            std::cout << "Loaded YAML file." << std::endl;
            auto connection_info = yaml_node.as<
                    com::dag_scheduler::WorkflowService::ConnectionInfo>();
            std::cout << "Deserialized YAML file." << std::endl;
            com::dag_scheduler::WorkflowService workflow_service(
                    connection_info);
        } catch (const std::exception& exception) {
            std::cerr << "Error: " << exception.what() << std::endl;
            ret_value = EXIT_FAILURE;
        }
    }
    return ret_value;
}
