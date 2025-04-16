/*
  Copyright (C) 2024-2025  Selwin van Dijk

  This file is part of signalbackup-tools.

  signalbackup-tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-tools.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DEEPCOPYINGUNIQUEPTR_H_
#define DEEPCOPYINGUNIQUEPTR_H_

#include <memory>

template <typename T, typename U = std::default_delete<T>>
class DeepCopyingUniquePtr : public std::unique_ptr<T, U>
{
  // determine at compiletime whether T::clone() exists...
  struct HasCloneMethod
  {
    template <typename C, T *(C::*)() const> struct SFINAE {};
    template <typename C> static char Test(SFINAE<C, &C::clone> *);
    template <typename C> static int Test(...);
    static bool const value = sizeof(Test<T>(0)) == sizeof(char);
  };

  struct HasMoveCloneMethod
  {
    template <typename C, T *(C::*)()> struct SFINAE {};
    template <typename C> static char Test(SFINAE<C, &C::move_clone> *);
    template <typename C> static int Test(...);
    static bool const value = sizeof(Test<T>(0)) == sizeof(char);
  };

  static_assert(std::is_copy_constructible<T>::value, "CopyUniquePtr's contents must have copy constructor");

  // inherit constructors
  using std::unique_ptr<T, U>::unique_ptr;

 public:
  inline DeepCopyingUniquePtr(DeepCopyingUniquePtr<T, U> const &other)
    :
    std::unique_ptr<T, U>()
  {
    if constexpr (HasCloneMethod::value)
      this->reset(other ? other->clone() : nullptr);
    else
      this->reset(other ? new T(*(other.get())) : nullptr);
  }

  inline DeepCopyingUniquePtr<T, U> &operator=(DeepCopyingUniquePtr<T, U> const &other)
  {
    if (this != &other) [[likely]]
    {
      if constexpr (HasCloneMethod::value)
        this->reset(other ? other->clone() : nullptr);
      else
        this->reset(other ? new T(std::move(*(other.get()))) : nullptr);
    }
    return *this;
  }

  inline DeepCopyingUniquePtr(DeepCopyingUniquePtr<T, U> &&other) noexcept
    :
    std::unique_ptr<T, U>()
  {
    if constexpr (HasMoveCloneMethod::value)
      this->reset(other ? other->move_clone() : nullptr);
    else
      this->reset(other ? new T(std::move(*(other.get()))) : nullptr);
  }

  inline DeepCopyingUniquePtr<T, U> &operator=(DeepCopyingUniquePtr<T, U> &&other) noexcept
  {
    if (this != &other) [[likely]]
    {
      if constexpr (HasMoveCloneMethod::value)
        this->reset(other ? other->move_clone() : nullptr);
      else
        this->reset(other ? new T(std::move(*(other.get()))) : nullptr);
    }
    return *this;
  }
};

#endif
