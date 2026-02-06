#ifndef CORE_LINEAR_ALLOCATOR_H
#define CORE_LINEAR_ALLOCATOR_H

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

namespace core {
class LinearAllocator {
 public:
  explicit LinearAllocator(const size_t size_bytes) {
    start_ = operator new(size_bytes);
    current_ = start_;
  }

  ~LinearAllocator() { operator delete(start_); }

  void* Allocate(const size_t size) {
    void* ptr = current_;
    current_ = static_cast<char*>(current_) + size;
    return ptr;
  }

  void Clear() { current_ = start_; }

 private:
  void* start_;
  void* current_;
};
}  // namespace core

#endif  // CORE_LINEAR_ALLOCATOR_H
