// t0543.cc
// dependent qualified variable name in default argument

template < typename _Tp >
class allocator;

template < typename _Tp > 
struct __is__pod {};

template < typename _Val, 
           typename _Compare, 
           typename _Alloc = allocator < _Val > >
class _Rb_tree {
  template < typename _Key_compare,
             bool _Is__pod_comparator = __is__pod < _Key_compare >::_M_type >
  struct _Rb_tree_impl {};

  _Rb_tree_impl < _Compare > _M_impl;
};



// variant inside a namespace
namespace std {
  template < typename _Tp >
  class allocator;

  template < typename _Tp >
  struct __is__pod {};

  template < typename _Val,
             typename _Compare,
             typename _Alloc = allocator < _Val > >
  class _Rb_tree {
    template < typename _Key_compare,
               bool _Is__pod_comparator = std::__is__pod < _Key_compare >::_M_type >
    struct _Rb_tree_impl {};

    _Rb_tree_impl < _Compare > _M_impl;
  };
}
