#include <boost/dll/shared_library.hpp>

#include <cstddef>
#include <unordered_set>

namespace com
{
  namespace dag_scheduler
  {
    class DynamicLibraryRegistry
    {
    public:
      struct RegistryItem;

    public:
      static const RegistryItem &register_dynamic_library(
        const std::string &name);

    public:
      struct RegistryItem
      {
      public:
        RegistryItem();

        explicit RegistryItem(const std::string &library_name);

        ~RegistryItem();

        const boost::dll::shared_library &get_shared_lib() const;

        const std::string &get_name() const;

        std::size_t operator()(const RegistryItem &item) const;

        bool operator==(const RegistryItem &item) const;

      private:
        std::string library_name_;
        boost::dll::shared_library shared_library_;
      };

    private:
      static std::unordered_set<RegistryItem, RegistryItem> registery_;
    };    
  }
}
