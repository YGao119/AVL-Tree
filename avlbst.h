#ifndef RBBST_H
#define RBBST_H

#include "bst.h"
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>

struct KeyError {};

/**
 * A special kind of node for an AVL tree, which adds the height as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */
template<typename Key, typename Value>
class AVLNode : public Node<Key, Value> {
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int getHeight() const;
    void setHeight(int height);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int height_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor and setting
 * the color to red since every new node will be red when it is first inserted.
 */
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent)
        : Node<Key, Value>(key, value, parent), height_(1) {}

/**
 * A destructor which does nothing.
 */
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode() {}

/**
 * A getter for the height of a AVLNode.
 */
template<class Key, class Value>
int AVLNode<Key, Value>::getHeight() const {
    return height_;
}

/**
 * A setter for the height of a AVLNode.
 */
template<class Key, class Value>
void AVLNode<Key, Value>::setHeight(int height) {
    height_ = height;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const {
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const {
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const {
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template<class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value> {
public:
    virtual void insert(const std::pair<const Key, Value>& new_item);  // TODO
    virtual void remove(const Key& key);                               // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2);

    // Add helper functions here
    void leftRotate(AVLNode<Key, Value>* node);
    void rightRotate(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n);
};

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    // TODO

    // if empty tree, create a new node and make it root
    if (BinarySearchTree<Key, Value>::empty()) {
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        newNode->setHeight(1);
        BinarySearchTree<Key, Value>::root_ = newNode;
        return;
    }

    // find current location to insert the node
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::root_);
    AVLNode<Key, Value>* p = NULL;
    while (curr != NULL) {
        p = curr;
        // if the key is already in the tree, update the value
        if (new_item.first == curr->getKey()) {
            curr->setValue(new_item.second);
            return;
        } else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        } else {
            curr = curr->getLeft();
        }
    }

    // create a new node and adjust pointers
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, p);

    if (new_item.first > p->getKey()) {
        p->setRight(n);
        n->setParent(p);
        n->setHeight(1);
    } else {
        p->setLeft(n);
        n->setParent(p);
        n->setHeight(1);
    }

    // adjust parent height
    if (p->getHeight() != 2) {
        if (p->getLeft() && p->getRight()) {
            p->setHeight(std::max(p->getLeft()->getHeight(), p->getRight()->getHeight()) + 1);
        } else {
            p->setHeight(2);
        }
        insertFix(p, n);
    }
}

/**
 * a recursive helper function to make tree balance after insertion
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n) {
    if (p->getParent() == NULL)
        return;
    AVLNode<Key, Value>* g = p->getParent();

    int lHeight = 0;
    int rHeight = 0;
    if (g->getLeft()) {
        lHeight = g->getLeft()->getHeight();
    }
    if (g->getRight()) {
        rHeight = g->getRight()->getHeight();
    }
    int newHeight = std::max(lHeight, rHeight) + 1;

    // check and update grande parent's height
    if (g->getHeight() == newHeight) {
        // std::cout << "balance" << std::endl;
        return;
    } else if (std::abs(lHeight - rHeight) <= 1) {
        g->setHeight(newHeight);
        insertFix(g, p);
    } else {
        // Left-Left case
        if (n == p->getLeft() && p == g->getLeft()) {
            rightRotate(g);
        }
        // Left-Right case
        else if (n == p->getRight() && p == g->getLeft()) {
            leftRotate(p);
            rightRotate(g);
        }
        // Right-Left case
        else if (n == p->getLeft() && p == g->getRight()) {
            rightRotate(p);
            leftRotate(g);
        }
        // Right-Right case
        else if (n == p->getRight() && p == g->getRight()) {
            leftRotate(g);
        }
    }
}

/**
 * a  helper function to make  a left rotate
 */
template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key, Value>* node) {

    AVLNode<Key, Value>* c = node->getRight();
    AVLNode<Key, Value>* c_lc = c->getLeft();

    node->setRight(c_lc);
    if (c_lc) {

        c_lc->setParent(node);
    }
    c->setLeft(node);

    // update pointers
    if (node->getParent() != NULL) {
        c->setParent(node->getParent());
        if (node == node->getParent()->getLeft()) {
            node->getParent()->setLeft(c);
        } else {
            node->getParent()->setRight(c);
        }

    } else {
        BinarySearchTree<Key, Value>::root_ = c;
        c->setParent(NULL);
    }
    node->setParent(c);

    // update heights
    int n_lHeight = 0;
    int n_rHeight = 0;
    if (node->getLeft()) {
        n_lHeight = node->getLeft()->getHeight();
    }
    if (node->getRight()) {
        n_rHeight = node->getRight()->getHeight();
    }
    node->setHeight(std::max(n_lHeight, n_rHeight) + 1);

    if (c->getRight()) {
        c->setHeight(std::max(node->getHeight(), c->getRight()->getHeight()) + 1);
    } else {
        c->setHeight(node->getHeight() + 1);
    }
}

/**
 * a  helper function to make  a right rotate
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key, Value>* node) {

    AVLNode<Key, Value>* c = node->getLeft();
    AVLNode<Key, Value>* c_rc = c->getRight();

    node->setLeft(c_rc);
    if (c_rc) {
        c_rc->setParent(node);
    }
    c->setRight(node);

    // update pointers
    if (node->getParent() != NULL) {
        c->setParent(node->getParent());
        if (node == node->getParent()->getLeft()) {
            node->getParent()->setLeft(c);
        } else {
            node->getParent()->setRight(c);
        }
    } else {
        BinarySearchTree<Key, Value>::root_ = c;
        c->setParent(NULL);
    }
    node->setParent(c);

    // update heights
    int n_lHeight = 0;
    int n_rHeight = 0;
    if (node->getLeft()) {
        n_lHeight = node->getLeft()->getHeight();
    }
    if (node->getRight()) {
        n_rHeight = node->getRight()->getHeight();
    }
    node->setHeight(std::max(n_lHeight, n_rHeight) + 1);

    if (c->getLeft()) {
        c->setHeight(std::max(node->getHeight(), c->getLeft()->getHeight()) + 1);
    } else {
        c->setHeight(node->getHeight() + 1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    // TODO
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
    if (!curr) {
        return;
    }

    AVLNode<Key, Value>* par = curr->getParent();

    // no child case
    if (!curr->getLeft() && !curr->getRight()) {
        if (curr == BinarySearchTree<Key, Value>::root_) {
            BinarySearchTree<Key, Value>::root_ = NULL;
            delete curr;
        } else {
            if (curr == par->getLeft()) {
                par->setLeft(NULL);
            } else {
                par->setRight(NULL);
            }

            delete curr;
        }

    } else if (!curr->getLeft() && curr->getRight()) {  // one right child case
        AVLNode<Key, Value>* temp = curr->getRight();
        if (curr == BinarySearchTree<Key, Value>::root_) {
            BinarySearchTree<Key, Value>::root_ = temp;
            temp->setParent(NULL);
            delete curr;
        } else {
            if (curr == par->getLeft()) {
                par->setLeft(temp);
            } else {
                par->setRight(temp);
            }
            temp->setParent(par);
            delete curr;
        }

    } else if (!curr->getRight() && curr->getLeft()) {  // one left child case
        AVLNode<Key, Value>* temp = curr->getLeft();
        if (curr == BinarySearchTree<Key, Value>::root_) {
            BinarySearchTree<Key, Value>::root_ = temp;
            temp->setParent(NULL);
            delete curr;
        } else {
            if (curr == par->getLeft()) {
                par->setLeft(temp);
            } else {
                par->setRight(temp);
            }
            temp->setParent(par);
            delete curr;
        }
    } else {  // two children case
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(curr));
        nodeSwap(curr, pred);
        par = curr->getParent();
        if (curr->getLeft()) {
            if (curr == par->getLeft()) {
                par->setLeft(curr->getLeft());
            } else {
                par->setRight(curr->getLeft());
            }
            curr->getLeft()->setParent(par);
            delete curr;
        } else if (curr->getRight()) {
            if (curr == par->getLeft()) {
                par->setLeft(curr->getRight());
            } else {
                par->setRight(curr->getRight());
            }
            curr->getRight()->setParent(par);
            delete curr;
        } else {
            if (curr == par->getLeft())
                par->setLeft(NULL);
            else
                par->setRight(NULL);

            delete curr;
        }
    }

    removeFix(par);
}

/**
 * a recursive helper function to make tree balance after deletion
 */
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n) {

    if (!n) {
        return;
    }
    int n_lHeight = 0;
    int n_rHeight = 0;
    if (n->getLeft()) {
        n_lHeight = n->getLeft()->getHeight();
    }
    if (n->getRight()) {
        n_rHeight = n->getRight()->getHeight();
    }

    AVLNode<Key, Value>* p = n->getParent();
    // if n is out of balance
    if (std::abs(n_lHeight - n_rHeight) > 1) {

        // determine c, the taller child of n
        AVLNode<Key, Value>* c = NULL;
        int first;   // 0 = left, 1 = right
        int second;  // 0 = left, 1 = right    00 = ll 01 = lr 11 = rr 10 = rl
        if (n_lHeight > n_rHeight) {
            c = n->getLeft();
            first = 0;
        } else {
            c = n->getRight();
            first = 1;
        }

        int c_lHeight = 0;
        int c_rHeight = 0;

        if (c->getLeft()) {
            c_lHeight = c->getLeft()->getHeight();
        }
        if (c->getRight()) {
            c_rHeight = c->getRight()->getHeight();
        }

        if (c_lHeight > c_rHeight) {
            second = 0;
        } else if (c_lHeight < c_rHeight) {
            second = 1;
        } else {
            if (first == 0) {
                second = 0;
            } else {
                second = 1;
            }
        }

        // zig-zig case
        if (first == second) {
            // left-left case
            if (first == 0) {
                rightRotate(n);
            }
            // right-right case
            else {
                leftRotate(n);
            }

        }
        // zig-zag case
        else {
            // left-right case
            if (first == 0) {
                leftRotate(c);
                rightRotate(n);

            } else {
                rightRotate(c);
                leftRotate(n);
            }
        }

        removeFix(p);

    }
    // if n's height is unchanged. Done!
    else if (n->getHeight() == std::max(n_lHeight, n_rHeight) + 1) {

        return;
    }
    // if n's height changed but is still balanced, recursion
    else {
        n->setHeight(std::max(n_lHeight, n_rHeight) + 1);
        removeFix(p);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2) {
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int tempH = n1->getHeight();
    n1->setHeight(n2->getHeight());
    n2->setHeight(tempH);
}

#endif
