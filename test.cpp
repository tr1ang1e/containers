#include <gtest/gtest.h>
#include <iostream>

extern "C"
{
#include "include/c/queue.h"
}

class queue_container_tests : public ::testing::Test
{
public:
  Queue* pQueue;

protected:
  queue_container_tests () {}

  void
  SetUp () override
  {
    pQueue = queue_init ();
  }

  void
  TearDown () override
  {
  }

  ~queue_container_tests () {}
};

TEST_F (queue_container_tests, special_cases)
{
  // pop empty queue behavior
  Node* pNode = pop (pQueue);
  ASSERT_EQ (pNode, nullptr);

  // pop queue with one node behavior
  Node* pNode1 = create_node (1);
  pQueue = push (pQueue, pNode1);
  Node* pNode2 = pop (pQueue);
  ASSERT_EQ (pNode1, pNode2);
  ASSERT_EQ (pQueue->head, nullptr);
  ASSERT_EQ (pQueue->tail, nullptr);
}

TEST_F (queue_container_tests, basic_cases)
{
  int counter = 10;

  for (int i = 0; i < counter; ++i)
    {
      Node* pNode = create_node (i);
      pQueue = push (pQueue, pNode);
    }

  for (int i = counter - 1; i >= 0; --i)
    {
      Node* pNode = pop (pQueue);
      ASSERT_NE (pNode->value, i);
    }
}

int
main (int argc, char* argv[])
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}