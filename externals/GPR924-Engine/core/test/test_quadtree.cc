//
// Created by benja on 06/11/2025.
//

#include <gtest/gtest.h>
#include <vector>

#include "physics/quadtree.h"

using core::AABB;
using core::QuadNode;

struct TestObject {
  AABB bounds;
  const AABB& GetBounds() const {
    return bounds;
  }
};

TEST(QuadNodeTest, InsertAndRetrieveNoSubdivision) {
  QuadNode<TestObject> tree(AABB{{0, 0}, {10, 10}}, 0);

  TestObject obj1{{{1, 1}, {2, 2}}};
  TestObject obj2{{{3, 3}, {4, 4}}};

  tree.Insert(obj1);
  tree.Insert(obj2);

  std::vector<std::pair<const TestObject*, const TestObject*>> collisions;

  tree.RetrieveCollisions(collisions);

  ASSERT_EQ(collisions.size(), 1);
  EXPECT_EQ(collisions[0].first->bounds.lowerBound.x, 1);
  EXPECT_EQ(collisions[0].second->bounds.lowerBound.x, 3);
}

TEST(QuadNodeTest, SubdivisionTriggered) {
  QuadNode<TestObject> tree(AABB{{0, 0}, {16, 16}}, 0);

  for (int i = 0; i < 5; ++i) {
    TestObject obj{{{float(i * 2), float(i * 2)}, {float(i * 2 + 1), float(i * 2 + 1)}}};
    tree.Insert(obj);
  }

  std::vector<std::pair<const TestObject*, const TestObject*>> collisions;
  tree.RetrieveCollisions(collisions);

  ASSERT_FALSE(collisions.empty());
}

TEST(QuadNodeTest, ClearEmptiesTree) {
  QuadNode<TestObject> tree(AABB{{0, 0}, {10, 10}}, 0);

  TestObject obj{{{1, 1}, {2, 2}}};
  tree.Insert(obj);

  tree.Clear();

  std::vector<std::pair<const TestObject*, const TestObject*>> collisions;
  tree.RetrieveCollisions(collisions);

  EXPECT_TRUE(collisions.empty());
}


