/**
 * @file HashMap.h
 * @author Jackson Brenneman
 * @brief Hash table implementation
 * @date 2023-11-05
 *
 */


#ifndef _HASHMAP_H
#define _HASHMAP_H

#include <string>
#include <stdexcept>
#include <functional>
#include <utility>
#include <cmath>

#include "ForwardList.h"
#include "Vector.h"


/**
 * @brief Pair structure to group two objects (often key and value)
 *
 * @tparam T1 first object type
 * @tparam T2 second object type
 */
template <typename T1, typename T2>
struct Pair
{
    T1 first;
    T2 second;

    Pair(const T1& f = T1(), const T2& s = T2()) : first(f), second(s) {}
    template <typename U1, typename U2> Pair(U1&& f, U2&& s)
        : first(std::forward<U1>(f)), second(std::forward<U2>(s)) {}
    Pair(const Pair& other) = default;
    Pair(Pair&& p) = default;

    Pair& operator=(const Pair&) = default;
    Pair& operator=(Pair&&) = default;

    bool operator==(const Pair& rhs) const noexcept
        { return first == rhs.first; }
    bool operator!=(const Pair& rhs) const noexcept
        { return !(first == rhs.first); }

    template <typename F, typename G>
    friend ostream& operator<<(ostream&, const Pair<F,G>&);
};

template<typename F, typename G>
ostream& operator<<(ostream & os, const Pair<F,G> &p) {
    os << "(" << p.first << ", " << p.second << ") ";

    return os;
}


/* * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *      Unordered Map Class Declaration
 *
 * Uses hashing to store key, value pairs similar
 * std::unordered_map. Templated using key type,
 * value type, and a hash object. Stores pairs in
 * a vector of linked lists (chaining collision
 * resolution).
 *
 * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename Key, typename T, typename Hash = std::hash<Key>>
class UnorderedMap
{
    typedef Pair<const Key, T> pair;
    class map_iterator;
    class const_map_iterator;

    Vector<ForwardList<pair>> A;
    Hash h;
    size_t currentSize = 0;
    float _max_load_factor = 1.0;

public:
    typedef typename ForwardList<pair>::iterator local_iterator;
    typedef typename ForwardList<pair>::const_iterator const_local_iterator;

    typedef map_iterator iterator;
    typedef const_map_iterator const_iterator;


    UnorderedMap() : UnorderedMap(1) {}
    UnorderedMap(size_t n, const Hash& hs = Hash())
        : A(Vector<ForwardList<pair>>(nextPrime(n))),
          h(hs),
          currentSize(0),
          _max_load_factor(1.0) {}
    UnorderedMap(const UnorderedMap&) = default;
    UnorderedMap(UnorderedMap&&) = default;
    ~UnorderedMap() = default;

    UnorderedMap& operator=(const UnorderedMap&);
    UnorderedMap& operator=(UnorderedMap&&);

          iterator begin() noexcept { return make_iterator(0, A[0].begin()); }
    const_iterator cbegin() const noexcept
        { return make_const_iterator(0, A[0].cbegin()); }
          iterator end() noexcept { return make_iterator(bucket_count()); }
    const_iterator cend() const noexcept
        { return make_const_iterator(bucket_count()); }

    bool empty() const noexcept { return currentSize == 0; }
    size_t size() const noexcept { return currentSize; }

    void clear() noexcept { A.clear(); }
    Pair<iterator, bool> insert(const pair&);
    Pair<iterator, bool> insert(pair&&);
    //iterator erase(iterator);
    //iterator erase(const_iterator);
    //size_t erase(const Key&);

    //T& at(const Key&);
    //const T& at(const Key&) const;
    T& operator[](const Key&);
    iterator find(const Key&);
    const_iterator find(const Key&) const;
    size_t count(const Key& k) const { return (find(k) != cend()) ? 1 : 0; }

    local_iterator begin(size_t n) { return A[n].begin(); }
    const_local_iterator cbegin(size_t n) const { return A[n].cbegin(); }
    local_iterator end(size_t n) { return A[n].end(); }
    const_local_iterator cend(size_t n) const { return A[n].cend(); }
    size_t bucket_count() const { return A.size(); }
    size_t bucket_size(size_t n) const { return A[n].size(); }
    size_t bucket(const Key& k) const { return h(k) % bucket_count(); }

    float load_factor() const { return (float)currentSize / (float)bucket_count(); }
    float max_load_factor() const { return _max_load_factor; }
    void max_load_factor(float ml) { _max_load_factor = ml; }
    void rehash(size_t);
    void reserve(size_t n) { rehash(std::ceil(n / max_load_factor())); }

private:
    size_t nextPrime(size_t);

    iterator make_iterator(
        size_t ndx,
        local_iterator itr = local_iterator(NULL)
    ) {
        return iterator(A.begin() + ndx, itr, *this);
    }

    const_iterator make_const_iterator(
        size_t ndx,
        const_local_iterator itr = const_local_iterator(NULL)
    ) const {
        return const_iterator(A.cbegin() + ndx, itr, *this);
    }

    class map_iterator
    {
        typedef typename Vector<ForwardList<pair>>::iterator bucket_iterator;

    public:
        map_iterator() : bucket(nullptr), pos(nullptr) {}
        map_iterator(const map_iterator &it)
            : bucket(it.bucket), pos(it.pos), ref(it.ref) {}

        pair& operator*() { return *pos; }
        pair* operator->() { return &(*pos); }
        map_iterator& operator++();
        map_iterator operator++(int);
        bool operator==(const map_iterator& rhs) const noexcept
            { return pos == rhs.pos && bucket == rhs.bucket; }
        bool operator!=(const map_iterator& rhs) const noexcept
            { return !(*this == rhs); }

    private:
        bucket_iterator bucket;
        local_iterator pos;
        UnorderedMap &ref;

        map_iterator(const bucket_iterator &b, const local_iterator &p, UnorderedMap &map)
            : bucket(b), pos(p), ref(map) {}

        friend iterator UnorderedMap::make_iterator(size_t, local_iterator);
    }; // class map_iterator

    class const_map_iterator
    {
        typedef typename Vector<ForwardList<pair>>::const_iterator const_bucket_iterator;

    public:
        const_map_iterator() : bucket(nullptr), pos(nullptr) {}
        const_map_iterator(const map_iterator &it)
            : bucket(it.bucket), pos(it.pos), ref(it.ref) {}

        const pair& operator*() { return *pos; }
        const pair* operator->() { return &(*pos); }
        const_map_iterator& operator++();
        const_map_iterator operator++(int);
        bool operator==(const const_map_iterator& rhs) const noexcept
            { return pos == rhs.pos && bucket == rhs.bucket; }
        bool operator!=(const const_map_iterator& rhs) const noexcept
            { return !(*this == rhs); }

    private:
        const_bucket_iterator bucket;
        const_local_iterator pos;
        const UnorderedMap &ref;

        const_map_iterator(const_bucket_iterator b, const_local_iterator p, const UnorderedMap &map)
            : bucket(b), pos(p), ref(map) {}

        friend const_iterator UnorderedMap::make_const_iterator(size_t, const_local_iterator) const;
    }; // class const_map_iterator

public:
    template <typename F, typename G, typename I>
    friend ostream& operator<<(ostream&, const UnorderedMap<F,G,I>&);
}; // class unordered_map




/* * * * * * * * * * * * * * * *
 * Hash table implementation
 * * * * * * * * * * * * * * * */

template <typename F, typename G, typename I>
ostream& operator<<(ostream &os, const UnorderedMap<F,G,I>& rhs) {
    for(size_t i = 0; i < rhs.bucket_count(); i++){
        os << rhs.A[i] << endl;
    }
    return os;
}

// Copy assignment
template <typename Key, typename T, typename Hash>
UnorderedMap<Key,T,Hash>&
UnorderedMap<Key,T,Hash>::operator=(const UnorderedMap& other) {
    if(!empty())
        clear();

    h = other.h;
    A = other.A;
    currentSize = other.currentSize;
    _max_load_factor = other._max_load_factor;

    return *this;
}

// Move assignment
template <typename Key, typename T, typename Hash>
UnorderedMap<Key,T,Hash>&
UnorderedMap<Key,T,Hash>::operator=(UnorderedMap&& other) {
    if(!empty())
        clear();

    h = std::move(other.h);
    A = std::move(other.A);
    currentSize = other.currentSize;
    _max_load_factor = other._max_load_factor;

    return *this;
}

// Insert pair, return iterator to pair and bool if inserted
template <typename Key, typename T, typename H>
auto UnorderedMap<Key,T,H>::insert(const pair& p) -> Pair<iterator, bool> {
    bool ret = false;
    if(load_factor() >= max_load_factor())
        rehash(currentSize * 2);

    size_t index = h(p.first) % bucket_count(); //hash index
    auto itr = A[index].begin();
    if(count(p.first) > 0) {
        while(itr->first != p.first) {
            ++itr;
        }
    }
    else {
        A[index].push_front(p);
        currentSize++;
        ret = true;
    }
    auto retItr = make_iterator(index, itr);
    return Pair<iterator, bool>(retItr, ret);
}

// Move insert
template <typename Key, typename T, typename H>
auto UnorderedMap<Key,T,H>::insert(pair&& p) -> Pair<iterator, bool> {
    bool ret = false;
    if(load_factor() >= max_load_factor())
        rehash(currentSize * 2);

    size_t index = h(p.first) % bucket_count(); //hash index
    auto itr = A[index].begin();
    if(count(p.first) > 0) {
        while(itr->first != p.first) {
            ++itr;
        }
    }
    else {
        A[index].push_front(std::move(p));
        currentSize++;
        ret = true;
    }
    auto retItr = make_iterator(index, itr);
    return Pair<iterator, bool>(retItr, ret);
}

// Subscript operator
template <typename Key, typename T, typename H>
T& UnorderedMap<Key,T,H>::operator[](const Key& k) {
    if(load_factor() >= max_load_factor())
        rehash(currentSize * 2);
    size_t ndx = h(k) % bucket_count();
    bool found = false;
    if(A[ndx].empty()) {
        A[ndx].push_front(pair(k, T()));
        currentSize++;
        return A[ndx].front().second;
    }
    else {
        auto itr = begin(ndx);
        while(itr != end(ndx)){
            if(itr->first == k){
                found = true;
                break;
            }
            ++itr;
        }
        if(!found){
            A[ndx].push_front(pair(k, T()));
            currentSize++;
            return A[ndx].front().second;
        }
        else return itr->second;
    }
}

// Return iterator to key if found, else end()
template <typename Key, typename T, typename H>
auto UnorderedMap<Key,T,H>::find(const Key& k) -> iterator {
    size_t index = h(k) % bucket_count();
    auto itr = begin(index);
    do {
        if(itr->first == k)
            break;
        ++itr;
    } while(itr != end(index));

    return make_iterator(index, itr);
}

// Return const_iterator to key if found, else cend()
template <typename Key, typename T, typename H>
auto UnorderedMap<Key,T,H>::find(const Key& k) const -> const_iterator {
    size_t index = h(k) % bucket_count();
    auto itr = cbegin(index);
    while(itr != cend(index)) {
        if(itr->first == k)
            return make_const_iterator(index, itr);
        ++itr;
    }

    return cend();
}

// Rehash to new size n, n > currentSize / max_load_factor()
template <typename Key, typename T, typename H>
void UnorderedMap<Key,T,H>::rehash(size_t n) {
    if(n == 0) n = 1;
    while(n < currentSize / max_load_factor())
        n = (size_t)(currentSize / max_load_factor() * 2);

    Vector<ForwardList<pair>> temp(nextPrime(n));
    for(size_t i = 0; i < A.size(); i++){
        auto itr = A[i].begin();
        while(itr != A[i].end()){
            size_t index = h(itr->first) % temp.size();
            temp[index].push_front(std::move(*itr));
            ++itr;
        }
    }
    A = std::move(temp);
}

// Return next prime number after num
template <typename Key, typename T, typename H>
size_t UnorderedMap<Key,T,H>::nextPrime(size_t num) {
    if(num <= 1) return 2;
    bool isPrime = false;
    while(!isPrime){
        isPrime = true;
        for(unsigned i = 2; i <=(num/2); ++i) {
            if(num%i==0) {
                num++;
                isPrime = false;
                break;
            }
        }
    }
    return num;
}


template <typename Key, typename T, typename H>
inline auto
UnorderedMap<Key,T,H>::map_iterator::operator++() -> map_iterator& {
    ++pos;

    while(pos == bucket->end() && bucket != ref.A.end()) {
        bucket++;
        pos = bucket->begin();
    }
    if(bucket == ref.A.end())
        pos = local_iterator(NULL);

    return *this;
}

template <typename Key, typename T, typename H>
inline auto
UnorderedMap<Key,T,H>::map_iterator::operator++(int) -> map_iterator {
    auto temp = *this;
    ++(*this);
    return temp;
}

template <typename Key, typename T, typename H>
inline auto
UnorderedMap<Key,T,H>::const_map_iterator::operator++()
-> const_map_iterator& {
    ++pos;

    while(pos == bucket->cend() && bucket != ref.A.cend()) {
        bucket++;
        pos = bucket->cbegin();
    }
    if(bucket == ref.A.cend())
        pos = const_local_iterator(NULL);

    return *this;
}

template <typename Key, typename T, typename H>
inline auto
UnorderedMap<Key,T,H>::const_map_iterator::operator++(int)
-> const_map_iterator {
    auto temp = *this;
    ++(*this);
    return temp;
}


#endif
