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

#ifndef FIBOHEAP_H
#define FIBOHEAP_H

#include <cstddef>
#include <math.h>
#include <limits>
#include <iostream>

template<class T> class FibHeap {
	/* attributes */
		private:
			int n;
			FibNode *min;
			class FibNode {
				public:
					/* attributes */
						T key;
						bool mark;
						FibNode* p, left, right, child;
						int degree;
						void* payload;
					/* members */
						FibNode(T k, void* pl) : key(k), mark(false), p(nullptr), left(nullptr), right(nullptr), child(nullptr), degree(-1), payload(pl) {}
						~FibNode() {}
			};
	/* members */
		public:
			// constructors
				FibHeap(): n(0), min(nullptr) {}
			// destructor
				~FibHeap() { delete_fibnodes(min); }
			// getters
				bool empty() const { return n == 0; }
				size_t size() { return (size_t) n; }
			// accessors
				FibNode* topNode() { return minimum(); }
				T top() { return minimum()->key; }
				/*
				 * The minimum node of the heap.
				 */
				FibNode* minimum() { return min; }
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
				FibNode* extract_min() {
					FibNode* z, x, next;
					FibNode** childList; // std::array

					z = min;
					if(z != nullptr) {
						x = z->child;
						if(x != nullptr) {
							childList = new FibNode*[z->degree];
							next = x;
							for(int i = 0; i < (int)z->degree; ++i) {
								childList[i] = next;
								next = next->right;
							}
							for(int i = 0; i < (int)z->degree; ++i) {
								x = childList[i];
								min->left->right = x;
								x->left = min->left;
								min->left = x;
								x->right = min;
								x->p = nullptr;
							}
							delete [] childList;
						}
						z->left->right = z->right;
						z->right->left = z->left;
						if(z == z->right)
							min = nullptr;
						else {
							min = z->right;
							consolidate();
						}
						n--;
					}
					return z;
				}
			// modifiers
				void pop() {
					if(empty())
						return;
					FibNode* x = extract_min();
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
				void insert(FibNode* x) {
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
				 * union_fibheap(H1,H2)
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
				void cut(FibNode* x, FibNode* y) {
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
				void cascading_cut(FibNode* y) {
					FibNode* z;
					z = y->p;
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
				void remove_fibnode(FibNode* x) {
					decrease_key(x, std::numeric_limits<T>::min());
					FibNode* fn = extract_min();
					delete fn;
				}
				/*
				 * fib_heap_link(y,x)
				 * 1. remove y from the root list of heap
				 * 2. make y a child of x, incrementing x.degree
				 * 3. y.mark = FALSE
				 */
				void fib_heap_link(FibNode* y, FibNode* x) {
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
				FibNode* push(T k, void* pl) {
					FibNode* x = new FibNode(k, pl);
					insert(x);
					return x;
				}
				FibNode* push(T k) { return push(k, nullptr); }
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
				void decrease_key(FibNode* x, int k) {
					FibNode* y;
					if(x->key < k)
						return; // error("new key is greater than current key");
					x->key = k;
					y = x->p;
					if(y != nullptr && x->key < y->key) {
						cut(x, y);
						cascading_cut(y);
					}
					if(x->key < min->key)
						min = x;
				}
		protected:
			void delete_fibnodes(FibNode* x) {
				if(!x)
					return;
				FibNode* cur = x;
				while(true) {
					/*
					 * std::cerr << "cur: " << cur << std::endl;
					 * std::cerr << "x: " << x << std::endl;
					 */
					if (cur->left && cur->left != x) {
						// std::cerr << "cur left: " << cur->left << std::endl;
						FibNode* tmp = cur;
						cur = cur->left;
						if(tmp->child)
							delete_fibnodes(tmp->child);
						delete tmp;
					}
					else {
						if(cur->child)
							delete_fibnodes(cur->child);
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
				FibNode* w, next, x, y, temp;
				FibNode** A, rootList; // std::array
				int d, rootSize;
				int max_degree = static_cast<int>(floor(log(static_cast<double>(n)) / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2)));

				A = new FibNode*[max_degree + 2];
				std::fill_n(A, max_degree + 2, nullptr);
				w = min;
				rootSize = 0;
				next = w;
				do {
					++rootSize;
					next = next->right;
				} while(next != w);
				rootList = new FibNode*[rootSize];
				for(int i = 0; i < rootSize; ++i) {
					rootList[i] = next;
					next = next->right;
				}
				for(int i = 0; i < rootSize; ++i) {
					w = rootList[i];
					x = w;
					d = x->degree;
					while(A[d] != nullptr) {
						y = A[d];
						if(x->key > y->key) {
							temp = x;
							x = y;
							y = temp;
						}
						fib_heap_link(y, x);
						A[d] = nullptr;
						++d;
					}
					A[d] = x;
				}
				delete[] rootList;
				min = nullptr;
				for(int i = 0; i < max_degree + 2; ++i) {
					if(A[i] != nullptr) {
						if(min == nullptr)
							min = A[i]->left = A[i]->right = A[i];
						else {
							min->left->right = A[i];
							A[i]->left = min->left;
							min->left = A[i];
							A[i]->right = min;
							if(A[i]->key < min->key)
								min = A[i];
						}
					}
				}
				delete[] A;
			}
	};

	#endif