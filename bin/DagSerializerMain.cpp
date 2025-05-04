////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <dag_scheduler/DagSerialization.h>
#include <dag_scheduler/Logging.h>
#include <yaml-cpp/yaml.h>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    std::string help_string =
            (std::string("\t--dag_yaml should point to a ") +
             std::string("file with contents similar to:\n") +
             com::dag_scheduler::YAMLDAGDeserializer::fullSampleOutput());

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()(
            "dag_yaml",
            boost::program_options::value<std::string>()->required(),
            "Absolute path to dag.yml (dag.yaml) file.")("help",
                                                         help_string.c_str());

    boost::program_options::variables_map variable_map;
    boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc),
            variable_map);

    auto log_tag = com::dag_scheduler::LogTag("root");
    com::dag_scheduler::Logging::addStdCoutLogger(log_tag);

    int ret_value = EXIT_FAILURE;
    bool help = false;
    if (variable_map.count("help")) {
        com::dag_scheduler::Logging::info(log_tag, desc);
        ret_value = EXIT_SUCCESS;
        help = true;
    } else {
        try {
            com::dag_scheduler::Logging::info(log_tag, "Loading YAML file...");
            const auto service_yaml_file =
                    variable_map["dag_yaml"].as<std::string>();
            YAML::Node yaml_node = YAML::LoadFile(service_yaml_file);
            com::dag_scheduler::Logging::info(log_tag, "Loaded YAML file.");
            std::unique_ptr<com::dag_scheduler::DAG> dag_ptr =
                    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
            com::dag_scheduler::Logging::info(log_tag,
                                              "Deserialized YAML file.");
            com::dag_scheduler::Logging::info(log_tag, (*dag_ptr));
        } catch (const std::exception& e) {
            com::dag_scheduler::Logging::error(log_tag, "Error:", e.what());
            ret_value = EXIT_FAILURE;
        }
    }
    if (!help) {
        boost::program_options::notify(variable_map);
    }
    return ret_value;
}
