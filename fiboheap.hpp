/**
 * Fibonacci Heap
 * Copyright (c) 2014, Emmanuel Benazera beniz@droidnik.fr, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

// @todo replace Node* by unique_ptr<>

#ifndef FIBOHEAP_HPP
#define FIBOHEAP_HPP

#include <array>
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <exception>
#include <limits>
#include <iostream>
#include <utility>
#include <vector>

#include "named_tuple.h"

template<class T>
class FibHeap {
	/* attributes */
		public:
			struct Node;
			struct Node {
				T key;
				bool mark;
				Node* p; // replace by smart pointers
				Node* left;
				Node* right;
				Node* child;
				int degree; // see if it is possible to replace by unsigned int (and climits)
				void* payload; // replace by std::any
				Node() = delete;
				Node(T k, void* pl) : key(k), mark(false), p(nullptr), left(nullptr), right(nullptr), child(nullptr), degree(-1), payload(pl) {}
				~Node() noexcept {}
			};
		private:
			size_t n;
			Node* min;
	/* members */
		public:
			// constructors
				FibHeap() : n(0), min(nullptr) {
					//auto node = create_node(set<map<set<any>::const_iterator, unsigned int>>(), nullptr);
				}
				FibHeap(const FibHeap& other) : n(other.n), min(other.min) {}
				FibHeap(FibHeap&& other) noexcept : n(other.n), min(other.min) { delete_Nodes(other.min); }
			// destructor
				~FibHeap() noexcept { delete_Nodes(min); }
			// operators
				FibHeap& operator=(const FibHeap& other) {
					n = other.n;
					min = other.min;
				}
				FibHeap& operator=(FibHeap&& other) noexcept {
					n = other.n;
					min = other.min;
					delete_Nodes(other.min);
				}
			// getters
				bool empty() const noexcept { return n == 0; }
				size_t size() const noexcept { return n; }
			// accessors
				Node* topNode() { return minimum(); }
				T top() const { return minimum()->key; }
				Node* minimum() { return min; }
				Node* extract_min() {
					Node* z = min;

					if(Node* x, *next; z != nullptr) {
						if(z->child != nullptr) {
							std::vector<Node*> childList(z->degree);
							next = x = z->child;

							for_each(childList.begin(), childList.end(),
								[](auto& element) {
									element = element->right;
								}
							);

							for(auto i = 0; i < z->degree; ++i) {
								min->left->right = x = childList.at(i);
								x->left = min->left;
								min->left = x;
								x->right = min;
								x->p = nullptr;
							}
						}
						z->left->right = z->right;
						z->right->left = z->left;
						if(z == z->right)
							min = nullptr;
						else {
							min = z->right;
							consolidate();
						}
						--n;
					}
					return z;
				}
			// modifiers
				void pop() {
					if(empty())
						return;
					if(Node* x = extract_min();  x)
						delete x;
				}
				Node* insert(Node* x) {
					x->degree = 0;
					x->child = x->p = nullptr;
					x->mark = false;
					if(min == nullptr)
						min = x->left = x->right = x;
					else {
						min->left->right = x;
						x->left = min->left;
						min->left = x;
						x->right = min;
						if(x->key < min->key)
							min = x;
					}
					++n;
					return x;
				}
				static FibHeap* union_fibheap(FibHeap* H1, FibHeap* H2) {
					FibHeap* H = new FibHeap();
					H->min = H1->min;
					if(H->min != nullptr && H2->min != nullptr) {
						H2->min->left->right = H->min->right;
						H->min->right->left = H2->min->left; // error
						H->min->right = H2->min;
						H2->min->left = H->min;
					}
					if(H1->min == nullptr || (H2->min != nullptr && H2->min->key < H1->min->key))
						H->min = H2->min;
					H->n = H1->n + H2->n;
					return H;
				}
				void cut(Node* x, Node* y) {
					if(x->right == x)
						y->child = nullptr;
					else {
						x->right->left = x->left;
						x->left->right = x->right;
						if(y->child == x)
							y->child = x->right;
					}
					--y->degree;
					min->right->left = x;
					x->right = min->right;
					min->right = x;
					x->left = min;
					x->p = nullptr;
					x->mark = false;
				}
				void cascading_cut(Node* y) {
					if(Node* z = y->p; z != nullptr) {
						if(!y->mark)
							y->mark = true;
						else {
							cut(y, z);
							cascading_cut(z);
						}
					}
				}
				void remove_Node(Node* x) {
					decrease_key(x, std::numeric_limits<T>::min());
					delete extract_min();
				}
				void fib_heap_link(Node* y, Node* x) {
					y->left->right = y->right;
					y->right->left = y->left;
					if(x->child != nullptr) {
						x->child->left->right = y;
						y->left = x->child->left;
						x->child->left = y;
						y->right = x->child;
					}
					else
						y->left = y->right = x->child = y;
					y->p = x;
					++x->degree;
					y->mark = false;
				}
				Node* push(const T& k, void* pl = nullptr) { return insert(new Node(k, pl)); }
				Node* push(T&& k, void* pl = nullptr) {
					Node* x = new Node(k, pl);
					k = nullptr;
					return insert(x);
				}
				void decrease_key(Node* x, int k) {
					try {
						if(x->key < k)
							throw std::out_of_range("new key is greater than current key");
					}
					catch(std::out_of_range& e) {
						e.what();
						return;
					}
					x->key = k;
					if(Node* y = x->p; y != nullptr && x->key < y->key) {
						cut(x, y);
						cascading_cut(y);
					}
					if(x->key < min->key)
						min = x;
				}
		protected:
			void delete_Nodes(Node* x) {
				if(!x)
					return;
				Node* cur = x;
				while(true) {
					// cerr << "cur: " << cur << endl << "x: " << x << endl;
					if(cur->left && cur->left != x) {
						// cerr << "\tcur left: " << cur->left << endl;
						Node* tmp = cur;
						cur = cur->left;
						if(tmp->child)
							delete_Nodes(tmp->child);
						delete tmp;
					}
					else {
						if(cur->child)
							delete_Nodes(cur->child);
						delete cur;
						break;
					}
				}
			}
			void consolidate() {
				Node* w, *next, *x, *y;
				int d, rootSize, max_degree = /*constexpr ?*/ static_cast<int>(
					floor(
						log(static_cast<double>(n)) / log(0.5 * (1.0 + sqrt(5.0)))
					) + 2
				);

				std::vector<Node*> A(max_degree);
				fill(A.begin(), A.end(), nullptr);
				rootSize = 0;
				next = w = min;
				do {
					++rootSize;
					next = next->right;
				} while(next != w);

				std::vector<Node*> rootList(rootSize);
				for_each(rootList.begin(), rootList.end(),
					[](auto& element) {
						element = element->right;
					}
				);

				for(const auto& element : rootList) {
					x = w = element;
					d = x->degree;
					while(A.at(d) != nullptr) {
						y = A.at(d);
						/*
						if(y->key < x->key)
							std::swap(x, y);
						*/
						fib_heap_link(y, x);
						A.at(d) = nullptr;
						++d;
					}
					A.at(d) = x;
				}

				min = nullptr;
				for(const auto& element : A) {
					if(element != nullptr) {
						if(min == nullptr)
							min = element->left = element->right = element;
						else {
							min->left->right = element;
							element->left = min->left;
							min->left = element;
							element->right = min;
							/*
							if(element->key < min->key)
								min = element;
							*/
						}
					}
				}
			}
			auto create_node(T _key, void* _pl) {
				return named_tuple::make_named_tuple(
					NAMED_TUPLE_MEMBER(key, _key),
					NAMED_TUPLE_MEMBER(mark, false),
					NAMED_TUPLE_MEMBER(p, nullptr),
					NAMED_TUPLE_MEMBER(left, nullptr),
					NAMED_TUPLE_MEMBER(right, nullptr),
					NAMED_TUPLE_MEMBER(child, nullptr),
					NAMED_TUPLE_MEMBER(degree, -1),
					NAMED_TUPLE_MEMBER(payload, _pl)
				);
			}
	};

#endif