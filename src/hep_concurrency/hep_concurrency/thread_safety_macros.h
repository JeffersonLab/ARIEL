#ifndef hep_concurrency_thread_safety_macros_h
#define hep_concurrency_thread_safety_macros_h
#ifndef __ROOTCLING__
#define CMS_THREAD_SAFE [[cms::thread_safe]]
#define CMS_THREAD_GUARD(_var_) [[cms::thread_guard("#_var_")]]
#else
#define CMS_THREAD_SAFE
#define CMS_THREAD_GUARD(_var_)
#endif
#endif /* hep_concurrency_thread_safety_macros_h */
