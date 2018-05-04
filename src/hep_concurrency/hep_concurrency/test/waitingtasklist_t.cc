// vim: set sw=2 expandtab :
//
//  WaitingTaskList_test.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/27/11.
//

#include "cppunit/extensions/HelperMacros.h"
#include "hep_concurrency/WaitingTaskList.h"
#include "tbb/task.h"

#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

#include <unistd.h>

#if (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4))
#define CXX_THREAD_AVAILABLE
#endif

class Waiter : public tbb::task {
public:
  tbb::task*
  execute()
  {
    return nullptr;
  }
};

class WaitingTaskList_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(WaitingTaskList_test);
  CPPUNIT_TEST(addThenDone);
  CPPUNIT_TEST(doneThenAdd);
  CPPUNIT_TEST(addThenDoneFailed);
  CPPUNIT_TEST(doneThenAddFailed);
  CPPUNIT_TEST(stressTest);
  CPPUNIT_TEST_SUITE_END();

public:
  void addThenDone();
  void doneThenAdd();
  void addThenDoneFailed();
  void doneThenAddFailed();
  void stressTest();
  void
  setUp()
  {}
  void
  tearDown()
  {}
};

#if 0
namespace {

class TestCalledTask : public hep::concurrency::WaitingTask {
public:
  TestCalledTask(std::atomic<bool>& iCalled, std::exception_ptr& iPtr): m_called(iCalled), m_ptr(iPtr) {}

  tbb::task* execute()
  {
    if (exceptionPtr()) {
      m_ptr = *exceptionPtr();
    }
    m_called = true;
    return nullptr;
  }

private:
  std::atomic<bool>& m_called;
  std::exception_ptr& m_ptr;
};

class TestValueSetTask : public hep::concurrency::WaitingTask {
public:
  TestValueSetTask(std::atomic<bool>& iValue): m_value(iValue) {}
  tbb::task* execute()
  {
    CPPUNIT_ASSERT(m_value);
    return nullptr;
  }

private:
  std::atomic<bool>& m_value;
};

} // unnamed namespace
#endif // 0

void
WaitingTaskList_test::addThenDone()
{
  std::atomic<bool> called{false};
  std::exception_ptr excPtr;
  auto TestCalledFunctor = [&called, &excPtr](exception_ptr* ex) {
    if (*ex) {
      excPtr = *ex;
    }
    called = true;
  };
  hep::concurrency::WaitingTaskList waitList;
  {
    auto waitTask = new (tbb::task::root_allocator()) Waiter;
    waitTask->set_ref_count(2);
    // NOTE: allocate_child does NOT increment the ref_count of waitTask!
    // auto t = new (waitTask->allocate_child()) TestCalledTask{called,excPtr};
    auto t = make_waiting_task(waitTask->allocate_child(), TestCalledFunctor);
    waitList.add(t);
    usleep(10);
    __sync_synchronize();
    CPPUNIT_ASSERT(false == called);
    waitList.doneWaiting(std::exception_ptr{});
    waitTask->wait_for_all();
    tbb::task::destroy(*waitTask);
    __sync_synchronize();
    CPPUNIT_ASSERT(true == called);
    CPPUNIT_ASSERT(bool(excPtr) == false);
  }
  waitList.reset();
  called = false;
  {
    std::exception_ptr excPtr;
    auto waitTask = new (tbb::task::root_allocator()) Waiter;
    waitTask->set_ref_count(2);
    // auto t = new (waitTask->allocate_child()) TestCalledTask{called, excPtr};
    auto t = make_waiting_task(waitTask->allocate_child(), TestCalledFunctor);
    waitList.add(t);
    usleep(10);
    CPPUNIT_ASSERT(false == called);
    waitList.doneWaiting(std::exception_ptr{});
    waitTask->wait_for_all();
    tbb::task::destroy(*waitTask);
    CPPUNIT_ASSERT(true == called);
    CPPUNIT_ASSERT(bool(excPtr) == false);
  }
}

void
WaitingTaskList_test::doneThenAdd()
{
  std::atomic<bool> called{false};
  std::exception_ptr excPtr;
  hep::concurrency::WaitingTaskList waitList;
  {
    auto waitTask = new (tbb::task::root_allocator()) Waiter;
    waitTask->set_ref_count(2);
    // auto t = new (waitTask->allocate_child()) TestCalledTask{called, excPtr};
    auto t = make_waiting_task(waitTask->allocate_child(), TestCalledFunctor);
    waitList.doneWaiting(std::exception_ptr{});
    waitList.add(t);
    waitTask->wait_for_all();
    tbb::task::destroy(*waitTask);
    CPPUNIT_ASSERT(true == called);
    CPPUNIT_ASSERT(bool(excPtr) == false);
  }
}

void
WaitingTaskList_test::addThenDoneFailed()
{
  std::atomic<bool> called{false};
  std::exception_ptr excPtr;
  hep::concurrency::WaitingTaskList waitList;
  {
    std::exception_ptr excPtr;
    auto waitTask = new (tbb::task::root_allocator()) Waiter;
    waitTask->set_ref_count(2);
    // auto t = new (waitTask->allocate_child()) TestCalledTask{called, excPtr};
    auto t = make_waiting_task(waitTask->allocate_child(), TestCalledFunctor);
    waitList.add(t);
    usleep(10);
    CPPUNIT_ASSERT(false == called);
    waitList.doneWaiting(std::make_exception_ptr(std::string("failed")));
    waitTask->wait_for_all();
    tbb::task::destroy(*waitTask);
    CPPUNIT_ASSERT(true == called);
    CPPUNIT_ASSERT(bool(excPtr) == true);
  }
}

void
WaitingTaskList_test::doneThenAddFailed()
{
  std::atomic<bool> called{false};
  std::exception_ptr excPtr;
  hep::concurrency::WaitingTaskList waitList;
  {
    auto waitTask = new (tbb::task::root_allocator()) Waiter;
    waitTask->set_ref_count(2);
    // auto t = new (waitTask->allocate_child()) TestCalledTask{called, excPtr};
    auto t = make_waiting_task(waitTask->allocate_child(), TestCalledFunctor);
    waitList.doneWaiting(std::make_exception_ptr(std::string("failed")));
    waitList.add(t);
    waitTask->wait_for_all();
    tbb::task::destroy(*waitTask);
    CPPUNIT_ASSERT(true == called);
    CPPUNIT_ASSERT(bool(excPtr) == true);
  }
}

namespace {
#if defined(CXX_THREAD_AVAILABLE)
  void
  join_thread(std::thread* iThread)
  {
    if (iThread->joinable()) {
      iThread->join();
    }
  }
#endif
}

void
WaitingTaskList_test::stressTest()
{
#if defined(CXX_THREAD_AVAILABLE)
  std::atomic<bool> called{false};
  std::exception_ptr excPtr;
  hep::concurrency::WaitingTaskList waitList;
  unsigned int index = 10;
  const unsigned int nTasks = 100;
  while (0 != --index) {
    called = false;
    auto waitTask = new (tbb::task::root_allocator()) Waiter;
    waitTask->set_ref_count(3);
    {
      std::thread makeTasksThread([&waitList, waitTask, &called, &excPtr] {
        for (unsigned int i = 0; i < nTasks; ++i) {
          // auto t = new (tbb::task::allocate_additional_child_of(*waitTask))
          // TestCalledTask{called, excPtr};
          auto t = make_waiting_task(
            tbb::task::allocate_additional_child_of(*waitTask),
            TestCalledFunctor);
          waitList.add(t);
        }
        waitTask->decrement_ref_count();
      });
      std::shared_ptr<std::thread>(&makeTasksThread, join_thread);
      std::thread doneWaitThread([&waitList, &called, waitTask] {
        called = true;
        waitList.doneWaiting(std::exception_ptr{});
        waitTask->decrement_ref_count();
      });
      std::shared_ptr<std::thread>(&doneWaitThread, join_thread);
    }
    waitTask->wait_for_all();
    tbb::task::destroy(*waitTask);
  }
#endif
}

CPPUNIT_TEST_SUITE_REGISTRATION(WaitingTaskList_test);

#include "hep_concurrency/test/CppUnit_testdriver.icpp"
