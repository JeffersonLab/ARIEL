#ifndef cetlib_LibraryManager_h
#define cetlib_LibraryManager_h

#include "cetlib/hard_cast.h"
#include "cetlib/search_path.h"
#include "cetlib/shlib_utils.h"

#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace cet {
  class LibraryManager;
}

class cet::LibraryManager {
public:
  // Create a LibraryManager that searches through a search path
  // specified by search_path for dynamically loadable libraries
  // having the given lib_type. Library names are expected to be of
  // the form:
  //      libaa_bb_cc_xyz_<lib_type>.<ext>
  // and where <ext> is provided automatically as appropriate for the
  // platform.
  explicit LibraryManager(cet::search_path search_path, std::string lib_type);

  explicit LibraryManager(cet::search_path search_path,
                          std::string lib_type,
                          std::string pattern);

  // Use platform-dependent dynamic loader search path
  explicit LibraryManager(std::string lib_type);
  explicit LibraryManager(std::string lib_type, std::string pattern);

  // The d'tor does NOT unload libraries, because that is dangerous to
  // do in C++. Use the compiler-generated destructor.
  // ~LibraryManager();

  // Find and return a symbol named 'sym_name' in the library
  // identified by 'libspec'. The library is dynamically loaded if
  // necessary. If more than one library matching libspec is found, an
  // exception is thrown. If the correct library cannot be found or
  // loaded, an exception is thrown. If the symbol specified cannot be
  // loaded, an exception is thrown unless the final argument to the
  // function call is LibraryManager::nothrow, in which case the
  // desired function pointer will be NULL.

  template <typename T>
  T getSymbolByLibspec(std::string const& libspec,
                       std::string const& sym_name) const;
  template <typename T>
  void getSymbolByLibspec(std::string const& libspec,
                          std::string const& sym_name,
                          T& sym) const;

  template <typename T>
  T getSymbolByPath(std::string const& lib_loc,
                    std::string const& sym_name) const;

  template <typename T>
  void getSymbolByPath(std::string const& lib_loc,
                       std::string const& sym_name,
                       T& sym) const;

  // Versions which won't throw if they can't find the symbol.
  static struct nothrow_t {
  } nothrow;

  template <typename T>
  T getSymbolByLibspec(std::string const& libspec,
                       std::string const& sym_name,
                       nothrow_t) const;
  template <typename T>
  void getSymbolByLibspec(std::string const& libspec,
                          std::string const& sym_name,
                          T& sym,
                          nothrow_t) const;
  template <typename T>
  T getSymbolByPath(std::string const& lib_loc,
                    std::string const& sym_name,
                    nothrow_t) const;
  template <typename T>
  void getSymbolByPath(std::string const& lib_loc,
                       std::string const& sym_name,
                       T& sym,
                       nothrow_t) const;

  // Get a list of loadable libraries (full paths). Returns the number
  // of entries.
  size_t getLoadableLibraries(std::vector<std::string>& list) const;
  template <class OutIter>
  size_t getLoadableLibraries(OutIter dest) const;

  // Get a list of already-loaded libraries (full paths). Returns the
  // number of entries.
  size_t getLoadedLibraries(std::vector<std::string>& list) const;
  template <class OutIter>
  size_t getLoadedLibraries(OutIter dest) const;

  // Get list of valid libspecs. Returns the number of entries.
  size_t getValidLibspecs(std::vector<std::string>& list) const;
  template <class OutIter>
  size_t getValidLibspecs(OutIter dest) const;

  // Get pair of short and full libspecs corresponding to library full
  // path.
  std::pair<std::string, std::string> getSpecsByPath(
    std::string const& lib_loc) const;

  // Load all libraries at once.
  void loadAllLibraries() const;

  // Check whether libraries are loaded.
  bool libraryIsLoaded(std::string const& path) const;
  // Check whether library is loadable. Note this is *not* efficient
  // and is only intended for diagnostic purposes. If it needs to be
  // efficient the implementation will need to be improved.
  bool libraryIsLoadable(std::string const& path) const;

  // This manager's library type.
  std::string
  libType() const
  {
    return lib_type_;
  }

  // This managers library search pattern.
  std::string
  patternStem() const
  {
    return pattern_stem_;
  }

private:
  // Internally-useful typedefs.
  using lib_loc_map_t = std::map<std::string, std::string>;
  using spec_trans_map_t = std::map<std::string, std::set<std::string>>;
  using lib_ptr_map_t = std::map<std::string, void*>;
  using good_spec_trans_map_t = std::map<std::string, std::string>;

  // Private helper functions.
  static std::string dllExtPattern();

  void lib_loc_map_inserter(std::string const& path);
  void spec_trans_map_inserter(lib_loc_map_t::value_type const& entry);
  void good_spec_trans_map_inserter(spec_trans_map_t::value_type const& entry);
  void* get_lib_ptr(std::string const& lib_loc) const;
  void* getSymbolByLibspec_(std::string const& libspec,
                            std::string const& sym_name,
                            bool should_throw_on_dlsym = true) const;
  void* getSymbolByPath_(std::string const& lib_loc,
                         std::string const& sym_name,
                         bool should_throw_on_dlsym = true) const;

  cet::search_path const search_path_;
  std::string const lib_type_;     // eg _plugin.
  std::string const pattern_stem_; // Library search pattern stem.
  // Map of library filename -> full path.
  lib_loc_map_t lib_loc_map_{};
  // Map of spec -> full path.
  spec_trans_map_t spec_trans_map_{};
  // Map of only good translations.
  good_spec_trans_map_t good_spec_trans_map_{};
  // Cache of already-loaded libraries.
  mutable lib_ptr_map_t lib_ptr_map_{};
};

inline std::string
cet::LibraryManager::dllExtPattern()
{
  static std::string const dllExtPatt{"\\" + shlib_suffix()};
  return dllExtPatt;
}

template <typename T>
inline T
cet::LibraryManager::getSymbolByLibspec(std::string const& libspec,
                                        std::string const& sym_name) const
{
  return hard_cast<T>(getSymbolByLibspec_(libspec, sym_name));
}

template <typename T>
inline void
cet::LibraryManager::getSymbolByLibspec(std::string const& libspec,
                                        std::string const& sym_name,
                                        T& sym) const
{
  hard_cast<T>(getSymbolByLibspec_(libspec, sym_name), sym);
}

template <typename T>
inline T
cet::LibraryManager::getSymbolByPath(std::string const& lib_loc,
                                     std::string const& sym_name) const
{
  return hard_cast<T>(getSymbolByPath_(lib_loc, sym_name));
}

template <typename T>
inline void
cet::LibraryManager::getSymbolByPath(std::string const& lib_loc,
                                     std::string const& sym_name,
                                     T& sym) const
{
  hard_cast<T>(getSymbolByPath_(lib_loc, sym_name), sym);
}

template <class OutIter>
size_t
cet::LibraryManager::getLoadableLibraries(OutIter dest) const
{
  size_t count{};
  for (auto const& lib_loc : lib_loc_map_) {
    *dest++ = lib_loc.second;
    ++count;
  }
  return count;
}

// Versions which won't throw on failure to obtain symbol.
template <typename T>
inline T
cet::LibraryManager::getSymbolByLibspec(std::string const& libspec,
                                        std::string const& sym_name,
                                        nothrow_t) const
{
  return hard_cast<T>(getSymbolByLibspec_(libspec, sym_name, false));
}

template <typename T>
inline void
cet::LibraryManager::getSymbolByLibspec(std::string const& libspec,
                                        std::string const& sym_name,
                                        T& sym,
                                        nothrow_t) const
{
  hard_cast<T>(getSymbolByLibspec_(libspec, sym_name, false), sym);
}

template <typename T>
inline T
cet::LibraryManager::getSymbolByPath(std::string const& lib_loc,
                                     std::string const& sym_name,
                                     nothrow_t) const
{
  return hard_cast<T>(getSymbolByPath_(lib_loc, sym_name, false));
}

template <typename T>
inline void
cet::LibraryManager::getSymbolByPath(std::string const& lib_loc,
                                     std::string const& sym_name,
                                     T& sym,
                                     nothrow_t) const
{
  hard_cast<T>(getSymbolByPath_(lib_loc, sym_name, false), sym);
}

template <class OutIter>
size_t
cet::LibraryManager::getLoadedLibraries(OutIter dest) const
{
  size_t count{};
  for (auto const& lib_ptr : lib_ptr_map_) {
    *dest++ = lib_ptr.first;
    ++count;
  }
  return count;
}

template <class OutIter>
size_t
cet::LibraryManager::getValidLibspecs(OutIter dest) const
{
  size_t count{};
  for (auto const& spec_trans : spec_trans_map_) {
    *dest++ = spec_trans.first;
    ++count;
  }
  return count;
}

#endif /* cetlib_LibraryManager_h */

// Local Variables:
// mode: c++
// End:
