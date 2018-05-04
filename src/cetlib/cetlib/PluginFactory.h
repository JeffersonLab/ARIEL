#ifndef cetlib_PluginFactory_h
#define cetlib_PluginFactory_h
////////////////////////////////////////////////////////////////////////
// PluginFactory.
//
// General facility for managing the loading and creation of plugins.
//
// One can use this class directly; however one is encouraged to use a
// subclass (e.g.cet::BasicPluginFactory) which provides convenient
// facilities for managing plugins that conform to a particular pattern.
//
// One is expected (but not mandated) to call setDiagReleaseVersion() to
// allow to PluginFactory to provide more information about the plugin
// in the event of a failure. In a future enhancement this will likely
// be obtained from the plugin library itself where available.
//
// Note that due to the nature of the C functions which find symbols in
// dynamic libraries, there is no type safety: a found symbol of the
// correct name will be coerced to the desired function type. If that
// type is inccorrect, chaos is likely to ensue.
////////////////////////////////////////////////////////////////////////

#include "cetlib/LibraryManager.h"
#include "cetlib/detail/wrapLibraryManagerException.h"
#include "cetlib_except/exception.h"

#include <memory>
#include <string>

namespace cet {
  class PluginFactory;
}

class cet::PluginFactory {
public:
  explicit PluginFactory(cet::search_path const& search_path,
                         std::string const& suffix = "plugin");

  // Provide a string or function giving the release and version of the
  // plugin (default "Unknown").
  void setDiagReleaseVersion(std::string const& rv);
  void setDiagReleaseVersion(std::function<std::string()> rvf);

  // General function to find and call a named function from the
  // specified plugin library. RESULT_TYPE must be specified; ARGS may
  // be deduced.
  template <typename RESULT_TYPE, typename... ARGS>
  RESULT_TYPE call(std::string const& libspec,
                   std::string const& funcname,
                   ARGS&&... args) const;

  // Nothrow tag (see find(), below).
  static LibraryManager::nothrow_t nothrow;

  // General functions to find and return a pointer to a function of the
  // specified name and type. Use "nothrow" to select the non-throwing
  // version of the function.
  template <typename RESULT_TYPE, typename... ARGS>
  auto find(std::string const& funcname, std::string const& libspec) const
    -> RESULT_TYPE (*)(ARGS...);

  template <typename RESULT_TYPE, typename... ARGS>
  auto find(std::string const& funcname,
            std::string const& libspec,
            LibraryManager::nothrow_t) const -> RESULT_TYPE (*)(ARGS...);

  template <typename FUNCTION_TYPE>
  std::enable_if_t<std::is_function<FUNCTION_TYPE>::value, FUNCTION_TYPE*> find(
    std::string const& funcname,
    std::string const& libspec) const;

  // May define subclasses.
  virtual ~PluginFactory() = default;

private:
  // Not copyable.
  PluginFactory(PluginFactory const&) = delete;
  PluginFactory& operator=(PluginFactory const&) = delete;

  template <typename T>
  void resolveSymbolOrThrow_(std::string const& libspec,
                             std::string const& funcname,
                             T& symbol) const;

  std::string releaseVersion_() const;

  LibraryManager lm_;
  std::string releaseVersionString_{};
  std::function<std::string()> releaseVersionFunc_{};
};

inline void
cet::PluginFactory::setDiagReleaseVersion(std::function<std::string()> rvf)
{
  releaseVersionFunc_ = rvf;
}

inline void
cet::PluginFactory::setDiagReleaseVersion(std::string const& rv)
{
  releaseVersionString_ = rv;
}

template <typename RESULT_TYPE, typename... ARGS>
inline RESULT_TYPE
cet::PluginFactory::call(std::string const& libspec,
                         std::string const& funcname,
                         ARGS&&... args) const
{
  return (*find<RESULT_TYPE, ARGS...>(libspec, funcname))(
    std::forward<ARGS>(args)...);
}

template <typename FUNCTION_TYPE>
inline std::enable_if_t<std::is_function<FUNCTION_TYPE>::value, FUNCTION_TYPE*>
cet::PluginFactory::find(std::string const& libspec,
                         std::string const& funcname) const
{
  FUNCTION_TYPE** symbol{nullptr};
  resolveSymbolOrThrow_(libspec, funcname, symbol);
  return *symbol;
}

template <typename RESULT_TYPE, typename... ARGS>
inline auto
cet::PluginFactory::find(std::string const& libspec,
                         std::string const& funcname) const
  -> RESULT_TYPE (*)(ARGS...)
{
  RESULT_TYPE (*symbol)(ARGS...) = nullptr;
  resolveSymbolOrThrow_(libspec, funcname, symbol);
  return symbol;
}

template <typename RESULT_TYPE, typename... ARGS>
inline auto
cet::PluginFactory::find(std::string const& libspec,
                         std::string const& funcname,
                         LibraryManager::nothrow_t nothrow) const
  -> RESULT_TYPE (*)(ARGS...)
{
  return lm_.getSymbolByLibspec<RESULT_TYPE (*)(ARGS...)>(
    libspec, funcname, nothrow);
}

template <typename T>
void
cet::PluginFactory::resolveSymbolOrThrow_(std::string const& libspec,
                                          std::string const& funcname,
                                          T& symbol) const
{
  try {
    lm_.getSymbolByLibspec(libspec, funcname, symbol);
  }
  catch (exception const& e) {
    detail::wrapLibraryManagerException(
      e, "Plugin", libspec, releaseVersion_());
  }
  if (symbol == nullptr) {
    throw exception("Configuration", "BadPluginLibrary")
      << "Plugin " << libspec << " with version " << releaseVersion_()
      << " has internal symbol definition problems: consult an expert.";
  }
}
#endif /* cetlib_PluginFactory_h */

// Local Variables:
// mode: c++
// End:
