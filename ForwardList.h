/**
 * @file ForwardList.h
 * @author Jackson Brenneman
 * @brief Singly linked list template class
 * @date 2023-11-5
 *
 */

#ifndef _FORWARD_LIST_H_
#define _FORWARD_LIST_H_

#include <stdexcept>
#include <iostream>

using std::logic_error;
using std::invalid_argument;
using std::out_of_range;
using std::istream;
using std::ostream;

template <typename T>
struct Node
{
    T data;
    Node *next;

    Node() = default;
    Node(const T& d, Node *p = NULL) : data(d), next(p) {}
    Node(T&& d, Node *p = NULL) : data(std::move(d)), next(p) {}
    Node(const Node& rhs) = default;
    Node(Node&& rhs) = default;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *               ForwardList Class Declaration
 *
 *            Singly linked list w/ iterator
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
class ForwardList
{
    template <typename val_type = T, typename ptr_type = Node<T>*>
    class List_Iterator;

    Node<T> *head = NULL;
    size_t _size = 0;

public:
    typedef List_Iterator<T, Node<T>*> iterator;
    typedef List_Iterator<const T, const Node<T>*> const_iterator;

    ForwardList() : head(NULL), _size(0) {}
    ForwardList(size_t, const T& = T());
    ForwardList(const ForwardList&);
    ForwardList(ForwardList&&) = default;
    ~ForwardList();

    ForwardList& operator=(const ForwardList&);
    ForwardList& operator=(ForwardList&&) = default;

          T& front() { return head->data; }
    const T& front() const { return head->data; }

          iterator begin() noexcept { return iterator(head); }
          iterator end() noexcept { return iterator(NULL); }
    const_iterator begin() const noexcept { return const_iterator(head); }
    const_iterator end() const noexcept { return const_iterator(NULL); }
    const_iterator cbegin() const noexcept { return const_iterator(head); }
    const_iterator cend() const noexcept { return const_iterator(NULL); }

    bool empty() const noexcept { return head == NULL; }
    size_t size() const noexcept { return _size; }

    void clear() noexcept;
    iterator insert_after(const_iterator, const T&);
    iterator insert_after(const_iterator, T&&);
    iterator erase_after(const_iterator);
    void push_front(const T&);
    void push_front(T&&);
    void pop_front();

    void remove(const T&);

private:
    template <typename val_type, typename ptr_type>
    class List_Iterator
    {
    public:
        List_Iterator(ptr_type ptr) { current = ptr; }

        val_type& operator*() { return current->data; }
        val_type* operator->() { return &(current->data); }
        const val_type& operator*() const { return current->data; }
        const val_type* operator->() const { return &(current->data); }

        List_Iterator operator++() { current = current->next; return *this; }

        bool operator==(const List_Iterator &rhs) const noexcept
            { return this->current == rhs.current; }
        bool operator!=(const List_Iterator &rhs) const noexcept
            { return !(*this == rhs); }

    private:
        ptr_type current;
    };

public:
    template <typename F>
    friend ostream& operator<<(ostream &, const ForwardList<F>&);
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             ForwardList Class Definitions               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Default constructor
template <typename T>
ForwardList<T>::ForwardList(size_t n, const T& val) {
    head = NULL;
    _size = 0;

    for(size_t i = 0; i < n; i++)
        push_front(val);
}

// Copy constructor
template <typename T>
ForwardList<T>::ForwardList(const ForwardList& rhs) {
    head = NULL;
    _size = 0;

    auto itr = rhs.cbegin();
    while(itr != rhs.cend()) {
        push_front(*itr);
        ++itr;
    }
}

/**
 * List Destructor
 * Calls the clear method for list deletion
 */
template <typename T>
ForwardList<T>::~ForwardList() {
    clear();
    delete head;
}

// Overloaded assignment copy
template <typename T>
ForwardList<T>& ForwardList<T>::operator=(const ForwardList& rhs) {
    if(this == &rhs)
        return *this;
    else if(!empty())
        clear();

    auto itr = rhs.cbegin();
    while(itr != rhs.cend()) {
        push_front(*itr);
        ++itr;
    }

    return *this;
}

// Deletes all nodes in list
template <typename T>
void ForwardList<T>::clear() noexcept {
    if(empty())
        return;

    Node<T> *ptr = head;
    while(ptr != nullptr) {
        ptr = head->next;
        delete head;
        head = ptr;
    }
    head = NULL;
    _size = 0;
}

// Inserts new node after node pointed to by itr
template <typename T>
typename ForwardList<T>::iterator
ForwardList<T>::insert_after(const_iterator itr, const T& info) {
    auto newNode = new Node<T>(info, itr.current->next);
    itr.current->next = newNode;

    _size++;

    return iterator(newNode);
}

// Inserts new node after node pointed to by itr
template <typename T>
typename ForwardList<T>::iterator
ForwardList<T>::insert_after(const_iterator itr, T&& info) {
    auto newNode = new Node<T>(std::move(info), itr.current->next);
    itr.current->next = newNode;

    _size++;

    return iterator(newNode);
}

// Removes node after node pointed to by itr
template <typename T>
typename ForwardList<T>::iterator
ForwardList<T>::erase_after(const_iterator itr) {
    auto temp = itr.current->next->next;
    delete itr.current->next;
    itr.current->next = temp;

    _size--;

    return iterator(temp);
}

// Inserts new node at front of list
template <typename T>
void ForwardList<T>::push_front(const T& info) {
    auto newNode = new Node<T>(info, head);
    head = newNode;
    _size++;
}

// Inserts new node at front of list
template <typename T>
void ForwardList<T>::push_front(T&& info) {
    auto newNode = new Node<T>(std::move(info), head);
    head = newNode;
    _size++;
}

// Removes node at front of list
template <typename T>
void ForwardList<T>::pop_front() {
    auto temp = head->next;
    delete head;
    head = temp;

    _size--;
}

// Removes all nodes with data matching val
template <typename T>
void ForwardList<T>::remove(const T& val) {
    auto itr = cbegin();
    while(itr.current->next != NULL) {
        if(itr.current->next->data == val) {
            erase_after(itr);
        }
        else
            ++itr;
    }
}


template <typename F>
ostream& operator<<(ostream &os, const ForwardList<F> &rhs) {
    if(rhs.empty()) return os;

    auto itr = rhs.cbegin();
    while (itr != rhs.cend()) {
        os << *itr << " ";
        ++itr;
    }
    return os;
}


#endif //_FORWARD_LIST_H_
