/*
 *   Copyright (c) 2011 Higepon(Taro Minowa) <higepon@users.sourceforge.jp>
 *                      Brad Appleton <bradapp@enteract.com>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef AVL_TREE_H_
#define AVL_TREE_H_

// Based on AvlTrees by Brad Appleton <bradapp@enteract.com>.
// http://www.cmcrossroads.com/bradapp/ftp/src/libs/C++/AvlTrees.html
// See LICENSE_AvlTrees.txt

template <class KeyType, class ValueType> class AVLTree {
 private:
  enum CompareResult {
    kMinCmp = -1,
    kEqCmp = 0,
    kMaxCmp = 1
  };

  enum Direction {
    kLeft = 0,
    kRight = 1
  };

 public:

  class Comparable {
   private:
    KeyType key;
    ValueType value;

   public:
    Comparable(KeyType key, ValueType value) : key(key), value(value) {}

    CompareResult Compare(KeyType key) const {
      return (key == this->key) ? kEqCmp
          : ((key < this->key) ? kMinCmp : kMaxCmp);
    }

    KeyType Key() const {
      return key;
    }

    ValueType Value() const {
      return value;
    }

    void SetValue(ValueType v) {
      value = v;
    }
  };

  enum BalanceFactor {
    kE = 0,
    kL = -1,
    kR = 1
  };

  struct Node {
    explicit Node(Comparable* item) :
        item(item),
        balance_factor(kE) {
      children[kLeft] = NULL;
      children[kRight] = NULL;
    }

    ~Node() {
      if (Left()) {
        delete Left();
      }
      if (Right()) {
        delete Right();
      }
    }

    bool IsLeftImbalance() const {
      return balance_factor < kL;
    }

    bool IsRightImbalance() const {
      return balance_factor > kR;
    }

    Node* Right() const {
      return children[kRight];
    }

    Node* Left() const {
      return children[kLeft];
    }

    static Direction Opposite(Direction dir) {
      return static_cast<Direction>(1 - static_cast<int>(dir));
    }

    static int RotateOnce(Node*& root, Direction dir) {
      Direction other_dir = Opposite(dir);
      Node* old_root = root;

      int height_change = (root->children[other_dir]->balance_factor == 0)
          ? kHeightNoChange : kHeightChange;

      root = old_root->children[other_dir];

      old_root->children[other_dir] = root->children[dir];
      root->children[dir] = old_root;

      old_root->balance_factor =
          -((dir == kLeft) ?
            --(root->balance_factor) : ++(root->balance_factor));

      return  height_change;
    }

    static int min(int a, int b) {
      return  (a < b) ? a : b;
    }

    static int max(int a, int b) {
      return  (a > b) ? a : b;
    }

    static int RotateTwice(Node*& root, Direction dir) {
      Direction other_dir = Opposite(dir);
      Node* old_root = root;
      Node* old_other_dir_subtree = root->children[other_dir];

      root = old_root->children[other_dir]->children[dir];

      old_root->children[other_dir] = root->children[dir];
      root->children[dir] = old_root;

      old_other_dir_subtree->children[dir] = root->children[other_dir];
      root->children[other_dir] = old_other_dir_subtree;

      root->Left()->balance_factor =
          -max(static_cast<int>(root->balance_factor), 0);
      root->Right()->balance_factor =
          -min(static_cast<int>(root->balance_factor), 0);
      root->balance_factor = 0;

      return kHeightChange;
    }

    static Comparable* Get(KeyType key, Node* root, CompareResult cmp) {
      CompareResult result;
      while (root &&  (result = root->Compare(key, cmp))) {
        root = root->children[(result < 0) ? kLeft : kRight];
      }
      return root ? root->item : NULL;
    }

    static Comparable* Insert(Comparable* item, Node*& root) {
      int change;
      return Insert(item, root, change);
    }

    static Comparable* Insert(Comparable* item, Node*& root, int& change) { // NOLINT
      if (root == NULL) {
        root = new Node(item);
        change = kHeightChange;
        return NULL;
      }

      Comparable* found = NULL;
      CompareResult result = root->Compare(item->Key());
      Direction dir = (result == kMinCmp) ? kLeft : kRight;

      int increase = 0;
      if (result != kEqCmp) {
        found = Insert(item, root->children[dir], change);
        if (found) {
          return found;
        }
        increase = result * change;
      } else  {
        increase = kHeightNoChange;
        return root->item;
      }

      root->balance_factor += increase;
      change = (increase && root->balance_factor)
          ? (1 - ReBalance(root)) : kHeightNoChange;
      return NULL;
    }

    static int ReBalance(Node*& root) {
      int height_change = kHeightNoChange;

      if (root->IsLeftImbalance()) {
        if (root->Left()->balance_factor == kR) {
          height_change = RotateTwice(root, kRight);
        } else {
          height_change = RotateOnce(root, kRight);
        }
      } else if (root->IsRightImbalance()) {
        if (root->Right()->balance_factor == kL) {
          height_change = RotateTwice(root, kLeft);
        } else {
          height_change = RotateOnce(root, kLeft);
        }
      }

      return height_change;
    }

    static Comparable* Remove(KeyType key, Node*& root, CompareResult cmp) {
      int change;
      return Remove(key, root, change, cmp);
    }

    static Comparable* Remove(const KeyType key,
                              Node*& root,
                              int& change,
                              CompareResult cmp) {
      if (root == NULL) {
        change = kHeightNoChange;
        return NULL;
      }

      Comparable* found = NULL;
      int decrease = 0;

      CompareResult result = root->Compare(key, cmp);
      Direction dir = (result == kMinCmp) ? kLeft : kRight;

      if (result != kEqCmp) {
        found = Remove(key, root->children[dir], change, cmp);
        if (!found) {
          return found;
        }
        decrease = result * change;
      } else  {
        found = root->item;
        if ((root->Left() == NULL) &&
            (root->Right() == NULL)) {
          delete root;
          root = NULL;
          change = kHeightChange;
          return  found;
        } else if ((root->Left() == NULL) ||
                   (root->Right() == NULL)) {
          Node* toDelete = root;
          root = root->children[(root->Right()) ? kRight : kLeft];
          change = kHeightChange;
          toDelete->children[kLeft] = toDelete->children[kRight] = NULL;
          delete  toDelete;
          return  found;
        } else {
          root->item = Remove(key, root->children[kRight],
                              decrease, kMinCmp);
        }
      }
      root->balance_factor -= decrease;

      if (decrease) {
        if (root->balance_factor) {
          change = ReBalance(root);
        } else {
          change = kHeightChange;
        }
      } else {
        change = kHeightNoChange;
      }
      return  found;
    }

    CompareResult Compare(KeyType key, CompareResult cmp = kEqCmp) const {
      switch (cmp) {
        case kEqCmp:
          return item->Compare(key);
        case kMinCmp:
          return  (children[kLeft] == NULL) ? kEqCmp : kMinCmp;
        default:
          return (children[kRight] == NULL) ? kEqCmp : kMaxCmp;
      }
    }

    Node* children[2];
    Comparable* item;
    int balance_factor;

   private:
    Node() {}
    Node(const Node& n) {}
    Node & operator=(const Node&) {}
  };

  AVLTree() : root_(NULL) {
  }

  virtual ~AVLTree() {
    if (root_) {
      delete root_;
    }
  }

  Node* Root() const {
    return root_;
  }

  void Add(const KeyType key, const ValueType value) {
    Comparable* item = new Comparable(key, value);
    Comparable* result = Node::Insert(item, root_);
    if (result) {
      result->SetValue(value);
      delete item;
    }
  }

  Comparable* Remove(const KeyType key, CompareResult cmp = kEqCmp) {
    return Node::Remove(key, root_, cmp);
  }

  Comparable* Get(const KeyType key, CompareResult cmp = kEqCmp) const {
    return Node::Get(key, root_, cmp);
  }

  Comparable* GetLowerNearest(const KeyType key) const {
    Node* last_node_lt_key = NULL;
    Node* n = root_;

    while (n != NULL) {
      if (n->item->Key() == key) {
        return n->item;
      } else if (n->item->Key() < key) {
        last_node_lt_key = n;
        n = n->Right();
      } else {
        n = n->Left();
      }
    }
    if (last_node_lt_key == NULL) {
      return NULL;
    } else {
      return last_node_lt_key->item;
    }
  }

  bool IsBalanced() const {
    if (root_ == NULL) {
      return true;
    }
    int diff = abs(Height(root_->Left()) - Height(root_->Right()));
    return diff <= 1;
  }

  bool IsEmpty() const {
    return root_ == NULL;
  }

 private:
  enum HeightEffect {
    kHeightNoChange = 0,
    kHeightChange = 1
  };

  int Height(Node* n) const {
    if (n == NULL) {
      return 0;
    }
    int l = Height(n->Left());
    int r = Height(n->Right());
    return Node::max(l, r) + 1;
  }

  Node* root_;
};

#endif  // AVL_TREE_H_
