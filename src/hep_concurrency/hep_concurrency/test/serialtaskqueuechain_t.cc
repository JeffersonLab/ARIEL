// vim: set sw=2 expandtab :
//
//  SerialTaskQueue_test.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/27/11.
//

#include "hep_concurrency/SerialTaskQueueChain.h"
#include "tbb/task.h"

#include <atomic>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include <unistd.h>

using namespace std;

void
testPush()
{
  atomic<unsigned int> count{0};
  vector<shared_ptr<hep::concurrency::SerialTaskQueue>> queues = {
    make_shared<hep::concurrency::SerialTaskQueue>(),
    make_shared<hep::concurrency::SerialTaskQueue>()};
  hep::concurrency::SerialTaskQueueChain chain(queues);
  {
    shared_ptr<tbb::task> waitTask{
      new (tbb::task::allocate_root()) tbb::empty_task{},
      [](tbb::task* iTask) { tbb::task::destroy(*iTask); }};
    waitTask->set_ref_count(1 + 3);
    tbb::task* pWaitTask = waitTask.get();
    chain.push([&count, pWaitTask] {
      if (count++ != 0) {
        throw logic_error("testPush: count != 0");
      }
      usleep(10);
      pWaitTask->decrement_ref_count();
    });
    chain.push([&count, pWaitTask] {
      if (count++ != 1) {
        throw logic_error("testPush: count != 1");
      }
      usleep(10);
      pWaitTask->decrement_ref_count();
    });
    chain.push([&count, pWaitTask] {
      if (count++ != 2) {
        throw logic_error("testPush: count != 2");
      }
      usleep(10);
      pWaitTask->decrement_ref_count();
    });
    waitTask->wait_for_all();
    if (count != 3) {
      throw logic_error("testPush: count != 3");
    }
    while (chain.outstandingTasks() != 0) {
    }
  }
}

void
testPushOne()
{
  atomic<unsigned int> count{0};
  vector<shared_ptr<hep::concurrency::SerialTaskQueue>> queues = {
    make_shared<hep::concurrency::SerialTaskQueue>()};
  hep::concurrency::SerialTaskQueueChain chain(queues);
  {
    shared_ptr<tbb::task> waitTask{
      new (tbb::task::allocate_root()) tbb::empty_task{},
      [](tbb::task* iTask) { tbb::task::destroy(*iTask); }};
    waitTask->set_ref_count(1 + 3);
    tbb::task* pWaitTask = waitTask.get();
    chain.push([&count, pWaitTask] {
      if (count++ != 0) {
        throw logic_error("testPushOne: count != 0");
      }
      usleep(10);
      pWaitTask->decrement_ref_count();
    });
    chain.push([&count, pWaitTask] {
      if (count++ != 1) {
        throw logic_error("testPushOne: count != 1");
      }
      usleep(10);
      pWaitTask->decrement_ref_count();
    });
    chain.push([&count, pWaitTask] {
      if (count++ != 2) {
        throw logic_error("testPushOne: count != 2");
      }
      usleep(10);
      pWaitTask->decrement_ref_count();
    });
    waitTask->wait_for_all();
    if (count != 3) {
      throw logic_error("testPushOne: count != 3");
    }
    while (chain.outstandingTasks() != 0) {
    }
  }
}

void
testPushAndWait()
{
  atomic<unsigned int> count{0};
  vector<shared_ptr<hep::concurrency::SerialTaskQueue>> queues = {
    make_shared<hep::concurrency::SerialTaskQueue>(),
    make_shared<hep::concurrency::SerialTaskQueue>()};
  hep::concurrency::SerialTaskQueueChain chain(queues);
  {
    chain.push([&count] {
      if (count++ != 0) {
        throw logic_error("testPushAndWait: count != 0");
      }
      usleep(10);
    });
    chain.push([&count] {
      if (count++ != 1) {
        throw logic_error("testPushAndWait: count != 1");
      }
      usleep(10);
    });
    chain.pushAndWait([&count] {
      if (count++ != 2) {
        throw logic_error("testPushAndWait: count != 2");
      }
      usleep(10);
    });
    if (count != 3) {
      throw logic_error("testPushAndWait: count != 3");
    }
  }
  while (chain.outstandingTasks() != 0) {
  }
}

void
testPushAndWaitOne()
{
  atomic<unsigned int> count{0};
  vector<shared_ptr<hep::concurrency::SerialTaskQueue>> queues = {
    make_shared<hep::concurrency::SerialTaskQueue>()};
  hep::concurrency::SerialTaskQueueChain chain(queues);
  {
    chain.push([&count] {
      if (count++ != 0) {
        throw logic_error("testPushAndWaitOne: count != 0");
      }
      usleep(10);
    });
    chain.push([&count] {
      if (count++ != 1) {
        throw logic_error("testPushAndWaitOne: count != 1");
      }
      usleep(10);
    });
    chain.pushAndWait([&count] {
      if (count++ != 2) {
        throw logic_error("testPushAndWaitOne: count != 2");
      }
      usleep(10);
    });
    if (count != 3) {
      throw logic_error("testPushAndWaitOne: count != 3");
    }
  }
  while (chain.outstandingTasks() != 0)
    ;
}

template <typename F>
class StressTask : public tbb::task {

public:
  StressTask(F f) : f_(f) {}

  tbb::task*
  execute()
  {
    f_();
    return nullptr;
  }

private:
  F f_;
};

template <typename A, typename F>
tbb::task*
make_stresstask(A&& a, F f)
{
  return new (a) StressTask<F>(f);
}

void
stressTest()
{
  // cerr << "Begin stressTest ..." << endl;
  auto q1 = make_shared<hep::concurrency::SerialTaskQueue>();
  auto q2 = make_shared<hep::concurrency::SerialTaskQueue>();
  vector<shared_ptr<hep::concurrency::SerialTaskQueue>> queues = {q1, q2};
  hep::concurrency::SerialTaskQueueChain chain(queues);
  atomic<bool> waitToStart{true};
  atomic<unsigned> count_stress_tasks{0U};
  atomic<unsigned> count_stress{0U};
  unsigned const nTasks = 1'000U;
  unsigned const expectedCount = 5 * nTasks;
  // Note: This is 1,000 us == 1 ms
  unsigned const countingTick = 1'000U;
  // Note: This is 30,000 ticks == 30 s
  unsigned const maxCountingTicks = 30'000U;
  auto stressFunctor = [nTasks,
                        &chain,
                        &waitToStart,
                        countingTick,
                        maxCountingTicks,
                        &count_stress_tasks,
                        &count_stress]() {
    ++count_stress_tasks;
    {
      auto waitCount = 0U;
      while (waitToStart.load()) {
        usleep(countingTick);
        ++waitCount;
        if (waitCount > maxCountingTicks) {
          // We will not wait forever.
          throw logic_error(
            "Max counting wait exceeded! waitToStart never became false!");
        }
      }
    }
    for (unsigned i = 0U; i < nTasks; ++i) {
      chain.push([&count_stress] { ++count_stress; });
    }
  };
  for (unsigned index = 10U; index != 0U; --index) {
    waitToStart.store(true);
    count_stress_tasks.store(0U);
    count_stress.store(0U);
    // cerr << "Creating the_thread ..." << endl;
    thread the_thread(
      [nTasks, &chain, &waitToStart, &count_stress_tasks, &count_stress]() {
        ++count_stress_tasks;
        {
          auto waitCount = 0U;
          while (waitToStart.load()) {
            usleep(countingTick);
            ++waitCount;
            if (waitCount > maxCountingTicks) {
              // We will not wait forever.
              throw logic_error(
                "Max counting wait exceeded! waitToStart never became false!");
            }
          }
        }
        for (unsigned i = 0U; i < nTasks; ++i) {
          chain.push([&count_stress] { ++count_stress; });
        }
      });
    auto stresstask1 =
      make_stresstask(tbb::task::allocate_root(), stressFunctor);
    tbb::task::enqueue(*stresstask1);
    auto stresstask2 =
      make_stresstask(tbb::task::allocate_root(), stressFunctor);
    tbb::task::enqueue(*stresstask2);
    auto stresstask3 =
      make_stresstask(tbb::task::allocate_root(), stressFunctor);
    tbb::task::enqueue(*stresstask3);
    // cerr << "Waiting for stresstasks to start ..." << endl;
    {
      auto waitCount = 0U;
      while (count_stress_tasks.load() != 4) {
        usleep(countingTick);
        ++waitCount;
        if (waitCount > maxCountingTicks) {
          // We will not wait forever.
          throw logic_error(
            "Max counting wait exceeded! count_stress_tasks never reached 4!");
        }
      }
    }
    // cerr << "Allowing stresstasks and the thread to proceed ..." << endl;
    waitToStart.store(false);
    // cerr << "Creating tasks on chain ..." << endl;
    for (unsigned i = 0U; i < nTasks; ++i) {
      chain.push([&count_stress] { ++count_stress; });
    }
    // cerr << "Waiting for counting tasks to finish ..." << endl;
    auto waitCount = 0U;
    while (count_stress != expectedCount) {
      // Waiting for all counting to finish;
      usleep(countingTick);
      ++waitCount;
      if (waitCount > maxCountingTicks) {
        // We will not wait forever.
        throw logic_error("Max counting wait exceeded! count_stress never "
                          "reached expectedCount!");
      }
    }
    if (chain.outstandingTasks() != 0) {
      // Tasks have finished counting, but chain not quite done yet.
      usleep(1000);
    }
    if (chain.outstandingTasks() != 0) {
      ostringstream buf;
      buf << "stressTest: chain.outstandingTasks(): "
          << chain.outstandingTasks() << " != 0";
      throw logic_error(buf.str());
    };
    // cerr << "count_stress: " << count_stress << endl;
    if (count_stress != expectedCount) {
      ostringstream buf;
      buf << "stressTest: count_stress: " << count_stress
          << " != expectedCount: " << expectedCount;
      throw logic_error(buf.str());
    }
    // cerr << "Waiting for the_thread ..." << endl;
    the_thread.join();
  }
  // cerr << "End   stressTest ..." << endl;
}

int
main()
{
  testPush();
  testPushOne();
  testPushAndWait();
  testPushAndWaitOne();
  stressTest();
}
