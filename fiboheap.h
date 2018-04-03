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
#include <array>
#include <algorithm>
#include <cstddef>
#include <math.h>
#include <limits>
#include <iostream>
#include <vector>

template<class T>class FibHeap {
	/* attributes */
		public:
			struct FibNode {
				/* attributes */
					T key;
					bool mark;
					FibNode* p, left, right, child;
					int degree;
					void* payload;
				/* members */
					FibNode(T k, void* pl) : key(k), mark(false), p(nullptr), left(nullptr), right(nullptr), child(nullptr), degree(-1), payload(pl) {}
					~FibNode() noexcept {}
			};
			friend class FibNode;
		private:
			size_t n;
			FibNode* min;
	/* members */
		public:
			// constructors
				FibHeap();
			// destructor
				~FibHeap() noexcept;
			// getters
				bool empty() const noexcept;
				size_t size() const noexcept;
			// accessors
				FibNode* topNode();
				T top() const;
				/*
				 * The minimum node of the heap.
				 */
				FibNode* minimum();
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
				FibNode* extract_min();
			// modifiers
				void pop();
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
				void insert(FibNode* x);
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
				static FibHeap* union_fibheap(FibHeap* H1, FibHeap* H2);
				/*
				 * cut(x,y)
				 * 1. remove x from the child list of y, decrementing y.degree
				 * 2. add x to the root list of H
				 * 3. x.p = NIL
				 * 4. x.mark = FALSE
				 */
				void cut(FibNode* x, FibNode* y);
				/*
				 * cascading_cut(y)
				 * 1. z = y.p
				 * 2. if z != NIL
				 * 3. 	if y.mark == FALSE
				 * 4. 		y.mark = TRUE
				 * 5. 	else CUT(H,y,z)
				 * 6. 		CASCADING-CUT(H,z)
				 */
				void cascading_cut(FibNode* y);
				/*
				 * set to infinity so that it hits the top of the heap, then easily remove.
				 */
				void remove_fibnode(FibNode* x);
				/*
				 * fib_heap_link(y,x)
				 * 1. remove y from the root list of heap
				 * 2. make y a child of x, incrementing x.degree
				 * 3. y.mark = FALSE
				 */
				void fib_heap_link(FibNode* y, FibNode* x);
				FibNode* push(const T& k, void* pl);
				FibNode* push(const T& k);
				FibNode* push(T&& k, void* pl);
				FibNode* push(T&& k);
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
				void decrease_key(FibNode* x, int k);
		protected:
			void delete_fibnodes(FibNode* x);
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
			void consolidate();
	};

#endif