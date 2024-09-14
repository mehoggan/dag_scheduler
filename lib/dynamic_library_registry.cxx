#include "dag_scheduler/dynamic_library_registry.h"

#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace com {
  namespace dag_scheduler {
    std::unordered_set<
      DynamicLibraryRegistry::RegistryItem,
      DynamicLibraryRegistry::RegistryItem>
      DynamicLibraryRegistry::registery_;

    const DynamicLibraryRegistry::RegistryItem &
    DynamicLibraryRegistry::register_dynamic_library(const std::string &name
    ) {
      typedef std::unordered_set<RegistryItem> Collection_t;
      RegistryItem item(name);
      Collection_t::iterator find_it = registery_.find(item);
      Collection_t::iterator ret = registery_.end();
      if (find_it == registery_.end()) {
        auto find_pair_it = registery_.insert(item);
        ret = find_pair_it.first;
      } else {
        ret = find_it;
      }
      return (*ret);
    }

    DynamicLibraryRegistry::RegistryItem::RegistryItem() {}

    DynamicLibraryRegistry::RegistryItem::RegistryItem(
      const std::string &library_name
    ) : library_name_(library_name) {
      try {
        shared_library_ = boost::dll::shared_library(library_name);
      } catch (boost::system::system_error &system_error) {
        std::stringstream error_builder;
        error_builder << "Could not load library from ";
        error_builder << library_name << " with " << system_error.what();
        throw std::runtime_error(
          error_builder.str() + " with: " + system_error.what()
        );
      }
    }

    DynamicLibraryRegistry::RegistryItem::~RegistryItem() {
      shared_library_.unload();
    }

    const boost::dll::shared_library &
    DynamicLibraryRegistry::RegistryItem::get_shared_lib() const {
      return shared_library_;
    }

    const std::string &
    DynamicLibraryRegistry::RegistryItem::get_name() const {
      return library_name_;
    }

    std::size_t DynamicLibraryRegistry::RegistryItem::operator()(
      const DynamicLibraryRegistry::RegistryItem &item
    ) const {
      return std::hash<std::string>{}(item.library_name_);
    }

    bool
    DynamicLibraryRegistry::RegistryItem::operator==(const RegistryItem &item
    ) const {
      return library_name_ == item.library_name_;
    }
  } // namespace dag_scheduler
} // namespace com
