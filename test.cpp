#include <gtest/gtest.h>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

extern "C"
{

#include "include/c/queue.h"
}

class containers_tests : public ::testing::Test
{
protected:
  const int TEST_ITEMS_COUNT = 1000;

  struct TestStruct
  {
    int mem0, mem1, mem2;

    bool operator== (const TestStruct& other) const
    {
      return mem0 == other.mem0 && mem1 == other.mem1 && mem2 == other.mem2;
    }
  };
};

TEST_F (containers_tests, queue_base_test)
{
  Queue queue = { 0 };

  auto result = queue_init (&queue);
  EXPECT_TRUE (result);

  TestStruct item = { 0, 1, 2 };

  // pop from empty queue
  result = queue_pop (&queue, (void*)&item, sizeof (TestStruct));
  EXPECT_FALSE (result);

  // push to empty queue
  result = queue_push (&queue, &item, sizeof (TestStruct));
  EXPECT_TRUE (result);
  EXPECT_EQ ((void*)queue.head, (void*)queue.tail);

  // pop from queue with one item
  TestStruct actual = { 0 };
  result = queue_pop (&queue, &actual, sizeof (TestStruct));

  EXPECT_TRUE (item == actual);
  EXPECT_TRUE (queue.head == NULL);
  EXPECT_TRUE (queue.tail == NULL);
}

TEST_F (containers_tests, queue_regular_test)
{
  Queue queue = { 0 };

  auto result = queue_init (&queue);
  EXPECT_TRUE (result);

  for (auto i = 0; i < TEST_ITEMS_COUNT; ++i)
  {
    TestStruct Item = { i, i + 1, i + 2 };
    result = queue_push (&queue, &Item, sizeof (TestStruct));
    EXPECT_TRUE (result);
  }

  for (auto i = 0; i < TEST_ITEMS_COUNT; ++i)
  {
    TestStruct expectedItem = { i, i + 1, i + 2 };
    TestStruct actualItem = { 0 };

    result = queue_pop (&queue, &actualItem, sizeof (TestStruct));
    EXPECT_TRUE (expectedItem == actualItem);
  }

  EXPECT_TRUE (queue.head == NULL);
  EXPECT_TRUE (queue.tail == NULL);
}

TEST_F (containers_tests, concurrent_queue_regular_test)
{
  QueueSafe queue = { 0 };

  auto result = concurrent_queue_init (&queue);
  EXPECT_TRUE (result);

  for (auto i = 0; i < TEST_ITEMS_COUNT; ++i)
  {
    TestStruct Item = { i, i + 1, i + 2 };
    result = concurrent_queue_push (&queue, &Item, sizeof (TestStruct));
    EXPECT_TRUE (result);
  }

  for (auto i = 0; i < TEST_ITEMS_COUNT; ++i)
  {
    TestStruct expectedItem = { i, i + 1, i + 2 };
    TestStruct actualItem = { 0 };

    result = concurrent_queue_pop (&queue, &actualItem, sizeof (TestStruct), 0);
    EXPECT_TRUE (expectedItem == actualItem);
  }

  EXPECT_TRUE (queue.queue.head == NULL);
  EXPECT_TRUE (queue.queue.tail == NULL);
}

TEST_F (containers_tests, concurrent_queue_destroy_test)
{
  QueueSafe queue = { 0 };

  auto result = concurrent_queue_init (&queue);
  EXPECT_TRUE (result);

  for (auto i = 0; i < TEST_ITEMS_COUNT; ++i)
  {
    TestStruct Item = { 0 };
    (void)concurrent_queue_push (&queue, &Item, sizeof (TestStruct));
  }

  concurrent_queue_destroy (&queue);

  EXPECT_TRUE (queue.queue.head == NULL);
  EXPECT_TRUE (queue.queue.tail == NULL);
}

TEST_F (containers_tests, concurrent_queue_prod_cons_test)
{
  const size_t PRODUCERS_COUNT = 3;
  const size_t CONSUMERS_COUNT = 3;
  std::list<TestStruct> receivedItems;
  std::mutex receivedItemsMutex;
  unsigned int waitAsyncTimeoutMs = 100;

  QueueSafe queue = { 0 };

  auto result = concurrent_queue_init (&queue);
  EXPECT_TRUE (result);

  std::list<std::thread> producers;
  for (auto id = 0; id < PRODUCERS_COUNT; id++)
  {
    producers.push_back (std::thread ([&, id] {
      for (auto i = 0; i < TEST_ITEMS_COUNT; ++i)
      {
        TestStruct item = { id };
        concurrent_queue_push (&queue, &item, sizeof (TestStruct));
      }
    }));
  }

  std::list<std::thread> consumers;
  for (auto id = 0; id < CONSUMERS_COUNT; id++)
  {
    consumers.push_back (std::thread ([&] {
      while (true)
      {
        TestStruct item = { 0 };
        auto result = concurrent_queue_pop (&queue, &item, sizeof (TestStruct), waitAsyncTimeoutMs);

        std::lock_guard guard (receivedItemsMutex);

        if (result)
        {
          receivedItems.push_back (item);
        }

        if (receivedItems.size () == PRODUCERS_COUNT * TEST_ITEMS_COUNT)
        {
          break;
        }
      }
    }));
  }

  for (auto& item : producers)
  {
    if (item.joinable ())
    {
      item.join ();
    }
  }

  for (auto& item : consumers)
  {
    if (item.joinable ())
    {
      item.join ();
    }
  }

  auto itemsSentByThread = [&] (size_t id) {
    size_t count = 0;
    for (const auto& item : receivedItems)
    {
      if (item.mem0 == id)
      {
        count++;
      }
    }

    return count;
  };

  for (auto id = 0; id < PRODUCERS_COUNT; id++)
  {
    auto itemsCount = itemsSentByThread (id);
    EXPECT_TRUE (itemsCount == TEST_ITEMS_COUNT);
  }

  EXPECT_TRUE (queue.queue.tail == NULL);
  EXPECT_TRUE (queue.queue.head == NULL);
}

int main (int argc, char* argv[])
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}