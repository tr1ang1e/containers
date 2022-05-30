#include <gtest/gtest.h>
#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include <utility>

extern "C"
{
#include "include/c/rb_tree.h"
}


/************************************************************************
 *                             HELPER    	                            *
 ************************************************************************/

#define LOG_TREE(counter, number)    \
  do                                 \
  {                                  \
    if (counter == number)           \
      printSerializedTree (#number); \
  } while (0)

struct TestStruct
{
  int mem0, mem1, mem2;

  bool operator== (const TestStruct& other) const
  {
    return mem0 == other.mem0 && mem1 == other.mem1 && mem2 == other.mem2;
  }
};

/************************************************************************
 *                             CLASS    	                            *
 ************************************************************************/

class RBTreeTestClass : public ::testing::Test
{
  std::queue<RBTNode*> nodesQueue;
  std::vector<std::pair<std::string, COLOR>> serializedTree;
  std::vector<int> depthValues;

  void Traversal ()
  {
    // breadth-first traversal

    if (!nodesQueue.empty ())
    {
      RBTNode* pCurr = nodesQueue.front ();
      nodesQueue.pop ();

      std::string key = pCurr->key;
      COLOR color = pCurr->color;
      serializedTree.push_back ({ key, color });

      if (!is_NIL_same (pCurr))
      {
        nodesQueue.push (pCurr->left);
        nodesQueue.push (pCurr->right);
      }

      Traversal ();
    }
  }

  void CountDepth (RBTNode* pNode, int depth)
  {
    // count BLACK depth

    if (pNode->color == BLACK)
    {
      ++depth;
    }

    if (is_NIL_same (pNode))
    {
      depthValues.push_back (depth);
    }
    else
    {
      CountDepth (pNode->left, depth);
      CountDepth (pNode->right, depth);
    }
  }

  std::vector<std::pair<std::string, COLOR>> getSerialize ()
  {
    // emplty containers
    nodesQueue = {};
    serializedTree = {};

    // serialize with recursive function
    nodesQueue.push (pRoot);
    Traversal ();

    return serializedTree;
  }

public:
  RBTNode* pRoot;

  void SetUp () override { pRoot = NULL; }

  void printSerializedTree (std::string filename)
  {
    // use following code to send arg into this function
    /*
     * const testing::TestInfo* const test_info =
     * testing::UnitTest::GetInstance()->current_test_info();
     * printSerializedTree(test_info->name());
     *
     * use macro LOG_TREE(node_number, condition) to print serialized tree
     * when node_number == condition
     */

    // print level by level from left to right

    std::vector<std::pair<std::string, COLOR>> serializedTree = getSerialize ();

    std::ofstream fout;
    fout.open (filename + ".txt");

    for (auto pair : serializedTree)
    {
      fout << ((pair.first == "") ? "=" : pair.first) << " : ";
      fout << ((pair.second == RED) ? "R" : "B") << "\n";
    }

    fout.close ();
  }

  bool isTreeBalanced ()
  {
    bool result = false;

    depthValues = {};

    CountDepth (pRoot, 0);

    if (depthValues.size ())
    {
      int expected = depthValues[0];

      result = true;

      for (const auto& depth : depthValues)
      {
        if (depth != expected)
        {
          result = false;
          break;
        }
      }
    }

    return result;
  }

  ~RBTreeTestClass () override
  {
    if (pRoot != NULL)
    {
      if (pRoot->data != NULL)
      {
        free (pRoot->data);
      }

      free (pRoot);
    }
  }
};

/************************************************************************
 *                             TESTS    	                            *
 ************************************************************************/

TEST_F (RBTreeTestClass, RBTRegularTest)
{
  // rbt_insert
  // rbt_get

  bool result;
  const int keyMaxSize = 10;
  const int RBT_NODES_COUNT = 10000;
  int timeComplexity = 0;

  // regular insertion
  for (auto i = 0; i < RBT_NODES_COUNT; ++i)
  {
    TestStruct item = { i, i + 1, i + 2 };

    char key[keyMaxSize] = { 0 };
    snprintf (key, keyMaxSize, "%i", i);

    result = rbt_insert (&pRoot, &item, sizeof (TestStruct), key);
    ASSERT_TRUE (result);
    EXPECT_EQ (pRoot->color, BLACK);    // check if root is always black

    // full tree depth-first traversal : turn off to evaluate performance
    ASSERT_TRUE (isTreeBalanced ()) << "node number : " << i;
  }

  // insertion when key already exists in tree
  for (auto i = 0; i < 100; ++i)
  {
    TestStruct item = { 0 };

    char key[keyMaxSize] = { 0 };
    snprintf (key, keyMaxSize, "%i", i);

    result = rbt_insert (&pRoot, &item, sizeof (TestStruct), key);
    ASSERT_FALSE (result);
  }

  // regular getting
  for (auto i = 0; i < RBT_NODES_COUNT; ++i)
  {
    TestStruct expected = { i, i + 1, i + 2 };
    TestStruct actual = { 0 };

    char key[keyMaxSize] = { 0 };
    snprintf (key, keyMaxSize, "%i", i);

    result = rbt_get (pRoot, &actual, sizeof (actual), key);
    EXPECT_TRUE (result);
    EXPECT_TRUE (expected == actual);
  }

  // getting when key doesn't exist in tree
  for (auto i = RBT_NODES_COUNT; i < RBT_NODES_COUNT + 100; ++i)
  {
    TestStruct actual = { 0 };

    char key[keyMaxSize] = { 0 };
    snprintf (key, keyMaxSize, "%i", i);

    result = rbt_get (pRoot, &actual, sizeof (actual), key);
    EXPECT_FALSE (result);
  }
}

TEST_F (RBTreeTestClass, RBTDeletionTest)
{
  // rbt_delete
  // rbt_destroy

  ASSERT_TRUE (true);
}