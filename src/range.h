#pragma once

#include <iterator>

namespace sysy {

/*!
 * \defgroup utility Utilities
 * @{
 */

/*!
 * \brief `range` is an simple wrapper of an iterator pair [begin, end)
 *
 * Example usage
 *
 * ```cpp
 *    vector<int> v = {1,2,3};
 *    auto rg = make_range(v);
 *    for (auto v : rg)
 *      cout << v << '\n';
 * ```
 */
template <typename IterT> struct range {
  using iterator = IterT;
  using value_type = typename std::iterator_traits<iterator>::value_type;
  using reference = typename std::iterator_traits<iterator>::reference;

private:
  iterator b;
  iterator e;

public:
  explicit range(iterator b, iterator e) : b(b), e(e) {}
  iterator begin() { return b; }
  iterator end() { return e; }
  iterator begin() const { return b; }
  iterator end() const { return e; }
  auto size() const { return std::distance(b, e); }
  auto empty() const { return b == e; }
};

//! create `range` object from iterator pair [begin, end)
template <typename IterT> range<IterT> make_range(IterT b, IterT e) {
  return range(b, e);
}
//! create `range` object from a container who has `begin()` and `end()` methods
template <typename ContainerT>
range<typename ContainerT::iterator> make_range(ContainerT &c) {
  return make_range(c.begin(), c.end());
}
//! create `range` object from a container who has `begin()` and `end()` methods
template <typename ContainerT>
range<typename ContainerT::const_iterator> make_range(const ContainerT &c) {
  return make_range(c.begin(), c.end());
}

/*!
 * @}
 */


} // namespace sysy