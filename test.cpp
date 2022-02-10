#include <gtest/gtest.h>
#include <iostream>

extern "C"
{
#include "include/c/queue.h"
}

class queue_tests : public ::testing::Test
{
public:
protected:
  queue_tests () {}

  void SetUp () override {}

  void TearDown () override {}

  ~queue_tests () {}
};

class rb_tree_tests : public ::testing::Test
{
public:
protected:
  rb_tree_tests () {}

  void SetUp () override {}

  void TearDown () override {}

  ~rb_tree_tests () {}
};

int main (int argc, char* argv[])
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}