#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wsign-conversion"

namespace boost {
  namespace mpl {
    // Commenting the next line make the assert failure go away
    // Changing the class name also make it go away
    struct forward_iterator_tag {};
    // struct forward_iterator_tag : int_<0> { typedef forward_iterator_tag
    // type; };
  }
}

#pragma GCC diagnostic pop
