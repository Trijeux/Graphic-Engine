#ifndef GPR924_ENGINE_OBSERVER_H
#define GPR924_ENGINE_OBSERVER_H

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

#include <span>
#include <stdexcept>
#include <vector>

namespace common {
template <typename T>
class ObserverSubject {
 public:
  static void AddObserver(T* observer) {
    auto it = std::ranges::find(observers_, nullptr);
    if (it == observers_.end()) {
      observers_.push_back(observer);
    } else {
      *it = observer;
    }
  }
  static void RemoveObserver(T* observer) {
    auto it = std::ranges::find(observers_, observer);
    if (it != observers_.end()) {
      *it = nullptr;
    } else {
      throw std::runtime_error("Observer does not exist");
    }
  }
  static std::span<T*> GetObservers() { return observers_; }

 private:
  inline static std::vector<T*> observers_;
};
}  // namespace common
#endif  // GPR924_ENGINE_OBSERVER_H
