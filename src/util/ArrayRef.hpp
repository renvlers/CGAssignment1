#pragma once

#ifndef _ARRAY_REF_HPP_
#define _ARRAY_REF_HPP_

#include <cstddef>
#include <type_traits>

template <typename T> class ArrayRef {
private:
    T* const pArray = nullptr;
    size_t count = 0;

    ArrayRef operator=(const ArrayRef& other) = delete;
    ArrayRef& operator=(ArrayRef&& other) = delete;

public:
    ArrayRef();
    ArrayRef(const T& data);
    template <size_t elementCount> ArrayRef(const T (&data)[elementCount]);
    ArrayRef(const T* pData, size_t count);
    ArrayRef(const ArrayRef<std::remove_const_t<T>>& other);

    T* pointer() const;
    size_t size() const;

    T& operator[](size_t index) const;
    T* begin() const;
    T* end() const;
};

template <typename T> ArrayRef<T>::ArrayRef() = default;
template <typename T> ArrayRef<T>::ArrayRef(const T& data) : pArray(const_cast<T*>(&data)), count(1) {}
template <typename T> template <size_t elementCount> ArrayRef<T>::ArrayRef(const T (&data)[elementCount]) : pArray(const_cast<T*>(data)), count(elementCount) {}
template <typename T> ArrayRef<T>::ArrayRef(const T* pData, size_t count) : pArray(const_cast<T*>(pData)), count(count) {}
template <typename T> ArrayRef<T>::ArrayRef(const ArrayRef<std::remove_const_t<T>>& other) : pArray(const_cast<T*>(other.pointer())), count(other.size()) {}

template <typename T> T* ArrayRef<T>::pointer() const { return pArray; }
template <typename T> size_t ArrayRef<T>::size() const { return count; }

template <typename T> T& ArrayRef<T>::operator[](size_t index) const { return pArray[index]; }
template <typename T> T* ArrayRef<T>::begin() const { return pArray; }
template <typename T> T* ArrayRef<T>::end() const { return pArray + count; }

#endif // _ARRAY_REF_HPP_