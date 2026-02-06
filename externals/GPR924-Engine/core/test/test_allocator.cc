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

Contributors: Jemoel Ablay
*/

#include <gtest/gtest.h>
#include "allocators/linear_allocator.h"

TEST(LinearAllocator, SingleAllocation) {
  core::LinearAllocator allocator(1024);

  void* ptr1 = allocator.Allocate(128);
  ASSERT_NE(ptr1, nullptr);

  void* ptr2 = allocator.Allocate(256);
  ASSERT_NE(ptr2, nullptr);
  ASSERT_NE(ptr1, ptr2);
}

TEST(LinearAllocator, MultipleAllocations)
{
  core::LinearAllocator allocator(1024);

  constexpr int num = 5;
  void* ptrs[num];

  for (int i = 0; i < num; i++)
  {
    ptrs[i] = allocator.Allocate(100);
    ASSERT_NE(ptrs[i], nullptr);
    if (i > 0)
      ASSERT_GT(reinterpret_cast<uintptr_t>(ptrs[i]), reinterpret_cast<uintptr_t>(ptrs[i-1]));
  }
}

TEST(LinearAllocator, ClearAllowsReuse)
{
  core::LinearAllocator allocator(1024);

  void* first = allocator.Allocate(200);
  allocator.Allocate(300);

  allocator.Clear();

  void* third = allocator.Allocate(200);
  ASSERT_EQ(third, first);
}

TEST(LinearAllocator, AllocateExactSize)
{
  constexpr size_t total_size = 512;
  core::LinearAllocator allocator(total_size);

  void* ptr1 = allocator.Allocate(256);
  void* ptr2 = allocator.Allocate(256);

  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);
}