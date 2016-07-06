#ifndef THREE_ANY_IPP
#define THREE_ANY_IPP

namespace three {
namespace detail {

template<typename T>
  const std::type_info& typed_base_any_policy<T>::type() {
    return typeid(T);
  }

}
bool any::empty() const {
  return policy->type() == typeid(detail::empty_any);
}

} // namespace three

#endif // THREE_ANY_IPP