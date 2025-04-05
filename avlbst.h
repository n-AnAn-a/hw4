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
    void updateBalanceInsert(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void updateBalanceRemove(AVLNode<Key, Value>* node, int8_t diff);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void doubleRotation(int8_t b, AVLNode<Key, Value>* child, AVLNode<Key, Value>* parent, AVLNode<Key, Value>* grand, bool leftHeavy);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& item) {
  if (this->root_ == nullptr) {
    this->root_ = new AVLNode<Key, Value>(item.first, item.second, nullptr);
    return;
  }

  AVLNode<Key, Value>* found = static_cast<AVLNode<Key, Value>*>(this->internalFind(item.first));
  if (found != nullptr) {
    found->setValue(item.second);
    return;
  }

  AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
  AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(item.first, item.second, nullptr);

  while (current != nullptr) {
    if (item.first < current->getKey()) {
      if (current->getLeft() == nullptr) {
        current->setLeft(newNode);
        newNode->setParent(current);
        break;
      }
      current = current->getLeft();
    } 
    else {
      if (current->getRight() == nullptr) {
        current->setRight(newNode);
        newNode->setParent(current);
        break;
      }
      current = current->getRight();
    }
  }

  if (current->getBalance() == 1 || current->getBalance() == -1) {
    current->setBalance(0);
    return;
  } 
  else if (current->getBalance() == 0) {
    current->updateBalance(newNode == current->getLeft() ? -1 : 1);
    updateBalanceInsert(current, newNode);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceInsert(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child) {
  if (!parent || parent == this->root_) return;

  AVLNode<Key, Value>* grand = parent->getParent();
  if (!grand) return;

  bool isLeft = (grand->getLeft() == parent);
  grand->updateBalance(isLeft ? -1 : 1);

  int8_t balance = grand->getBalance();
  if (balance == 0) return;
  if (balance == -1 || balance == 1) {
    updateBalanceInsert(grand, parent);
    return;
  }

  if (balance == -2) {
    bool isLeftLeft = (parent->getLeft() == child);
    if (isLeftLeft) {
      parent->setBalance(0);
      grand->setBalance(0);
      rotateRight(grand);
    } 
    else {
      int8_t b = child->getBalance();
      rotateLeft(parent);
      rotateRight(grand);
      doubleRotation(b, child, parent, grand, true);
    }
  } 
  else if (balance == 2) {
    bool isRightRight = (parent->getRight() == child);
    if (isRightRight) {
      parent->setBalance(0);
      grand->setBalance(0);
      rotateLeft(grand);
    } 
    else {
      int8_t b = child->getBalance();
      rotateRight(parent);
      rotateLeft(grand);
      doubleRotation(b, child, parent, grand, false);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::doubleRotation(int8_t b, AVLNode<Key, Value>* child, AVLNode<Key, Value>* parent, AVLNode<Key, Value>* grand, bool leftHeavy) {
  child->setBalance(0);
  if (b == 0) {
    parent->setBalance(0);
    grand->setBalance(0);
  } 
  else if ((b == -1 && leftHeavy) || (b == 1 && !leftHeavy)) {
    parent->setBalance(0);
    grand->setBalance(leftHeavy ? 1 : -1);
  } 
  else {
    parent->setBalance(leftHeavy ? -1 : 1);
    grand->setBalance(0);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
  if (this->root_ == nullptr) return;

  if (this->root_->getKey() == key && this->root_->getLeft() == nullptr && this->root_->getRight() == nullptr) {
    delete this->root_;
    this->root_ = nullptr;
    return;
  }

  AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
  if (target == nullptr) return;

  if (target->getLeft() && target->getRight()) {
    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(target));
    nodeSwap(target, pred);
  }

  AVLNode<Key, Value>* parent = target->getParent();
  int8_t balanceAdjust;

  // No children
  if (!target->getLeft() && !target->getRight()) {
    if (parent == nullptr || this->root_ == target) {
      this->root_ = nullptr;
    } 
    else {
      if (parent->getLeft() == target) {
        parent->setLeft(nullptr);
        balanceAdjust = 1;
      } 
      else {
        parent->setRight(nullptr);
        balanceAdjust = -1;
      }
    }
  }
  // One child
  else {
    AVLNode<Key, Value>* child = target->getLeft() ? target->getLeft() : target->getRight();
    balanceAdjust = (target->getLeft() ? 1 : -1);

    if (target == this->root_) {
      this->root_ = child;
      child->setParent(nullptr);
    } 
    else {
      if (parent->getLeft() == target) {
        parent->setLeft(child);
        child->setParent(parent);
        balanceAdjust = 1;
      } 
      else {
        parent->setRight(child);
        child->setParent(parent);
        balanceAdjust = -1;
      }
    }
  }

  delete target;
  updateBalanceRemove(parent, balanceAdjust);
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
  BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
  int8_t tempB = n1->getBalance();
  n1->setBalance(n2->getBalance());
  n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceRemove(AVLNode<Key, Value>* node, int8_t diff) {
  if (!node) return;

  AVLNode<Key, Value>* parent = node->getParent();
  int8_t pdiff = (parent && parent->getLeft() == node) ? 1 : -1;

  int8_t balance = node->getBalance() + diff;
  node->setBalance(balance);

  if (balance == -2 || balance == 2) {
    AVLNode<Key, Value>* child = (balance == -2) ? node->getLeft() : node->getRight();
    int8_t childBalance = child->getBalance();

    if ((balance == -2 && childBalance == -1) || (balance == 2 && childBalance == 1)) {
    // Single Rotation
      if (balance == -2) rotateRight(node);
      else rotateLeft(node);

      node->setBalance(0);
      child->setBalance(0);
      updateBalanceRemove(parent, pdiff);
    } 
    else if (childBalance == 0) {
    // Single Rotation, but stop recursion
      if (balance == -2) {
        rotateRight(node);
        node->setBalance(-1);
        child->setBalance(1);
      } 
      else {
        rotateLeft(node);
        node->setBalance(1);
        child->setBalance(-1);
      }
    } 
    else {
    // Double Rotation
      AVLNode<Key, Value>* grand = (balance == -2) ? child->getRight() : child->getLeft();
      int8_t b = grand->getBalance();

      if (balance == -2) {
        rotateLeft(child);
        rotateRight(node);
      } 
      else {
        rotateRight(child);
        rotateLeft(node);
      }

      node->setBalance((b == (balance == -2 ? -1 : 1)) ? (balance == -2 ? 1 : -1) : 0);
      child->setBalance((b == (balance == -2 ? 1 : -1)) ? (balance == -2 ? -1 : 1) : 0);
      grand->setBalance(0);

      updateBalanceRemove(parent, pdiff);
      }
    } 
  else if (balance == 0) {
    updateBalanceRemove(parent, pdiff);
  }
  // if balance == -1 or 1, no further action is needed
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y) {
  AVLNode<Key, Value>* x = y->getLeft();
  AVLNode<Key, Value>* B = x->getRight();

  x->setParent(y->getParent());
  if (y == this->root_) {
    this->root_ = x;
  } 
  else {
    if (y->getParent()->getLeft() == y)
      y->getParent()->setLeft(x);
    else
      y->getParent()->setRight(x);
  }

  x->setRight(y);
  y->setParent(x);
  y->setLeft(B);
  if (B) B->setParent(y);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* y) {
  AVLNode<Key, Value>* x = y->getRight();
  AVLNode<Key, Value>* B = x->getLeft();

  x->setParent(y->getParent());
  if (y == this->root_) {
    this->root_ = x;
  } 
  else {
    if (y->getParent()->getLeft() == y)
      y->getParent()->setLeft(x);
    else
      y->getParent()->setRight(x);
  }

  x->setLeft(y);
  y->setParent(x);
  y->setRight(B);
  if (B) B->setParent(y);
}

#endif
