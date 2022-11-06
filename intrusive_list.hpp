#pragma once

#include <cstddef>

template <typename T>
struct IntrusiveNode {
    IntrusiveNode<T>* next;
    IntrusiveNode<T>* prev;

    IntrusiveNode(): next(this), prev(this){}

    // Disable copying
    IntrusiveNode(const IntrusiveNode&) = delete;
    IntrusiveNode& operator = (const IntrusiveNode&) = delete;

    bool unlinked() const {
        return next == this;
    }

    void unlink(){
        next = this;
        prev = this;
    }
};

template <typename T, IntrusiveNode<T> T::*link>
struct IntrusiveList {
    typedef IntrusiveNode<T> Node;
    Node sentinel;

    IntrusiveList(){}
    
    // Disable copying of const lists
    IntrusiveList(const IntrusiveList&) = delete;
    IntrusiveList operator = (const IntrusiveList&) = delete;
    
    IntrusiveList(IntrusiveList &other){
        steal(other);
    }

    IntrusiveList& operator = (IntrusiveList &other){
        steal(other);
        return *this;
    }

    static T* node_to_value(Node *node){
        size_t offset = reinterpret_cast<size_t>(&(reinterpret_cast<T*>(NULL)->*link));
        return reinterpret_cast<T*>(reinterpret_cast<char*>(node) - offset);
    }

    static Node* value_to_node(T *value){
        return &(value->*link);
    }

    static const T* node_to_value(const Node *node){
        size_t offset = reinterpret_cast<size_t>(&(reinterpret_cast<T*>(NULL)->*link));
        return reinterpret_cast<const T*>(reinterpret_cast<const char*>(node) - offset);
    }

    static const Node* value_to_node(const T *value){
        return &(value->*link);
    }

    struct iterator {
        Node *node;
        
        iterator(Node* node): node(node) {}

        T* operator -> (){
            return node_to_value(node);
        }

        T& operator * (){
            return *node_to_value(node);
        }
        
        iterator& operator ++ (){
            node = node->next;
            return *this;
        }
        
        iterator& operator -- (){
            node = node->prev;
            return *this;
        }
        
        bool operator == (const iterator &it) const {
            return node == it.node;
        }
        
        bool operator != (const iterator &it) const {
            return node != it.node;
        }

        iterator erase(){
            Node *prev = node->prev;
            Node *next = node->next;
            
            prev->next = next;
            next->prev = prev;

            node->unlink();
            
            return iterator(next);
        }
    };

    struct const_iterator {
        const Node *node;
        
        const_iterator(const Node* node): node(node) {}

        const T* operator -> () const {
            return node_to_value(node);
        }

        const T& operator * () const {
            return *node_to_value(node);
        }
        
        const_iterator& operator ++ (){
            node = node->next;
            return *this;
        }
        
        const_iterator& operator -- (){
            node = node->prev;
            return *this;
        }
        
        bool operator == (const const_iterator &it) const {
            return node == it.node;
        }
        
        bool operator != (const const_iterator &it) const {
            return node != it.node;
        }
    };

    iterator begin(){
        return iterator(sentinel.next);
    }

    iterator end(){
        return iterator(&sentinel);
    }

    const_iterator begin() const {
        return const_iterator(sentinel.next);
    }

    const_iterator end() const {
        return const_iterator(&sentinel);
    }

    void insert(iterator it, T &value){
        Node *node = value_to_node(&value);
        Node *prev = it.node->prev;
        Node *next = it.node;
        
        node->next = prev->next;
        node->prev = prev;
        
        prev->next = node;
        next->prev = node;
    }

    void push_front(T &value){
        insert(begin(), value);
    }

    void push_back(T &value){
        insert(end(), value);
    }

    void pop_front(){
        erase(begin());
    }

    void pop_back(){
        erase(--end());
    }

    bool empty() const {
        return sentinel.next == &sentinel;
    }

    iterator erase(iterator it){
        return it.erase();
    }

    static void erase_value(T &value){
        Node *node = value_to_node(&value);
        
        iterator(node).erase();
    }

    void steal(IntrusiveList &other){
        if (!other.empty()){
            sentinel.next = other.sentinel.next;
            sentinel.prev = other.sentinel.prev;
            
            sentinel.next->prev = &sentinel;
            sentinel.prev->next = &sentinel;

            other.sentinel.next = &other.sentinel;
            other.sentinel.prev = &other.sentinel;
        }else{
            sentinel.unlink();
        }
    }
};
