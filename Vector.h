/**
 * @file Vector.h
 * @author Jackson Brenneman
 * @brief Resizable array class template
 * @date 2023-11-5
 *
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iostream>
#include <stdexcept>

using std::ostream;
using std::endl;
using std::out_of_range;

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *          Vector Class Declaration
 *
 *   Resizeable array similar to std::vector
 *
 * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
class Vector
{
    template <typename val_type, typename ptr_type = val_type*>
    class Iterator;

    T *_data = nullptr;
    size_t currentSize = 0;
    size_t currentCapacity = 0;

public:
    typedef Iterator<T> iterator;
    typedef Iterator<const T> const_iterator;

    Vector() = default;
    Vector(const size_t, const T& = T());
    Vector(const Vector&);
    Vector(Vector&&);
    ~Vector() { if(_data) delete[] _data; }

    Vector& operator=(const Vector&);
    Vector& operator=(Vector&&);

          T& at(const size_t);
    const T& at(const size_t) const;

          T& operator[](const size_t ndx) { return _data[ndx]; }
    const T& operator[](const size_t ndx) const { return _data[ndx]; }

          T& front() { return _data[0]; }
    const T& front() const { return _data[0]; }
          T& back() { return _data[currentSize-1]; }
    const T& back() const { return _data[currentSize-1]; }

          T* data() noexcept { return _data; }
    const T* data() const noexcept { return _data; }

          iterator begin() noexcept{ return iterator(_data); }
          iterator end() noexcept { return iterator(_data + currentSize); }
    const_iterator begin() const noexcept { return const_iterator(_data); }
    const_iterator end() const noexcept { return const_iterator(_data + currentSize); }
    const_iterator cbegin() const noexcept { return const_iterator(_data); }
    const_iterator cend() const noexcept { return const_iterator(_data + currentSize); }

    bool empty() const noexcept { return currentSize == 0; }
    size_t size() const noexcept { return currentSize; }
    void reserve(size_t cap) { if(cap > currentCapacity) increaseCapacity(cap); }
    size_t capacity() const noexcept { return currentCapacity; }
    void shrink_to_fit();

    void clear() noexcept { currentSize = 0; }
    iterator insert(const_iterator, const T&);
    iterator insert(const_iterator, T&&);
    iterator erase(const_iterator);
    void push_back(const T&);
    void push_back(T&&);
    void pop_back();
    void resize(size_t, const T& = T());

    bool operator==(const Vector& rhs) const noexcept
        { return _data == rhs._data;}
    bool operator!=(const Vector& rhs) const noexcept
        { return !(*this == rhs); }

private:
    void increaseCapacity(size_t);
    iterator make_iterator(T *ptr) { return iterator(ptr); }

    template <typename val_type, typename ptr_type>
    class Iterator
    {
    public:
        Iterator() { current = NULL; }
        Iterator(ptr_type ptr) { current = ptr;}

        val_type& operator*() { return *current; }
        val_type* operator->() { return current; }
        const val_type& operator*() const { return *current; }
        const val_type* operator->() const { return current; }

        Iterator& operator++() { current++; return *this; }
        Iterator& operator--() { current--; return *this; }
        Iterator operator++(int) { Iterator temp(current); current++; return temp; }
        Iterator operator--(int) { Iterator temp(current); current--; return temp; }

        Iterator operator+(size_t n) { Iterator temp(current + n); return temp; }
        Iterator operator-(size_t n) { Iterator temp(current - n); return temp; }

        bool operator==(const Iterator& rhs) const noexcept
            { return this->current == rhs.current; }
        bool operator!=(const Iterator& rhs) const noexcept
            { return !(*this == rhs); }

    private:
        ptr_type current;
    };

public:
    template <typename F>
    friend ostream& operator<<(ostream&, const Vector<F>&);
};




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*               Vector Class Definitions                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Constructor
template <typename T>
Vector<T>::Vector(const size_t size, const T& info) {
    currentSize = size;
    currentCapacity = size;

    if(currentCapacity == 0) _data = nullptr;
    else _data = new T[currentCapacity];

    for(size_t i = 0; i < size; i++)
        _data[i] = info;
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& other) {
    currentSize = other.size();
    currentCapacity = currentSize;

    if(currentCapacity == 0) _data = nullptr;
    else _data = new T[currentCapacity];

    for(size_t i = 0; i < currentSize; i++)
        _data[i] = other._data[i];
}

// Move constructor
template <typename T>
Vector<T>::Vector(Vector&& other) {
    currentSize = other.currentSize;
    currentCapacity = other.currentCapacity;
    _data = other._data;

    other.currentSize = 0;
    other.currentCapacity = 0;
    other._data = nullptr;
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    if(currentCapacity > 0)
        delete[] _data;

    currentSize = other.size();
    currentCapacity = currentSize;
    if(currentCapacity == 0) _data = nullptr;
    else _data = new T[currentCapacity];

    for(size_t i = 0; i < currentSize; i++){
        _data[i] = other._data[i];
    }

    return *this;
}

// Move assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) {
    if(currentCapacity > 0)
        delete[] _data;

    currentSize = other.currentSize;
    currentCapacity = other.currentCapacity;
    _data = other._data;

    other.currentSize = 0;
    other.currentCapacity = 0;
    other._data = nullptr;

    return *this;
}

// element access with bounds checking
template <typename T>
T& Vector<T>::at(const size_t index) {
    if(index > currentSize - 1)
        throw out_of_range("ERROR: invalid index in at method");

    return _data[index];
}

// const element access with bounds checking
template <typename T>
const T& Vector<T>::at(const size_t index) const {
    if(index > currentSize - 1)
        throw out_of_range("ERROR: invalid index in at method");

    return _data[index];
}

// Shrinks vector capacity to current size
template <typename T>
void Vector<T>::shrink_to_fit(){
    if(currentSize < currentCapacity) {
        currentCapacity = currentSize;
        T* temp = new T[currentCapacity];
        for(size_t i = 0; i < currentSize; i++)
            temp[i] = _data[i];
        if(_data != NULL)
            delete[] _data;
        _data = temp;
    }
}

// Inserts element at iterator position
template <typename T>
typename Vector<T>::iterator
Vector<T>::insert(const_iterator pos, const T& value) {
    if(currentSize == currentCapacity)
        increaseCapacity(2*currentCapacity);

    T *end = const_cast<T*>(&(*pos));
    end--;
    T *ptr = _data + currentSize - 1;
    while (ptr != end) {
        ptr[1] = ptr[0];
        ptr--;
    }
    end[1] = value;
    currentSize++;

    return make_iterator(++end);
}

// Inserts element at iterator position
template <typename T>
typename Vector<T>::iterator
Vector<T>::insert(const_iterator pos, T&& value) {
    if(currentSize == currentCapacity)
        increaseCapacity(2*currentCapacity);

    T *end = const_cast<T*>(&(*pos));
    end--;
    T *ptr = _data + currentSize - 1;
    while (ptr != end) {
        ptr[1] = ptr[0];
        ptr--;
    }
    end[1] = std::move(value);
    currentSize++;

    return make_iterator(++end);
}

// Erases element at iterator position
template <typename T>
typename Vector<T>::iterator
Vector<T>::erase(const_iterator pos) {
    T* ptr = const_cast<T*>(&(*pos));
    bool isLast = false;
    T* ret = nullptr;
    if(ptr != &back())
        ret = &ptr[0];
    else
        isLast = true;
    T* end = &back();
    while (ptr != end){
        ptr[0] = ptr[1];
        ptr++;
    }
    currentSize--;

    if(!isLast)
        return make_iterator(ret);
    else
        return this->end();
}

// Insert element at back of array
template <typename T>
void Vector<T>::push_back(const T& value) {
    if(currentSize >= currentCapacity)
        increaseCapacity(2*currentCapacity);

    _data[currentSize] = value;
    currentSize++;
}

// Insert element at back of array
template <typename T>
void Vector<T>::push_back(T&& value) {
    if(currentSize >= currentCapacity)
        increaseCapacity(2*currentCapacity);

    _data[currentSize] = std::move(value);
    currentSize++;
}

// Remove the last element
template <typename T>
void Vector<T>::pop_back() {
    if(!empty()) currentSize--;
}

// Sets new size, dynamically allocates new array if needed
template <typename T>
void Vector<T>::resize(size_t count, const T& value) {
    if(count > currentSize){
        if(count > currentCapacity)
            increaseCapacity(2*count);

        for(size_t i = currentSize; i < count; i++)
            _data[i] = value;
        currentSize = count;
    }
    else if(count < currentSize)
        currentSize = count;
}


// Increases vector capacity by creating a new array and copying data
template <typename T>
void Vector<T>::increaseCapacity(size_t cap) {
    if(cap == 0) cap++;

    if(cap > currentCapacity){
        currentCapacity = cap;
        T* temp = new T[currentCapacity];
        for(size_t i = 0; i < currentSize; i++)
            temp[i] = _data[i];
        if(_data != NULL)
            delete[] _data;
        _data = temp;
    }
}


template <typename F>
ostream& operator<<(ostream& os, const Vector<F>& rhs) {
    for(size_t i = 0; i < rhs.currentSize; i++){
        os << rhs._data[i] << " ";
    }

    return os;
}

#endif //_VECTOR_H_
