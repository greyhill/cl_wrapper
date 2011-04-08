#ifndef _CL_WRAPPER_HPP_
#define _CL_WRAPPER_HPP_

#include <CL/cl.h>

namespace cl {

namespace detail {

template<typename T>
struct cl_wrapper_detail {};

template<>
struct cl_wrapper_detail<cl_mem> {
  static inline void ref(cl_mem mem) { clRetainMemObject(mem); }
  static inline void unref(cl_mem mem) { clReleaseMemObject(mem); }
};

template<>
struct cl_wrapper_detail<cl_kernel> {
  inline void ref(cl_kernel kernel) { clRetainKernel(kernel); }
  inline void unref(cl_kernel kernel) { clReleaseKernel(kernel); }
};

template<>
struct cl_wrapper_detail<cl_program> {
  static inline void ref(cl_program program) { clRetainProgram(program); }
  static inline void unref(cl_program program) { clReleaseProgram(program); }
};

template<>
struct cl_wrapper_detail<cl_event> {
  static inline void ref(cl_event event) { clRetainEvent(event); }
  static inline void unref(cl_event event) { clReleaseEvent(event); }
};

template<>
struct cl_wrapper_detail<cl_context> {
  static inline void ref(cl_context context) { clRetainContext(context); }
  static inline void unref(cl_context context) { clReleaseContext(context); }
};

}

/** \brief reference-counted generic wrapper for OpenCL types.  behaves
 * somewhat similarly to Boost's shared_ptr<> */
template<typename T>
class cl_wrapper {
public:
  /** \brief new wrapper object pointed to NULL */
  cl_wrapper()
      : ref_(NULL) {
  }
  /** \brief new wrapper object copies other wrapper's reference */
  cl_wrapper(const cl_wrapper &w) 
      : ref_(w.ref_) {
    if(ref_) detail::cl_wrapper_detail<T>::ref(ref_);
  }
  /** \brief new wrapper object at t; increments refcount */
  cl_wrapper(T t)
      : ref_(t) {
  }
  virtual ~cl_wrapper() {
    if(ref_) detail::cl_wrapper_detail<T>::unref(ref_);
  }

  /** \brief reference access */
  T operator*() const {
    return ref_;
  }
  /** \brief reference access */
  T get() const {
    return ref_;
  }
  /** \brief modify reference */
  void reset(T t) {
    if(ref_ == t) return;
    if(ref_) detail::cl_wrapper_detail<T>::unref(ref_);
    ref_ = t;
    if(ref_) detail::cl_wrapper_detail<T>::ref(ref_);
  }

  /** \brief test equality by reference */
  bool operator==(const T &t) const {
    return ref_ == t;
  }
  /** \brief test equality by reference */
  bool operator==(const cl_wrapper &w) const {
    return ref_ == w.ref_;
  }
  /** \brief behaves like reset() */
  cl_wrapper operator=(const cl_wrapper &r) {
    reset(r.ref_);
    return *this;
  }
  /** \brief behaves like reset() */
  cl_wrapper operator=(const T &t) {
    reset(t);
    return *this;
  }

  /** \brief automatically convert to underlying C type */
  operator T() const {
    return ref_;
  }

protected:
  T ref_;
};

typedef cl_wrapper<cl_mem> mem;
typedef cl_wrapper<cl_kernel> kernel;
typedef cl_wrapper<cl_program> program;
typedef cl_wrapper<cl_event> event;

class device {
public:
private:
  cl_device_id id_;
};

/** \brief OpenCL context wrapper */
class context : public cl_wrapper<cl_context> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  context()
      : cl_wrapper<cl_context>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  context(const context &cl) 
      : cl_wrapper<cl_context>(cl) { }
  /** \brief standard ctors; see cl_wrapper<> */
  context(cl_context c)
      : cl_wrapper<cl_context>(c) { }
  virtual ~context() { }
};

}

#endif

