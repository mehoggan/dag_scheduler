////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/DynamicLibraryRegistry.h"

#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace com::dag_scheduler {
std::unordered_set<DynamicLibraryRegistry::RegistryItem,
                   DynamicLibraryRegistry::RegistryItem>
        DynamicLibraryRegistry::registry_;

const DynamicLibraryRegistry::RegistryItem&
DynamicLibraryRegistry::registerDynamicLibrary(const std::string& name) {
    typedef std::unordered_set<RegistryItem> Collection_t;
    RegistryItem item(name);
    Collection_t::iterator find_it = registry_.find(item);
    Collection_t::iterator ret_val = registry_.end();
    if (find_it == registry_.end()) {
        auto find_pair_it = registry_.insert(item);
        ret_val = find_pair_it.first;
    } else {
        ret_val = find_it;
    }
    return (*ret_val);
}

DynamicLibraryRegistry::RegistryItem::RegistryItem() {}

DynamicLibraryRegistry::RegistryItem::RegistryItem(
        const std::string& library_name)
        : library_name_(library_name) {
    try {
        shared_library_ = boost::dll::shared_library(library_name);
    } catch (boost::system::system_error& system_error) {
        std::stringstream error_builder;
        error_builder << "Could not load library from ";
        error_builder << library_name << " with " << system_error.what();
        throw std::runtime_error(error_builder.str() +
                                 " with: " + system_error.what());
    }
}

DynamicLibraryRegistry::RegistryItem::~RegistryItem() {
    shared_library_.unload();
}

const boost::dll::shared_library&
DynamicLibraryRegistry::RegistryItem::getSharedLib() const {
    return shared_library_;
}

const std::string& DynamicLibraryRegistry::RegistryItem::getName() const {
    return library_name_;
}

std::size_t DynamicLibraryRegistry::RegistryItem::operator()(
        const DynamicLibraryRegistry::RegistryItem& item) const {
    return std::hash<std::string>{}(item.library_name_);
}

bool DynamicLibraryRegistry::RegistryItem::operator==(
        const RegistryItem& item) const {
    return library_name_ == item.library_name_;
}
}  // namespace com::dag_scheduler
