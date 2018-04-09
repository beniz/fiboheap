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

#ifndef FIBOHEAP_HPP
#define FIBOHEAP_HPP

#include <array>
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <limits>
#include <iostream>
#include <utility>
#include <vector>
/*
#include "../named_types/includes/named_types/named_tuple.hpp"
namespace {
	template <typename T, T... chars>s constexpr named_tag<string_literal<T,chars...>> operator ""_t () { return {}; }
}
*/
template<class T> class FibHeap {
	/* attributes */
		public:
			named_types::named_tuple<T(key), bool(mark)> Node2;
			struct Node {
					T key;
					bool mark;
					Node* p, left, right, child;
					int degree;
					void* payload;
					Node() = delete;
					Node(T k, void* pl) : key(k), mark(false), p(nullptr), left(nullptr), right(nullptr), child(nullptr), degree(-1), payload(pl) {}
					~Node() noexcept {}
			};
			friend class Node;
		private:
			size_t n;
			Node* min;
	/* members */
		public:
			// constructors
				FibHeap() : n(0), min(nullptr) {}
				/*
				Foo(const Foo& other);
				Foo(Foo&& other) noexcept;
				*/
			// destructor
				~FibHeap() noexcept { delete_Nodes(min); }
			// operators
				/*
				Foo& operator=(const Foo& other);
				Foo& operator=(Foo&& other) noexcept;
				*/
			// getters
				bool empty() const noexcept { return n == 0; }
				size_t size() const noexcept { return n; }
			// accessors
				Node* topNode() { return minimum(); }
				T top() const { return minimum()->key; }
				/*
				 * The minimum node of the heap.
				 */
				Node* minimum() { return min; }
				/*
				 * extract_min
				 * 1. z = H.min
				 * 2. if z != NIL
				 * 3. 	for each child x of z
				 * 4. 		add x to the root list of H
				 * 5. 		x.p = NIL
				 * 6. 	remove z from the root list of H
				 * 7.		if z == z.right
				 * 8. 		H.min = NIL
				 * 9. 	else H.min = z.right
				 *10. 		CONSOLIDATE(H)
				 *11. 	H.n = H.n - 1
				 *12. return z
				 */
				Node* extract_min() {
					Node* z = min, x, next;

					if(z != nullptr) {
						x = z->child;
						if(x != nullptr) {
							std::array<Node*, z->degree> childList;
							next = x;
							for(auto& element : childList) {
								element = next;
								next = next->right;
							}
							for(int i = 0; i < z->degree; ++i) {
								min->left->right = x = childList.at(i);
								x->left = min->left;
								min->left = x;
								x->right = min;
								x->p = nullptr;
							}
							delete[] childList;
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
					Node* x = extract_min();
					if(x)
						delete x;
				}
				/*
				 * insert(x)
				 * 1. x.degree = 0
				 * 2. x.p = NIL
				 * 3. x.child = NIL
				 * 4. x.mark = FALSE
				 * 5. if H.min == NIL
				 * 6. 	create a root list for H containing just x
				 * 7. 	H.min = x
				 * 8. else insert x into H's root list
				 * 9. 	if x.key < H.min.key
				 *10. 		H.min = x
				 *11. H.n = H.n + 1
				 */
				void insert(Node* x) {
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
				}
				/*
				 * union_fibheap(H1, H2)
				 * 1. H = MAKE-FIB-HEAP()
				 * 2. H.min = H1.min
				 * 3. concatenate the root list of H2 with the root list of H
				 * 4. if (H1.min == NIL) or (H2.min != NIL and H2.min.key < H1.min.key)
				 * 5. 	H.min = H2.min
				 * 6. H.n = H1.n + H2.n
				 * 7. return H
				 */
				static FibHeap* union_fibheap(FibHeap* H1, FibHeap* H2) {
					FibHeap* H = new FibHeap();
					H->min = H1->min;
					if(H->min != nullptr && H2->min != nullptr) {
						H->min->right->left = H2->min->left;
						H2->min->left->right = H->min->right;
						H->min->right = H2->min;
						H2->min->left = H->min;
					}
					if(H1->min == nullptr || (H2->min != nullptr && H2->min->key < H1->min->key))
						H->min = H2->min;
					H->n = H1->n + H2->n;
					return H;
				}
				/*
				 * cut(x,y)
				 * 1. remove x from the child list of y, decrementing y.degree
				 * 2. add x to the root list of H
				 * 3. x.p = NIL
				 * 4. x.mark = FALSE
				 */
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
				/*
				 * cascading_cut(y)
				 * 1. z = y.p
				 * 2. if z != NIL
				 * 3. 	if y.mark == FALSE
				 * 4. 		y.mark = TRUE
				 * 5. 	else CUT(H,y,z)
				 * 6. 		CASCADING-CUT(H,z)
				 */
				void cascading_cut(Node* y) {
					Node* z = y->p;
					if(z != nullptr) {
						if(!y->mark)
							y->mark = true;
						else {
							cut(y, z);
							cascading_cut(z);
						}
					}
				}
				/*
				 * set to infinity so that it hits the top of the heap, then easily remove.
				 */
				void remove_Node(Node* x) {
					decrease_key(x, numeric_limits<T>::min());
					delete extract_min();
				}
				/*
				 * fib_heap_link(y,x)
				 * 1. remove y from the root list of heap
				 * 2. make y a child of x, incrementing x.degree
				 * 3. y.mark = FALSE
				 */
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
				Node* push(const T& k, void* pl) {
					Node* x = new Node(k, pl);
					insert(x);
					return x;
				}
				Node* push(const T& k) { return push(k, nullptr); }
				Node* push(T&& k, void* pl) {
					Node* x = new Node(k, pl);
					insert(x);
					k = nullptr;
					return x;
				}
				Node* push(T&& k) { return push(k, nullptr); }
				/*
				 * decrease_key(x,k)
				 * 1. if k > x.key
				 * 2. 	error "new key is greater than current key"
				 * 3. x.key = k
				 * 4. y = x.p
				 * 5. if y != NIL and x.key < y.key
				 * 6. 	CUT(H,x,y)
				 * 7. 	CASCADING-CUT(H,y)
				 * 8. if x.key < H.min.key
				 * 9. 	H.min = x
				 */
				void decrease_key(Node* x, int k) {
					if(x->key < k)
						return; // error("new key is greater than current key");
					x->key = k;
					Node* y = x->p;
					if(y != nullptr && x->key < y->key) {
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
			/*
			 * consolidate
			 * 1. let A[0 . . D(H.n)] be a new array
			 * 2. for i = 0 to D(H.n)
			 * 3. 	A[i] = NIL
			 * 4. for each node w in the root list of H
			 * 5. 	x = w
			 * 6. 	d = x.degree
			 * 7. 	while A[d] != NIL
			 * 8. 		y = A[d]
			 * 9. 		if x.key > y.key
			 *10.			exchange x with y
			 *11. 		FIB-HEAP-LINK(H,y,x)
			 *12. 		A[d] = NIL
			 *13. 		d = d + 1
			 *14. 	A[d] = x
			 *15. H.min = NIL
			 *16. for i = 0 to D(H.n)
			 *17. 	if A[i] != NIL
			 *18. 		if H.min == NIL
			 *19. 			create a root list for H containing just A[i]
			 *20. 			H.min = A[i]
			 *21. 		else insert A[i] into H's root list
			 *22. 			if A[i].key < H.min.key
			 *23. 				H.min = A[i]
			 */
			void consolidate() {
				Node* w, next, x, y;
				int d, rootSize, max_degree = static_cast<int>(
					floor(
						log(static_cast<double>(n)) / log(0.5 * (1.0 + sqrt(5.0)))
					) + 2
				);

				vector<Node*> A(max_degree);
				fill_n(A, max_degree, nullptr);
				rootSize = 0;
				next = w = min;
				do {
					++rootSize;
					next = next->right;
				} while(next != w);

				vector<Node*> rootList(rootSize);
				foreach(rootList.begin(), rootList.end(),
					[](auto& element) {
						element = element->right;
					}
				);

				for(const auto& element : rootList) {
					x = w = element;
					d = x->degree;
					while(A.at(d) != nullptr) {
						y = A.at(d);
						if(y->key < x->key)
							x = x + y - (y = x);
						fib_heap_link(y, x);
						A.at(d) = nullptr;
						++d;
					}
					A.at(d) = x;
				}
				delete[] rootList;

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
							if(element->key < min->key)
								min = element;
						}
					}
				}
				delete[] A;
			}
	};

#endif