#ifndef QUADTREE_H
#define QUADTREE_H
#include <memory>
#include <vector>

#include "maths/vec2.h"
/*
Copyright 2025 SAE Institute Switzerland SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Contributors: Benjamin Layson
*/

namespace core {


struct AABB {
  Vec2F lowerBound;
  Vec2F upperBound;

  [[nodiscard]] bool intersects(const AABB& other) const {return !(upperBound.x < other.lowerBound.x || lowerBound.x > other.upperBound.x ||
             upperBound.y < other.lowerBound.y || lowerBound.y > other.upperBound.y);
  }
  [[nodiscard]] bool contains(const AABB& other) const {return (lowerBound.x <= other.lowerBound.x && upperBound.x >= other.upperBound.x &&
            lowerBound.y <= other.lowerBound.y && upperBound.y >= other.upperBound.y);
  }
};

template<typename T>
concept HasGetBounds = requires(T obj) {
  { obj.GetBounds() } -> std::convertible_to<AABB>;
};


template<HasGetBounds T>
class QuadNode {
  AABB bounds_;
  int level_;
  std::vector<T> objectsInside_;
  std::unique_ptr<QuadNode> children_[4];

 public:
  explicit QuadNode(const AABB& bounds, int level)
      : bounds_(bounds), level_(level) {
    for (auto& child : children_) {
      child = nullptr;
    }
  }

  void Insert(const T& object) {
    for (auto& i : children_) {
      if (i) {
        if (i->bounds_.contains(object.GetBounds())) {
          i->Insert(object);
          return;
        }
      }
    }
    objectsInside_.push_back(object);

    constexpr int MAX_OBJECTS = 4;
    if (objectsInside_.size() > MAX_OBJECTS && level_ < 5) {
      if (!children_[0]) {
        float midX = (bounds_.lowerBound.x + bounds_.upperBound.x) * 0.5f;
        float midY = (bounds_.lowerBound.y + bounds_.upperBound.y) * 0.5f;

        children_[0] = std::make_unique<QuadNode>(
            AABB{bounds_.lowerBound, Vec2F{midX, midY}}, level_ + 1);
        children_[1] =
            std::make_unique<QuadNode>(AABB{{midX, bounds_.lowerBound.y},
                                            Vec2F{bounds_.upperBound.x, midY}},
                                       level_ + 1);
        children_[2] =
            std::make_unique<QuadNode>(AABB{{bounds_.lowerBound.x, midY},
                                            Vec2F{midX, bounds_.upperBound.y}},
                                       level_ + 1);
        children_[3] = std::make_unique<QuadNode>(
            AABB{{midX, midY}, bounds_.upperBound}, level_ + 1);

        auto oldObjects = std::move(objectsInside_);
        objectsInside_.clear();

        for (const T& obj : oldObjects) {
          Insert(obj);
        }
      }
    }
  }

  void RetrieveCollisions(
      std::vector<std::pair<const T*, const T*>>& collisions) const {
    for (size_t i = 0; i < objectsInside_.size(); ++i) {
      for (size_t j = i + 1; j < objectsInside_.size(); ++j) {
        collisions.push_back({&objectsInside_[i], &objectsInside_[j]});
      }
    }

    for (auto& i : children_) {
      if (i) {
        i->RetrieveCollisions(collisions);
      }
    }
  }
  void Clear() {
    objectsInside_.clear();
    for (auto& i : children_) {
      if (i) {
        i->Clear();
        i.reset();
      }
    }
  }
};


template<HasGetBounds T>
class QuadTree {
public:

  explicit QuadTree(const AABB& bounds): root_(bounds, 0) {}
  void Insert(const T& object) {root_.Insert(object);}
  void RetrieveCollisions(std::vector<std::pair<const T*, const T*>>& collisions) const {
    root_.RetrieveCollisions(collisions);
  }
  void Clear() {root_.Clear();}

private:
  QuadNode<T> root_;
};


}

#endif //QUADTREE_H
