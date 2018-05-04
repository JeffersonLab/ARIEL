#ifndef fhiclcpp_stdmap_shims_h
#define fhiclcpp_stdmap_shims_h

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "fhiclcpp/parse_shims_opts.h"

namespace shims {

  template <class Key,
            class T,
            class Compare = std::less<Key>,
            class Allocator = std::allocator<std::pair<const Key, T>>>
  class map {
  public:
    using mapmap_t = typename std::map<const Key, T, Compare, Allocator>;
    using listmap_t = typename std::list<std::pair<const Key, T>, Allocator>;

    static_assert(
      std::is_same<typename mapmap_t::key_type,
                   typename listmap_t::value_type::first_type>::value,
      "type mismatch for key_type");
    static_assert(
      std::is_same<typename mapmap_t::mapped_type,
                   typename listmap_t::value_type::second_type>::value,
      "type mismatch for mapped_type");
    static_assert(std::is_same<typename mapmap_t::value_type,
                               typename listmap_t::value_type>::value,
                  "type mismatch for value_type");
    static_assert(std::is_same<typename mapmap_t::size_type,
                               typename listmap_t::size_type>::value,
                  "type mismatch for size_type");

    using size_type = typename mapmap_t::size_type;

    using iterator_tag = std::input_iterator_tag;
    struct iterator_tuple {
      typename mapmap_t::iterator mapmap_iter;
      typename listmap_t::iterator listmap_iter;
    };

    template <class Category,
              class TT,
              class Distance = std::ptrdiff_t,
              class Pointer = TT*,
              class Reference = TT&>
    struct iter : std::iterator<Category, TT, Distance, Pointer, Reference> {
      using type = TT;

      iter(typename mapmap_t::iterator it) { _iters.mapmap_iter = it; }
      iter(typename listmap_t::iterator it) { _iters.listmap_iter = it; }

      TT& operator*()
      {
        return isSnippetMode() ? *_iters.listmap_iter : *_iters.mapmap_iter;
      }

      TT* operator->()
      {
        return isSnippetMode() ? &*_iters.listmap_iter : &*_iters.mapmap_iter;
      }
      TT const* operator->() const
      {
        return isSnippetMode() ? &*_iters.listmap_iter : &*_iters.mapmap_iter;
      }

      TT& operator++()
      {
        return isSnippetMode() ? *(_iters.listmap_iter++) :
                                 *(_iters.mapmap_iter++);
      }

      bool
      operator==(iter other) const
      {
        return isSnippetMode() ?
                 _iters.listmap_iter == other._iters.listmap_iter :
                 _iters.mapmap_iter == other._iters.mapmap_iter;
      }

      bool
      operator!=(iter other) const
      {
        return !operator==(other);
      }

      template <typename II>
      typename std::enable_if<
        std::is_same<typename mapmap_t::iterator, II>::value,
        II>::type get(II)
      {
        return _iters.mapmap_iter;
      }

      template <typename II>
      typename std::enable_if<
        std::is_same<typename listmap_t::iterator, II>::value,
        II>::type get(II)
      {
        return _iters.listmap_iter;
      }

      template <typename IIL, typename IIR>
      friend typename std::enable_if<
        !std::is_same<IIL, IIR>::value &&
          std::is_same<
            typename std::remove_const<typename IIL::type>::type,
            typename std::remove_const<typename IIR::type>::type>::value,
        bool>::type
      operator==(IIL, IIR);

      template <typename IIL, typename IIR>
      friend typename std::enable_if<
        !std::is_same<IIL, IIR>::value &&
          std::is_same<
            typename std::remove_const<typename IIL::type>::type,
            typename std::remove_const<typename IIR::type>::type>::value,
        bool>::type
      operator!=(IIL, IIR);

    private:
      iterator_tuple _iters;
    };

    using iterator = iter<iterator_tag, std::pair<const Key, T>>;
    using const_iterator = iter<iterator_tag, const std::pair<const Key, T>>;

    struct maps_tuple {
      mapmap_t mapmap;
      listmap_t listmap;
    };

    T& operator[](Key const& key)
    {
      if (isSnippetMode()) {
        for (auto& element : _maps.listmap) {
          if (element.first == key)
            return element.second;
        }
        _maps.listmap.emplace_back(std::make_pair(key, T{}));
        return _maps.listmap.back().second;
      } else {
        return _maps.mapmap[key];
      }
    }

    iterator
    begin()
    {
      return isSnippetMode() ? iterator{std::begin(_maps.listmap)} :
                               iterator{std::begin(_maps.mapmap)};
    }

    const_iterator
    begin() const
    {
      maps_tuple& maps = *const_cast<maps_tuple*>(&_maps);

      return isSnippetMode() ? const_iterator{std::begin(maps.listmap)} :
                               const_iterator{std::begin(maps.mapmap)};
    }

    const_iterator
    cbegin() const
    {
      return begin();
    }

    iterator
    end()
    {
      return isSnippetMode() ? iterator{std::end(_maps.listmap)} :
                               iterator{std::end(_maps.mapmap)};
    }

    const_iterator
    end() const
    {
      maps_tuple& maps = *const_cast<maps_tuple*>(&_maps);

      return isSnippetMode() ? const_iterator{std::end(maps.listmap)} :
                               const_iterator{std::end(maps.mapmap)};
    }

    const_iterator
    cend() const
    {
      return end();
    }

    T&
    at(Key const& key)
    {
      if (isSnippetMode()) {
        auto it =
          std::find_if(_maps.listmap.begin(),
                       _maps.listmap.end(),
                       [&key](auto& element) { return element.first == key; });
        if (it == _maps.listmap.end())
          throw std::out_of_range("Key <" + key + "> not found.");
        return it->second;
      } else {
        return _maps.mapmap.at(key);
      }
    }

    T const&
    at(Key const& key) const
    {
      if (isSnippetMode()) {
        auto it = std::find_if(
          _maps.listmap.cbegin(),
          _maps.listmap.cend(),
          [&key](auto const& element) { return element.first == key; });
        if (it == _maps.listmap.cend())
          throw std::out_of_range("Key <" + key + "> not found.");
        return it->second;
      } else {
        return _maps.mapmap.at(key);
      }
    }

    iterator
    find(Key const& key)
    {
      if (isSnippetMode()) {
        return std::find_if(
          _maps.listmap.begin(), _maps.listmap.end(), [&key](auto& element) {
            return element.first == key;
          });
      } else {
        return _maps.mapmap.find(key);
      }
    }

    const_iterator
    find(Key const& key) const
    {
      maps_tuple& maps = *const_cast<maps_tuple*>(&_maps);

      if (isSnippetMode()) {
        return std::find_if(
          maps.listmap.begin(),
          maps.listmap.end(),
          [&key](auto const& element) { return element.first == key; });
      } else {
        return maps.mapmap.find(key);
      }
    }

    size_t
    erase(Key const& key)
    {
      if (isSnippetMode()) {
        auto erase_count = size_t{0};
        auto i = _maps.listmap.begin();
        auto e = _maps.listmap.end();

        while (i != e) {
          if (key == i->first) {
            i = _maps.listmap.erase(i);
            ++erase_count;
          } else {
            i++;
          }
        }

        return erase_count;
      } else {
        return _maps.mapmap.erase(key);
      }
    }

    bool
    empty() const
    {
      return isSnippetMode() ? _maps.listmap.empty() : _maps.mapmap.empty();
    }

    size_type
    size() const
    {
      return isSnippetMode() ? _maps.listmap.size() : _maps.mapmap.size();
    }

    iterator
    erase(iterator it)
    {
      if (isSnippetMode()) {
        return _maps.listmap.erase(it.get(typename listmap_t::iterator{}));
      } else {
        return _maps.mapmap.erase(it.get(typename mapmap_t::iterator{}));
      }
    }

    iterator
    erase(const_iterator& it)
    {
      if (isSnippetMode()) {
        return _maps.listmap.erase(it.get(typename listmap_t::iterator{}));
      } else {
        return _maps.mapmap.erase(it.get(typename mapmap_t::iterator{}));
      }
    }

    template <class... Args>
    std::pair<iterator, bool>
    emplace(Args&&... args)
    {
      if (isSnippetMode()) {
        _maps.listmap.emplace_back(std::forward<Args>(args)...);
        return std::make_pair(iterator{std::prev(_maps.listmap.end())}, true);
      } else {
        auto result = _maps.mapmap.emplace(std::forward<Args>(args)...);
        return std::make_pair(iterator{result.first}, result.second);
      }
    }

    maps_tuple _maps;
  };
  template <typename IIL, typename IIR>
  typename std::enable_if<
    !std::is_same<IIL, IIR>::value &&
      std::is_same<typename std::remove_const<typename IIL::type>::type,
                   typename std::remove_const<typename IIR::type>::type>::value,
    bool>::type
  operator==(IIL left, IIR right)
  {
    return isSnippetMode() ?
             left._iters.listmap_iter == right._iters.listmap_iter :
             left._iters.mapmap_iter == right._iters.mapmap_iter;
  }

  template <typename IIL, typename IIR>
  typename std::enable_if<
    !std::is_same<IIL, IIR>::value &&
      std::is_same<typename std::remove_const<typename IIL::type>::type,
                   typename std::remove_const<typename IIR::type>::type>::value,
    bool>::type
  operator!=(IIL left, IIR right)
  {
    return !operator==(left, right);
  }
}

#endif /* fhiclcpp_stdmap_shims_h */
