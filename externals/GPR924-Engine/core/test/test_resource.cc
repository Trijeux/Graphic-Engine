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

Contributors: Elias Farhan
*/

#include "resource/resource.h"
#include <gtest/gtest.h>

class SimpleObject final {
  public:
  ~SimpleObject() {
    value_ = 0;
  }

  void set_value(int new_value) {
    value_ = new_value;
  }

  [[nodiscard]] const auto& value() const{return value_;}
private:
  int value_ = 0;
};

class SimpleObjectDestructor {
  public:
  explicit SimpleObjectDestructor(SimpleObject* objectPtr): objectPtr_(objectPtr) {

  }
  void operator()() const {
    //always check nullptr as destructor can be called when object is null
    if (objectPtr_ != nullptr) {
      objectPtr_->set_value(-1);
    }
  }
private:
  SimpleObject* objectPtr_ = nullptr;
};

TEST(Resource, RAII) {
  SimpleObject simple_object;
  simple_object.set_value(1);

  ASSERT_EQ(simple_object.value(), 1);
  {
    core::Resource<SimpleObject*, SimpleObjectDestructor> resource(&simple_object);
    //destructor is called, so value is -1 now
  }
  ASSERT_EQ(simple_object.value(), -1);
}

TEST(Resource, MoveSemantics) {
  SimpleObject simple_object;
  simple_object.set_value(1);

  ASSERT_EQ(simple_object.value(), 1);
  {
    core::Resource<SimpleObject*, SimpleObjectDestructor> resource_parent;
    {
      core::Resource<SimpleObject*, SimpleObjectDestructor> resource(&simple_object);
      resource_parent = std::move(resource);
      ASSERT_EQ(resource.get() , nullptr); //resource was moved into the parent
    }
    ASSERT_EQ(resource_parent->value(), 1);
    ASSERT_EQ(simple_object.value(), 1);
    //destructor of parent is called, so value is -1 now
  }
  ASSERT_EQ(simple_object.value(), -1);
}
