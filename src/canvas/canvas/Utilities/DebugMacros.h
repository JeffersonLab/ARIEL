#ifndef canvas_Utilities_DebugMacros_h
#define canvas_Utilities_DebugMacros_h
// vim: set sw=2 expandtab :

#include "canvas/Utilities/fwd.h"
#include "hep_concurrency/tsan.h"

#include <atomic>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace art {

  struct DebugValue {

    DebugValue();

    int
    operator()()
    {
      return value_.load();
    }

    std::atomic<char const*> cvalue_;

    std::atomic<int> value_;
  };

  struct DebugTasksValue {

    DebugTasksValue();

    int
    operator()()
    {
      return value_.load();
    }

    std::atomic<char const*> cvalue_;

    std::atomic<int> value_;
  };

  extern DebugValue debugit;
  extern DebugTasksValue debugTasks;
}

#define FDEBUG(LEVEL)                                                          \
  if ((LEVEL) <= art::debugit())                                               \
  std::cerr

#define TDEBUG(LEVEL)                                                          \
  if ((LEVEL) <= art::debugTasks())                                            \
  std::cerr

#define TDEBUG_BEGIN_FUNC_SI(LEVEL, FUNC, SI)                                  \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      ostringstream buf_;                                                      \
      buf_ << "----->"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "Begin " << FUNC << "\n";                                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_END_FUNC_SI(LEVEL, FUNC, SI)                                    \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      ostringstream buf_;                                                      \
      buf_ << "----->"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "End   " << FUNC << "\n";                                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_END_FUNC_SI_ERR(LEVEL, FUNC, SI, ERR)                           \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      ostringstream buf_;                                                      \
      buf_ << "----->"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "End   " << FUNC << " - " << ERR << "\n";                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_FUNC_SI_MSG(LEVEL, FUNC, SI, MSG)                               \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "----->"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "      " << FUNC << " - " << MSG << "\n";                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_FUNC_MSG(LEVEL, FUNC, MSG)                                      \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "----->"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "    "                                                           \
           << " "                                                              \
           << "      " << FUNC << " - " << MSG << "\n";                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_TASK_SI_MSG(LEVEL, TASK, SI, MSG)                               \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "=====>"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "      " << TASK << " - " << MSG << "\n";                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_TASK_MSG(LEVEL, TASK, MSG)                                      \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "=====>"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "    "                                                           \
           << " "                                                              \
           << "      " << TASK << " - " << MSG << "\n";                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_BEGIN_TASK_SI(LEVEL, TASK, SI)                                  \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "=====>"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "Begin " << TASK << "\n";                                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_END_TASK_SI(LEVEL, TASK, SI)                                    \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "=====>"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "End   " << TASK << "\n";                                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#define TDEBUG_END_TASK_SI_ERR(LEVEL, TASK, SI, ERR)                           \
  {                                                                            \
    if ((LEVEL) <= art::debugTasks()) {                                        \
      auto tid_ = hep::concurrency::getThreadID();                             \
      std::ostringstream buf_;                                                 \
      buf_ << "=====>"                                                         \
           << " " << std::setw(6) << std::right << tid_ << std::left << " "    \
           << "[" << std::setw(2) << std::right << std::setfill('0') << SI     \
           << std::setfill(' ') << std::left << "]"                            \
           << " "                                                              \
           << "End   " << TASK << " - " << ERR << "\n";                        \
      std::cerr << buf_.str();                                                 \
    }                                                                          \
  }

#endif /* canvas_Utilities_DebugMacros_h */

// Local Variables:
// mode: c++
// End:
