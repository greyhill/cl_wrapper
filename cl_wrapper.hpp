#ifndef _CL_WRAPPER_HPP_
#define _CL_WRAPPER_HPP_

#include <CL/cl.h>

#include <stdexcept>
#include <string>
#include <vector>

#define CHECK_CL_ERROR(err) if((err) != CL_SUCCESS) throw cl_error((err));

namespace cl {

static const char* cl_error_string(cl_int err) {
  switch(err) {
    case CL_SUCCESS:                          
      return "Success!";
    case CL_DEVICE_NOT_FOUND:                 
      return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:             
      return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:           
      return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:    
      return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:                 
      return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:               
      return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:     
      return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:                 
      return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:            
      return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:       
      return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:            
      return "Program build failure";
    case CL_MAP_FAILURE:                      
      return "Map failure";
    case CL_INVALID_VALUE:                    
      return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:              
      return "Invalid device type";
    case CL_INVALID_PLATFORM:                 
      return "Invalid platform";
    case CL_INVALID_DEVICE:                   
      return "Invalid device";
    case CL_INVALID_CONTEXT:                  
      return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:         
      return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:            
      return "Invalid command queue";
    case CL_INVALID_HOST_PTR:                 
      return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:               
      return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  
      return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:               
      return "Invalid image size";
    case CL_INVALID_SAMPLER:                  
      return "Invalid sampler";
    case CL_INVALID_BINARY:                   
      return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:            
      return "Invalid build options";
    case CL_INVALID_PROGRAM:                  
      return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:       
      return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:              
      return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:        
      return "Invalid kernel definition";
    case CL_INVALID_KERNEL:                   
      return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:                
      return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:                
      return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:                 
      return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:              
      return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:           
      return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:          
      return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:           
      return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:            
      return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:          
      return "Invalid event wait list";
    case CL_INVALID_EVENT:                    
      return "Invalid event";
    case CL_INVALID_OPERATION:                
      return "Invalid operation";
    case CL_INVALID_GL_OBJECT:                
      return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:              
      return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:                
      return "Invalid mip-map level";
    default:                                  
      return "Unknown";
  }
}

template<int UNUSED>
class cl_error_ : public std::runtime_error {
public:
  cl_error_(cl_int err) : std::runtime_error(cl_error_string(err)),
      err_(err) { }

  cl_int err_code() const { return err_; }

private:
  cl_int err_;
};
typedef cl_error_<0> cl_error;

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
  static inline void ref(cl_kernel kernel) { clRetainKernel(kernel); }
  static inline void unref(cl_kernel kernel) { clReleaseKernel(kernel); }
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

template<>
struct cl_wrapper_detail<cl_command_queue> {
  static inline void ref(cl_command_queue queue) {
    clRetainCommandQueue(queue);
  }
  static inline void unref(cl_command_queue queue) {
    clReleaseCommandQueue(queue);
  }
};

template<typename PT, typename CPPTYPE>
struct platform_property_functor { };

template<typename PT>
struct platform_property_functor<PT, std::string> {
  std::string operator()(const PT &platform, cl_uint prop_name) const {
    cl_int err;
    size_t size;
    err = clGetPlatformInfo(platform.id(), prop_name, 0, NULL, &size);
    CHECK_CL_ERROR(err);
    std::string to_return;
    to_return.resize(size);
    err = clGetPlatformInfo(platform.id(), prop_name, size,
        &to_return[0], NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }
};

template<typename DT, typename CPPTYPE>
struct device_property_functor { 
  CPPTYPE operator()(const DT &device, cl_uint prop_name) const {
    cl_int err;
    CPPTYPE to_return;
    err = clGetDeviceInfo(device.id(), prop_name, sizeof(CPPTYPE),
        &to_return, NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }
};

template<typename DT>
struct device_property_functor<DT, std::string> {
  std::string operator()(const DT &device, cl_uint prop_name) const {
    cl_int err;
    size_t size;
    err = clGetDeviceInfo(device.id(), prop_name, 0, NULL, &size);
    CHECK_CL_ERROR(err);
    std::string to_return;
    to_return.resize(size);
    err = clGetDeviceInfo(device.id(), prop_name, size, &to_return[0],
        NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }
};

template<typename DT>
struct device_property_functor<DT, bool> {
  bool operator()(const DT &device, cl_uint prop_name) const {
    return device_property_functor<DT, cl_bool>()(device, prop_name) ==
      CL_TRUE;
  }
};

template<typename DT>
struct device_property_functor<DT, std::vector<size_t> > {
  std::vector<size_t> operator()(const DT &device, cl_uint prop_name)
      const {
    cl_int err;
    size_t size;
    err = clGetDeviceInfo(device.id(), prop_name, 0, NULL, &size);
    CHECK_CL_ERROR(err);
    std::vector<size_t> to_return(size / sizeof(size_t));
    err = clGetDeviceInfo(device.id(), prop_name, size, &to_return[0],
        NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }
};

template<typename CT, typename CPPTYPE>
struct context_property_functor {
  CPPTYPE operator()(const CT &context, cl_uint prop_name) const {
    cl_int err;
    CPPTYPE to_return;
    err = clGetContextInfo(context.id(), prop_name, sizeof(to_return),
        &to_return, NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }
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
    upref_();
  }
  /** \brief new wrapper object at t; don't increment refcount */
  cl_wrapper(T t)
      : ref_(t) {
  }
  virtual ~cl_wrapper() {
    downref_();
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
    downref_();
    ref_ = t;
    upref_();
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
  /** \brief behaves like reset() with no automatic reference increment */
  cl_wrapper operator=(const T &t) {
    if(ref_ == t) return *this;
    downref_();
    ref_ = t;
    return *this;
  }

  /** \brief automatically convert to underlying C type */
  operator T() const {
    return ref_;
  }
  /** \brief convert to underlying C type */
  T id() const {
    return ref_;
  }

protected:
  void upref_() {
    if(ref_) detail::cl_wrapper_detail<T>::ref(ref_);
  }

  void downref_() {
    if(ref_) detail::cl_wrapper_detail<T>::unref(ref_);
  }

  T ref_;
};

/** \brief opencl device management class.  get a list of all the
 * devices available for a platform using the platform.devices() member
 * function. */
template<int UNUSED>
class device_ {
public:
  device_()
      : id_(NULL) { }
  device_(cl_device_id id)
      : id_(id) { }
  device_(const device_ &d)
      : id_(d.id_) { }
  ~device_() { }

  device_& operator=(const device_ &d) { id_ = d.id_; return *this; }
  bool operator==(const device_ &d) { return id_ == d.id_; }

#define DEVICE_PROPERTY(name, cl_name, type) \
  type name() const { \
    return detail::device_property_functor<device_<0>, type >()(*this, \
        cl_name); \
  }
  DEVICE_PROPERTY(address_bits, CL_DEVICE_ADDRESS_BITS, cl_uint);
  DEVICE_PROPERTY(available, CL_DEVICE_AVAILABLE, bool);
  DEVICE_PROPERTY(compiler_available, CL_DEVICE_COMPILER_AVAILABLE,
      bool);
  // CL_DEVICE_DOUBLE_FP_CONFIG
  DEVICE_PROPERTY(endian_little, CL_DEVICE_ENDIAN_LITTLE, bool);
  DEVICE_PROPERTY(error_correction_support,
      CL_DEVICE_ERROR_CORRECTION_SUPPORT, bool);
  DEVICE_PROPERTY(execution_capabilities,
      CL_DEVICE_EXECUTION_CAPABILITIES, cl_device_exec_capabilities);
  DEVICE_PROPERTY(extensions, CL_DEVICE_EXTENSIONS, std::string);
  DEVICE_PROPERTY(global_mem_cache_size,
      CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, cl_ulong);
  DEVICE_PROPERTY(global_mem_cache_type,
      CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, cl_device_mem_cache_type);
  DEVICE_PROPERTY(global_mem_cacheline_size,
      CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, cl_uint);
  DEVICE_PROPERTY(global_mem_size, CL_DEVICE_GLOBAL_MEM_SIZE, cl_ulong);
  // CL_DEVICE_HALF_FP_CONFIG
  DEVICE_PROPERTY(image_support, CL_DEVICE_IMAGE_SUPPORT, bool);
  DEVICE_PROPERTY(image2d_max_height, CL_DEVICE_IMAGE2D_MAX_HEIGHT,
      size_t);
  DEVICE_PROPERTY(image2d_max_width, CL_DEVICE_IMAGE2D_MAX_WIDTH,
      size_t);
  DEVICE_PROPERTY(image3d_max_depth, CL_DEVICE_IMAGE3D_MAX_DEPTH,
      size_t);
  DEVICE_PROPERTY(image3d_max_height, CL_DEVICE_IMAGE3D_MAX_HEIGHT,
      size_t);
  DEVICE_PROPERTY(image3d_max_width, CL_DEVICE_IMAGE3D_MAX_WIDTH,
      size_t);
  DEVICE_PROPERTY(local_mem_size, CL_DEVICE_LOCAL_MEM_SIZE, cl_ulong);
  // CL_DEVICE_LOCAL_MEM_TYPE
  DEVICE_PROPERTY(max_clock_frequency, CL_DEVICE_MAX_CLOCK_FREQUENCY,
      cl_uint);
  DEVICE_PROPERTY(max_compute_units, CL_DEVICE_MAX_COMPUTE_UNITS,
      cl_uint);
  DEVICE_PROPERTY(max_constant_args, CL_DEVICE_MAX_CONSTANT_ARGS,
      cl_uint);
  DEVICE_PROPERTY(max_constant_buffer_size,
      CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, cl_ulong);
  DEVICE_PROPERTY(max_mem_alloc_size, CL_DEVICE_MAX_MEM_ALLOC_SIZE,
      cl_ulong);
  DEVICE_PROPERTY(max_parameter_size, CL_DEVICE_MAX_PARAMETER_SIZE,
      size_t);
  DEVICE_PROPERTY(max_read_image_args, CL_DEVICE_MAX_READ_IMAGE_ARGS,
      cl_uint);
  DEVICE_PROPERTY(max_samplers, CL_DEVICE_MAX_SAMPLERS, cl_uint);
  DEVICE_PROPERTY(max_work_group_size, CL_DEVICE_MAX_WORK_GROUP_SIZE,
      size_t);
  DEVICE_PROPERTY(max_work_item_dimensions,
      CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, cl_uint);
  DEVICE_PROPERTY(max_work_item_sizes, CL_DEVICE_MAX_WORK_ITEM_SIZES,
      std::vector<size_t>);
  DEVICE_PROPERTY(max_write_image_args, CL_DEVICE_MAX_WRITE_IMAGE_ARGS,
      cl_uint);
  DEVICE_PROPERTY(mem_base_addr_align, CL_DEVICE_MEM_BASE_ADDR_ALIGN,
      cl_uint);
  DEVICE_PROPERTY(min_data_type_align_size,
      CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, cl_uint);
  DEVICE_PROPERTY(name, CL_DEVICE_NAME, std::string);
  DEVICE_PROPERTY(platform, CL_DEVICE_PLATFORM, cl_platform_id);
  DEVICE_PROPERTY(preferred_vector_width_char,
      CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, cl_uint);
  DEVICE_PROPERTY(preferred_vector_width_short,
      CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, cl_uint);
  DEVICE_PROPERTY(preferred_vector_width_int,
      CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, cl_uint);
  DEVICE_PROPERTY(preferred_vector_width_long,
      CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, cl_uint);
  DEVICE_PROPERTY(preferred_vector_width_float,
      CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, cl_uint);
  DEVICE_PROPERTY(preferred_vector_width_double,
      CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, cl_uint);
  DEVICE_PROPERTY(profile, CL_DEVICE_PROFILE, std::string);
  DEVICE_PROPERTY(profiling_timer_resolution,
      CL_DEVICE_PROFILING_TIMER_RESOLUTION, size_t);
  DEVICE_PROPERTY(queue_properties, CL_DEVICE_QUEUE_PROPERTIES,
      cl_command_queue_properties);
  DEVICE_PROPERTY(single_fp_config, CL_DEVICE_SINGLE_FP_CONFIG,
      cl_device_fp_config);
  DEVICE_PROPERTY(type, CL_DEVICE_TYPE, cl_device_type);
  DEVICE_PROPERTY(vendor, CL_DEVICE_VENDOR, std::string);
  DEVICE_PROPERTY(vendor_id, CL_DEVICE_VENDOR_ID, cl_uint);
  DEVICE_PROPERTY(version, CL_DEVICE_VERSION, std::string);
  DEVICE_PROPERTY(driver_version, CL_DRIVER_VERSION, std::string);
#undef DEVICE_PROPERTY

  operator cl_device_id() const { return id_; }
  cl_device_id id() const { return id_; }

protected:
  cl_device_id id_;
};
typedef device_<0> device;

/** \brief opencl platform management class.  get a list of all
 * available platforms using the static member platform::platforms() */
template<int UNUSED>
class platform_ {
public:
  platform_() 
      : id_(NULL) { }
  platform_(cl_platform_id id)
      : id_(id) { }
  platform_(const platform_ &p)
      : id_(p.id_) {
  }
  ~platform_() { }

  platform_& operator=(const platform_ &p) { id_ = p.id_; return *this; }
  bool operator==(const platform_ &p) { return id_ == p.id_; }

#define PLATFORM_PROPERTY(name, cl_name, type) \
  type name() const { \
    return detail::platform_property_functor<platform_<0>, type>()(*this, \
        cl_name); \
  }
  PLATFORM_PROPERTY(profile, CL_PLATFORM_PROFILE, std::string);
  PLATFORM_PROPERTY(version, CL_PLATFORM_VERSION, std::string);
  PLATFORM_PROPERTY(name, CL_PLATFORM_NAME, std::string);
  PLATFORM_PROPERTY(vendor, CL_PLATFORM_VENDOR, std::string);
  PLATFORM_PROPERTY(extensions, CL_PLATFORM_EXTENSIONS, std::string);
#undef PLATFORM_PROPERTY

  std::vector<device> devices(cl_device_type type = CL_DEVICE_TYPE_ALL) 
      const {
    cl_int err;
    cl_uint num_devices;
    err = clGetDeviceIDs(id_, type, 0, NULL, &num_devices);
    CHECK_CL_ERROR(err);
    std::vector<device> to_return(num_devices);
    err = clGetDeviceIDs(id_, type, num_devices,
        reinterpret_cast<cl_device_id*>(&to_return[0]), NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }

  /** \brief returns a reference to a list of all platforms available */
  static const std::vector<platform_>& platforms() {
    if(!platforms_built_) {
      cl_int err;
      cl_uint num_platforms;
      err = clGetPlatformIDs(0, NULL, &num_platforms);
      CHECK_CL_ERROR(err);
      platforms_.resize(num_platforms);
      err = clGetPlatformIDs(num_platforms,
          reinterpret_cast<cl_platform_id*>(&platforms_[0]), NULL);
      CHECK_CL_ERROR(err);
    }
    return platforms_;
  }

  /** \brief automatically convert to CL type */
  operator cl_platform_id() const {
    return id_;
  }
  /** \brief convert to CL type */
  cl_platform_id id() const {
    return id_;
  }

protected:
  cl_platform_id id_;

private:
  static bool platforms_built_;
  static std::vector<platform_> platforms_;
};
template<int UNUSED> bool platform_<UNUSED>::platforms_built_ = false;
template<int UNUSED> std::vector<platform_<UNUSED> > 
    platform_<UNUSED>::platforms_;
typedef platform_<0> platform;

/** \brief OpenCL context wrapper */
template<int UNUSED>
class context_ : public cl_wrapper<cl_context> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  context_()
      : cl_wrapper<cl_context>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  context_(const context_ &cl) 
      : cl_wrapper<cl_context>(cl) { }
  /** \brief standard ctors; see cl_wrapper<> */
  context_(cl_context c)
      : cl_wrapper<cl_context>(c) { }
  /** \brief create a new context */
  context_(platform platform, int num_devices, const device *devices)
      : cl_wrapper<cl_context>() {
    cl_int err;
    cl_context c;
    cl_context_properties props[] = {
      CL_CONTEXT_PLATFORM, 
      (cl_context_properties)platform.id(), 
      (cl_context_properties)0 };
    c = clCreateContext(props, num_devices, 
        reinterpret_cast<const cl_device_id*>(&devices[0]),
        NULL, NULL, &err);
    CHECK_CL_ERROR(err);
    ref_ = c;
  }
  virtual ~context_() { }

#define CONTEXT_PROPERTY(name, cl_name, type) \
  type name() const { \
    return detail::context_property_functor<context_<0>, type>()(*this,\
        cl_name); \
  }
  // CL_CONTEXT_DEVICE
  // CL_CONTEXT_PROPERTIES
  CONTEXT_PROPERTY(reference_count, CL_CONTEXT_REFERENCE_COUNT,
      cl_uint);
#undef CONTEXT_PROPERTY
};
typedef context_<0> context;

/** \brief OpenCL buffer wrapper */
template<int UNUSED>
class buffer_ : public cl_wrapper<cl_mem> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  buffer_() 
      : cl_wrapper<cl_mem>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  buffer_(const buffer_ &b)
      : cl_wrapper<cl_mem>(b) { }
  /** \brief standard ctors; see cl_wrapper<> */
  buffer_(cl_mem m)
      : cl_wrapper<cl_mem>(m) { }
  /** \brief create a new opencl buffer */
  buffer_(const context &c, cl_mem_flags flags, size_t size, void
      *host_ptr = NULL) 
      : cl_wrapper<cl_mem>() {
    cl_int err;
    cl_mem m = NULL;
    m = clCreateBuffer(c.id(), flags, size, host_ptr, &err);
    CHECK_CL_ERROR(err);
    ref_ = m;
  }
  virtual ~buffer_() { }
};
typedef buffer_<0> buffer;

template<int UNUSED>
class image2d_ : public cl_wrapper<cl_mem> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  image2d_()
      : cl_wrapper<cl_mem>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  image2d_(const image2d_ &i)
      : cl_wrapper<cl_mem>(i) { }
  /** \brief standard ctors; see cl_wrapper<> */
  image2d_(cl_mem m)
      : cl_wrapper<cl_mem>(m) { }
  /** \brief create a new image2d object */
  image2d_(const context &context, cl_mem_flags flags,
      cl_channel_order channel_order,
      cl_channel_type channel_type,
      size_t width,
      size_t height,
      void *user_data = NULL,
      size_t pitch = 0)
      : cl_wrapper<cl_mem>() {
    cl_int err;
    cl_image_format format = { channel_order, channel_type };
    cl_mem i = clCreateImage2D(context.id(),
        flags,
        &format,
        width,
        height,
        pitch,
        user_data,
        &err);
    CHECK_CL_ERROR(err);
    ref_ = i;
  }
  virtual ~image2d_() { }
};
typedef image2d_<0> image2d;

template<int UNUSED>
class image3d_ : public cl_wrapper<cl_mem> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  image3d_() : cl_wrapper<cl_mem>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  image3d_(const image3d_ &i) : cl_wrapper<cl_mem>(i) { }
  /** \brief standard ctors; see cl_wrapper<> */
  image3d_(cl_mem m) : cl_wrapper<cl_mem>(m) { }
  /** \brief create a new 3d image */
  image3d_(const context &context, cl_mem_flags flags,
      cl_channel_order channel_order,
      cl_channel_type channel_type,
      size_t width,
      size_t height,
      size_t depth,
      void *user_data = NULL,
      size_t row_pitch = 0,
      size_t slice_pitch = 0)
      : cl_wrapper<cl_mem>() {
    cl_int err;
    cl_image_format format = { channel_order, channel_type };
    cl_mem i = clCreateImage3D(context.id(),
        flags,
        &format,
        width, height, depth,
        row_pitch, slice_pitch,
        user_data, &err);
    CHECK_CL_ERROR(err);
    ref_ = i;
  }
  virtual ~image3d_() { }
};
typedef image3d_<0> image3d;

template<int UNUSED>
class kernel_ : public cl_wrapper<cl_kernel> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  kernel_()
      : cl_wrapper<cl_kernel>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  kernel_(const kernel_ &k)
      : cl_wrapper<cl_kernel>(k) { }
  /** \brief standard ctors; see cl_wrapper<> */
  kernel_(cl_kernel k)
      : cl_wrapper<cl_kernel>(k) { }
  virtual ~kernel_() { }

  template<typename T>
  kernel_& set_arg(cl_uint index, const T &value) {
    cl_int err;
    err = clSetKernelArg(ref_, index, sizeof(T), &value);
    CHECK_CL_ERROR(err);
    return *this;
  }
};
typedef kernel_<0> kernel;

template<int UNUSED>
class program_ : public cl_wrapper<cl_program> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  program_() : cl_wrapper<cl_program>() { } 
  /** \brief standard ctors; see cl_wrapper<> */
  program_(const program_ &p) : cl_wrapper<cl_program>(p) { }
  /** \brief standard ctors; see cl_wrapper<> */
  program_(cl_program p) : cl_wrapper<cl_program>(p) { }
  /** \brief create program from source code */
  program_(const context &ctx, const std::string &source)
      : cl_wrapper<cl_program>() {
    cl_int err;
    const char *src_ptr = source.c_str();
    cl_program p = clCreateProgramWithSource(ctx.id(),
        1, &src_ptr, NULL, &err);
    CHECK_CL_ERROR(err);
    ref_ = p;
  }
  /** \brief create program from source code and build immediately.
   * n.b. this prevents you from getting to the error log if something
   * goes wrong. */
  program_(const context &ctx, const std::string &source, const
      std::string &opts) {
    cl_int err;
    const char *src_ptr = source.c_str();
    cl_program p = clCreateProgramWithSource(ctx.id(),
        1, &src_ptr, NULL, &err);
    CHECK_CL_ERROR(err);
    ref_ = p;
    build(opts);
  }
  virtual ~program_() { }

  /** \brief compile this program for all devices associated with this
   * program's context */
  void build(const std::string &opts = "") {
    cl_int err;
    err = clBuildProgram(ref_, 0, NULL, opts.c_str(), NULL, NULL);
    CHECK_CL_ERROR(err);
  }

  /** \brief returns the build log generated by compiling this program
   * */
  std::string build_log(const device &d) const {
    cl_int err;
    size_t log_size;
    err = clGetProgramBuildInfo(ref_, d.id(), CL_PROGRAM_BUILD_LOG, 0,
        NULL, &log_size);
    CHECK_CL_ERROR(err);
    std::string to_return;
    to_return.resize(log_size);
    err = clGetProgramBuildInfo(ref_, d.id(), CL_PROGRAM_BUILD_LOG,
        log_size, &to_return[0], NULL);
    CHECK_CL_ERROR(err);
    return to_return;
  }

  /** \brief returns a reference to a kernel contained in this program
   * */
  kernel get_kernel(const std::string &kname) const {
    cl_int err;
    cl_kernel k = clCreateKernel(ref_, kname.c_str(), &err);
    CHECK_CL_ERROR(err);
    kernel ker(k);
    return ker;
  }
};
typedef program_<0> program;

template<int UNUSED>
class event_ : public cl_wrapper<cl_event> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  event_() : cl_wrapper<cl_event>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  event_(const event_ &e) : cl_wrapper<cl_event>(e) { }
  /** \brief standard ctors; see cl_wrapper<> */
  event_(cl_event e) : cl_wrapper<cl_event>(e) { }
  virtual ~event_() { }

  void wait() {
    cl_int err;
    err = clWaitForEvents(1, &ref_);
    CHECK_CL_ERROR(err);
  }
};
typedef event_<0> event;

template<int N>
void wait(cl_uint num_events, event_<N> *events) {
  cl_int err;
  err = clWaitForEvents(num_events, 
      reinterpret_cast<cl_event*>(events));
  CHECK_CL_ERROR(err);
}

template<int UNUSED>
class command_queue_ : public cl_wrapper<cl_command_queue> {
public:
  /** \brief standard ctors; see cl_wrapper<> */
  command_queue_() : cl_wrapper<cl_command_queue>() { }
  /** \brief standard ctors; see cl_wrapper<> */
  command_queue_(const command_queue_ &q)
      : cl_wrapper<cl_command_queue>(q) { }
  /** \brief standard ctors; see cl_wrapper<> */
  command_queue_(cl_command_queue q)
      : cl_wrapper<cl_command_queue>(q) { }
  /** \brief create a new command queue */
  command_queue_(const context &c, const device &d,
    cl_command_queue_properties properties = 0) 
      : cl_wrapper<cl_command_queue>() {
    cl_int err;
    cl_command_queue q = NULL;
    q = clCreateCommandQueue(c.id(), d.id(), properties, &err);
    CHECK_CL_ERROR(err);
    ref_ = q;
  }
  virtual ~command_queue_() { } 

  event read_buffer(const buffer &src, size_t offset, size_t size, void
      *dest, cl_uint num_events = 0, event *events = NULL, 
      bool blocking = false) {
    cl_int err;
    event to_return;
    err = clEnqueueReadBuffer(ref_, src.id(),
        blocking ? CL_TRUE : CL_FALSE,
        offset,
        size,
        dest,
        num_events,
        reinterpret_cast<cl_event*>(events),
        reinterpret_cast<cl_event*>(&to_return));
    CHECK_CL_ERROR(err);
    return to_return;
  }

  event write_buffer(const buffer &dst, size_t offset, size_t size, 
      void *src, cl_uint num_events = 0, event *events = NULL, 
      bool blocking = false) {
    cl_int err;
    event to_return;
    err = clEnqueueWriteBuffer(ref_, dst.id(),
        blocking ? CL_TRUE : CL_FALSE,
        offset,
        size,
        src,
        num_events,
        reinterpret_cast<cl_event*>(events),
        reinterpret_cast<cl_event*>(&to_return));
    CHECK_CL_ERROR(err);
    return to_return;
  }

  event copy_buffer(const buffer &src, const buffer &dst, 
      size_t src_offset, size_t dst_offset,
      size_t size, 
      cl_uint num_events = 0, event *events = NULL) {
    cl_int err;
    event to_return;
    err = clEnqueueCopyBuffer(ref_, src.id(), dst.id(),
        src_offset, dst_offset, size,
        num_events, reinterpret_cast<cl_event*>(events),
        reinterpret_cast<cl_event*>(&to_return));
    CHECK_CL_ERROR(err);
    return to_return;
  }

  event run_kernel(const kernel &k, cl_uint work_dim, 
      const size_t *global_work_size,
      const size_t *local_work_size,
      cl_uint num_events = 0,
      event *events = NULL) {
    cl_int err;
    event to_return;
    err = clEnqueueNDRangeKernel(ref_, k.id(), work_dim,
        NULL, global_work_size, local_work_size,
        num_events,
        reinterpret_cast<cl_event*>(events),
        reinterpret_cast<cl_event*>(&to_return));
    CHECK_CL_ERROR(err);
    return to_return;
  }

  event marker() {
    cl_int err;
    event to_return;
    err = clEnqueueMarker(ref_,
        reinterpret_cast<cl_event*>(&to_return));
    CHECK_CL_ERROR(err);
    return to_return;
  }

  void enqueue_wait_for_events(cl_uint num_events, event *events) {
    cl_int err;
    err = clEnqueueWaitForEvents(ref_, num_events,
        reinterpret_cast<cl_event*>(events));
    CHECK_CL_ERROR(err);
  }

  // TODO image-related stuff
};
typedef command_queue_<0> command_queue;

#undef CHECK_CL_ERROR

}

#endif

