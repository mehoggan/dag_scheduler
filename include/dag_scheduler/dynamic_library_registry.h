#include <boost/dll/shared_library.hpp>

#include <cstddef>
#include <unordered_set>

namespace com
{
  namespace dag_scheduler
  {
    /**
     * @brief A class that keeps dynamic libraries loaded in memory for use
     *        by a \ref DAG and its associated \ref TaskStage (s).
     *
     * A class that is used to keep dynamic libraries loaded in memory for the
     * duration of a \ref DAG 's life cycle. When stages are loaded from a
     * dynamic library, the library that creates the \ref TaskStage must be
     * persisted in memory for the lifetime of a \ref DAG in order for that
     * \ref DAG to execute without accessing bad memory.
     *
     * Developers Notes:
     * Added at the time of writing \ref YAMLDagDeserializer. It was noticed
     * that if a \ref boost::dll::shared_library was loaded on the stack, a
     * \ref TaskStage was created dynamically from it. That the \ref TaskStage
     * would be unloaded or inaccessible at the time the
     * \ref boost::dll::shared_library went out of scope within \ref
     * YAMLDagDeserializer.
     */
    class DynamicLibraryRegistry
    {
    public:
      struct RegistryItem;

    public:
      /**
       * @brief A static method for registering a boost::dll::shared_library
       * into a static block of memory to ensure that it persists.
       *
       * A static method for registering a boost::dll::shared_library into
       * a static block of memory to ensure that it persists. This is needed
       * because the boost::dll::shared_library needs to be loaded into memory
       * for the duration of the stage. At the time of writing this class it
       * was not determined if the \ref TaskStage or the \ref DAG should be
       * responsible for the life cycle of the boost::dll::shared_library.
       *
       * @param[in] name The underlying call to boost::dll::shared_library
       *                 requires boost::filesystem::path. For testing this
       *                 was always a path to the dynamic library in the build
       *                 system. However, this should work for a libray on the
       *                 underlying operating system's library path.
       *
       * @return A \ref RegisteryItem which associates the name of the library
       *         for future lookup within the \ref DynamicLibraryRegistry.
       */
      static const RegistryItem &register_dynamic_library(
        const std::string &name);

    public:
      /**
       * @brief A struct that associates a path or a library name to an
       * underlying boost::dll::shared_library.
       *
       * TODO (mehoggan): Add in the ability to deregister a \ref RegistryItem.
       */
      struct RegistryItem
      {
      public:
        /**
         * @brief ctor.
         */
        RegistryItem();

        /**
         * @brief ctor.
         *
         * Loads a shared library into memory based on a path or a \ref
         * library_name.
         *
         * @param[in] library_name The name or path to a library. If it is
         *                         by name that library must be on the
         *                         underlying operating systems library path.
         */
        explicit RegistryItem(const std::string &library_name);

        /**
         * @brief dtor.
         */
        ~RegistryItem();

        /**
         * @brief Getter for the \ref boost::dll:shared_library created at
         * time of insertion using \ref register_dynamic_library.
         *
         * @return A const reference to a \ref boost::dll::shared_library.
         */
        const boost::dll::shared_library &get_shared_lib() const;

        /**
         * @brief Getter for the \ref library_name passed in to the \ref ctor.
         */
        const std::string &get_name() const;

        /**
         * @brief Operator overload used by \ref DynamicLibraryRegistry
         * storage container that tracks all loaded shared libraries.
         */
        std::size_t operator()(const RegistryItem &item) const;

        /**
         * @brief Operator overload used by \ref DynamicLibraryRegistry
         * storage container that tracks all loaded shared libraries.
         */
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
