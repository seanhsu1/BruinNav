//
//  MyMap.hpp
//  Project4
//
//  Created by Shangyu Hsu on 3/9/17.
//  Copyright © 2017 Shangyu Hsu. All rights reserved.
//

#ifndef MyMap_h
#define MyMap_h

#include <string>
#include "support.h"

using namespace std;


template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap();
    
    ~MyMap();
    
    void clear();
    
    int size() const;
    
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key);
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    struct Node
    {
        Node(const KeyType& k, const ValueType& v)
        {
            key=k;
            value=v;
            left=right=nullptr;
        }
        KeyType key;
        ValueType value;
        Node* left;
        Node* right;
    };
    
    int size(Node* n) const
    {
        if(n==nullptr)
            return 0;
        return 1+size(n->left)+size(n->right);
    }
    
    void clear(Node* n)
    {
        if(n==nullptr)
            return;
        clear(n->left);
        clear(n->right);
        delete n;
    }
    
    Node* m_root;
};


template<typename KeyType, typename ValueType>
MyMap<KeyType,ValueType>::MyMap()
{
    m_root=nullptr;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType,ValueType>::~MyMap()
{
    clear();
}

//deallocate all nodes
template<typename KeyType, typename ValueType>
void MyMap<KeyType,ValueType>::clear()
{
    clear(m_root);
}

//return number of associations
template<typename KeyType, typename ValueType>
int MyMap<KeyType,ValueType>::size() const
{
    return size(m_root);
}

//associate KeyType key and ValueType value
template<typename KeyType, typename ValueType>
void MyMap<KeyType,ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if(m_root==nullptr)
    {
        m_root = new Node(key,value);
        return;
    }
    Node* current = m_root;
    for(;;)
    {
        if(key==current->key)
        {
            //if key already associated with a value, override the value
            current->value=value;
            return;
        }
        if(key<current->key)
        {
            if(current->left!=nullptr)
                current=current->left; //go to left node
            else
            {
                //if left child doesn't exist, set child as new node with value
                current->left=new Node(key, value);
                return;
            }
            
        }
        else if(key>current->key)
        {
            if(current->right!=nullptr)
                current=current->right; //go to right node
            else
            {
                //if right child doesn't exist, set child as new node with value
                current->right=new Node(key, value);
                return;
            }
            
        }
    }
}

// for a map that can't be modified, return a pointer to const ValueType
template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType,ValueType>::find(const KeyType& key) const
{
    Node* current = m_root;
    while (current != nullptr) {
        if (key == current->key)
            return &current->value;
        else if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }
    return nullptr;
}

// for a modifiable map, return a pointer to modifiable ValueType
template<typename KeyType, typename ValueType>
ValueType* MyMap<KeyType,ValueType>::find(const KeyType& key)
{
    return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
}



#endif /* MyMap_h */
