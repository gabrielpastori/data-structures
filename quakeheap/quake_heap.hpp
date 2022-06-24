// Copyright (c) 2022

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include "./quake_heap.h"
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::to_string;

template<class T1, class T2>
QuakeHeap<T1, T2>::Node::Node(T1 key, T2 value) {
  this -> key = key;
  this -> value = value;
  this -> level = 0;
  this -> left = nullptr;
  this -> right = nullptr;
  this -> parent = nullptr;
}

template<class T1, class T2>
QuakeHeap<T1, T2>::Node::Node(T1 key, Node* u, Node* v) {
  this -> key = key;
  this -> value = u->value;
  this -> level = u->level+1;
  this -> left = u;
  this -> right = v;
  this -> parent = nullptr;
}

template<class T1, class T2>
QuakeHeap<T1, T2>::Locator::Locator(QuakeHeap<T1, T2>::Node* u) {
  this -> u = u;
}

template<class T1, class T2>
typename QuakeHeap<T1, T2>::Node* QuakeHeap<T1, T2>::Locator::GetNode() {
  return this->u;
}

template<class T1, class T2>
QuakeHeap<T1, T2>::QuakeHeap(int n_levels) {
  for (int level = 0; level < n_levels; level++) {
    (this -> node_counter).push_back(0);
    list<Node*>* new_list = new list<Node*>();
    (this -> roots).push_back(new_list);
  }
}

template<class T1, class T2>
void QuakeHeap<T1, T2>::Clear() {
  for (int level = 0; level < roots.size(); level++) {
    roots[level]->clear();
    node_counter[level] = 0;
  }
}

template<class T1, class T2>
typename QuakeHeap<T1, T2>::Locator* QuakeHeap<T1, T2>::Insert(
  T1 key,
  T2 value
) {
  QuakeHeap<T1, T2>::Node* new_node = new QuakeHeap<T1, T2>::Node(key, value);
  node_counter[0]+=1;
  roots[0]->push_back(new_node);
  QuakeHeap::Locator* new_locator = new QuakeHeap<T1, T2>::Locator(new_node);
  return new_locator;
}

template<class T1, class T2>
int QuakeHeap<T1, T2>::GetMaxLevel(Locator* ptr_loc) {
  QuakeHeap::Node* base_node = ptr_loc->GetNode();
  while (base_node->parent != nullptr &&
  (base_node->parent)->key == base_node->key) {
    base_node = base_node->parent;
  }

  return base_node->level;
}

template<class T1, class T2>
T1 QuakeHeap<T1, T2>::GetMinKey() {
  T1 min_answer;
  bool visited = false;
  for (int level = 0; level < roots.size(); level++) {
    if (!roots[level]->size()) continue;
    for (auto it = roots[level]->begin(); it != roots[level]->end(); ++it) {
        if (!visited) {
          min_answer = (*it)->key;
          visited = true;
          continue;
        }
        min_answer = std::min(min_answer, (*it)->key);
      }
    }
  if (!visited) throw std::runtime_error("Empty heap");
  MergeTrees();
  return min_answer;
}

template<class T1, class T2>
void QuakeHeap<T1, T2>::MergeTrees() {
  for (int level = 0; level < (roots.size() - 1); level++) {
    if (!roots[level]->size()) continue;
    roots[level]->sort([](Node * lhs, Node * rhs) {
      return lhs->key < rhs->key;
    });

    while (roots[level]->size() >= 2) {
      auto u = roots[level]->begin();
      auto v = std::next(roots[level]->begin());
      QuakeHeap<T1, T2>::Node* w = new QuakeHeap<T1, T2>
        ::Node((*u)->key, *u, *v);
      (*u)->parent = w;
      (*v)->parent = w;

      node_counter[level+1]++;
      roots[level+1]->push_back(w);
      roots[level]->pop_front();
      roots[level]->pop_front();
    }
  }
}

template<class T1, class T2>
void QuakeHeap<T1, T2>::Preorder(Locator* u, vector<string> *response) {
  QuakeHeap<T1, T2>::Node* node_u = u->GetNode();
  if (node_u == nullptr) {
    response -> push_back("[null]");
    return;
  }
  if (node_u->level >= 1) {
    std::stringstream fmt;
    fmt << "(" << node_u->key << ")";
    response->push_back(fmt.str());
    QuakeHeap::Locator* left_node_locator =
      new QuakeHeap<T1, T2>::Locator(node_u->left);
    QuakeHeap::Locator* right_node_locator =
      new QuakeHeap<T1, T2>::Locator(node_u->right);

    QuakeHeap::Preorder(left_node_locator, response);
    QuakeHeap::Preorder(right_node_locator, response);

  } else if (node_u->level == 0) {
    std::stringstream fmt;
    fmt << "[" << node_u->key << " " << node_u->value << "]";
    response->push_back(fmt.str());
    return;
  }
}

template<class T1, class T2>
vector<string> QuakeHeap<T1, T2>::ListHeap() {
  vector<string> response;
  std::stringstream fmt;
  for (int level = 0; level <= (roots.size() - 1); level++) {
    if (!node_counter[level]) continue;
    roots[level]->sort([](Node * lhs, Node * rhs) {
      return lhs->key < rhs->key;
    });
    fmt << "{lev: " << level << " nodeCt: " << node_counter[level] << "}";
    response.push_back(fmt.str());
    fmt.str(std::string());

    for (auto it = roots[level]->begin(); it != roots[level]->end(); ++it) {
      QuakeHeap::Locator* root_locator =
      new QuakeHeap<T1, T2>::Locator(*it);

      QuakeHeap<T1, T2>::Preorder(root_locator, &response);
    }
  }
  return response;
}
