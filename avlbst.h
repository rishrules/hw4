#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

  // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalanceUp(AVLNode<Key, Value>* node, int diff, bool inserting);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node){
  if (node == nullptr || node->getRight() == nullptr){
    return;
  }

  AVLNode<Key, Value>* pivot = node->getRight();
  AVLNode<Key, Value>* parent = node->getParent();
  AVLNode<Key, Value>* middle = pivot->getLeft();

  pivot->setParent(parent);
  if (parent == nullptr) {
    this->root_ = pivot;
  }
  else if (parent->getLeft() == node) {
    parent->setLeft(pivot);
  }
  else {
    parent->setRight(pivot);
  }

  node->setRight(middle);
  if (middle != nullptr) {
    middle->setParent(node);
  }

  pivot->setLeft(node);
  node->setParent(pivot);

  int nodeB = node->getBalance();
  int pivotB = pivot->getBalance();

  nodeB = nodeB - 1 - std::max(pivotB, 0);
  pivotB = pivotB - 1 + std::min(nodeB, 0);

  node->setBalance(nodeB);
  pivot->setBalance(pivotB);
}
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
  if (node == nullptr || node->getLeft() == nullptr) {
    return;
  }

  AVLNode<Key, Value>* pivot = node->getLeft();
  AVLNode<Key, Value>* parent = node->getParent();
  AVLNode<Key, Value>* middle = pivot->getRight();

  pivot->setParent(parent);
  if (parent == nullptr) {
    this->root_ = pivot;
  }
  else if (parent->getLeft() == node) {
    parent->setLeft(pivot);
  }
  else {
    parent->setRight(pivot);
  }

  node->setLeft(middle);
  if (middle != nullptr) {
    middle->setParent(node);
  }

  pivot->setRight(node);
  node->setParent(pivot);

  int nodeB = node->getBalance();
  int pivotB = pivot->getBalance();

  nodeB = nodeB + 1 - std::min(pivotB, 0);
  pivotB = pivotB + 1 + std::max(nodeB, 0);

  node->setBalance(nodeB);
  pivot->setBalance(pivotB);
}
template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceUp(AVLNode<Key, Value>* node, int diff, bool inserting){
  while (node != nullptr) {
    node->updateBalance(diff);
    int balance = node->getBalance();
    AVLNode<Key, Value>* subtreeRoot = node;
    if (balance == -2) {
      // Left-right case requires two rotations.
      if (node->getLeft()->getBalance() > 0) {
        rotateLeft(node->getLeft());
      }
      rotateRight(node);
      subtreeRoot = node->getParent();
      // Insertion is finished after fixing the first imbalance.
      // After removal, a nonzero balance means height stayed the same.
      if (inserting || subtreeRoot->getBalance() != 0) {
        return;
      }
    }
    else if (balance == 2) {
      // Right-left case requires two rotations.
      if (node->getRight()->getBalance() < 0) {
        rotateRight(node->getRight());
      }
      rotateLeft(node);
      subtreeRoot = node->getParent();
      if (inserting || subtreeRoot->getBalance() != 0) {
        return;
      }
    }
    else {
      // Stop when the subtree's height did not change.
      if (inserting && balance == 0) {
        return;
      }
      if (!inserting && balance != 0) {
        return;
      }
    }

    AVLNode<Key, Value>* parent = subtreeRoot->getParent();
    if (parent == nullptr) {
      return;
    }

    if (parent->getLeft() == subtreeRoot) {
      diff = inserting ? -1 : 1;
    }
    else {
      diff = inserting ? 1 : -1;
    }

    node = parent;
  }
}
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
  // TODO
  if (this->root_ == nullptr) {
    this->root_ = new AVLNode<Key, Value>(
    new_item.first, new_item.second, nullptr);
    return;
  }

  AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

  while (true) {
    if (new_item.first == current->getKey()) {
      current->setValue(new_item.second);
      return;
    }

    if (new_item.first < current->getKey()) {
      if (current->getLeft() == nullptr) {
        current->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, current));

        rebalanceUp(current, -1, true);
        return;
      }

      current = current->getLeft();
    }
    else {
      if (current->getRight() == nullptr) {
        current->setRight(new AVLNode<Key, Value>(
        new_item.first, new_item.second, current));
        rebalanceUp(current, 1, true);
        return;
      }
      current = current->getRight();
    }
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
  // TODO
  AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

  if (node == nullptr) {
    return;
  }

  if (node->getLeft() != nullptr &&
    node->getRight() != nullptr) {
    AVLNode<Key, Value>* pred =
    static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
    nodeSwap(node, pred);
  }

  AVLNode<Key, Value>* parent = node->getParent();
  AVLNode<Key, Value>* child = node->getLeft();

  if (child == nullptr) {
    child = node->getRight();
  }

  int diff = 0;

  if (parent == nullptr) {
    this->root_ = child;
  }
  else if (parent->getLeft() == node) {
    parent->setLeft(child);
    diff = 1;  // Left subtree became shorter.
  }
  else {
    parent->setRight(child);
    diff = -1; // Right subtree became shorter.
  }

  if (child != nullptr) {
    child->setParent(parent);
  }

  delete node;

  if (parent != nullptr) {
    rebalanceUp(parent, diff, false);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
