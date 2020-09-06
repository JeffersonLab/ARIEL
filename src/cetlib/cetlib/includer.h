#ifndef cetlib_includer_h
#define cetlib_includer_h

// ======================================================================
//
// includer: A container-like data structure that
//           a) transparently handles #include'd files, and
//           b) can trace back its iterators
//
// ======================================================================

#include "cetlib/filepath_maker.h"

#include <string>
#include <vector>

namespace cet {
  class includer;
}

// ----------------------------------------------------------------------

class cet::includer {
  using uint = std::string::size_type;

public:
  using const_iterator = std::string::const_iterator;

  explicit includer(std::string const& filename,
                    cet::filepath_maker& abs_filename);

  explicit includer(std::istream& is, cet::filepath_maker& abs_filename);

  const_iterator
  begin() const
  {
    return text_.begin();
  }
  const_iterator
  end() const
  {
    return text_.end();
  }
  const_iterator
  cbegin() const
  {
    return text_.cbegin();
  }
  const_iterator
  cend() const
  {
    return text_.cend();
  }
  std::string whereis(const_iterator const& it) const;
  std::string highlighted_whereis(const_iterator const& it) const;
  std::string src_whereis(const_iterator const& it) const;

private:
  struct frame {
    uint including_framenum;
    std::string filename;
    uint starting_linenum;
    size_t starting_textpos;
    // The new-line positions are used to determine line numbers
    // whenever includer::get_posinfo is called.
    std::vector<size_t> nl_positions{};

    frame(uint const framenum,
          std::string const& filename,
          uint const linenum,
          size_t const textpos)
      : including_framenum{framenum}
      , filename{filename}
      , starting_linenum{linenum}
      , starting_textpos{textpos}
    {}
  };

  struct posinfo {
    uint textpos;  // Character position in fully-included text.
    uint linenum;  // Line number (1-based for user info).
    uint charpos;  // Character position in line (1-based for user info).
    uint framenum; // Inclusion level.
  };

  std::string text_{};
  std::vector<frame> frames_;
  std::vector<std::string> recursionStack_{};

  void include(int including_framenum,
               std::string const& filename,
               cet::filepath_maker& abs_filename);

  void include(std::istream& is, cet::filepath_maker& abs_filename);

  std::string backtrace(uint from_frame) const;
  void debug() const;

  posinfo get_posinfo(const_iterator const& it) const;

}; // includer

// ======================================================================

#endif /* cetlib_includer_h */

// Local variables:
// mode: c++
// End:
