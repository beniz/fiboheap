/**
 * Fibonacci Queue
 * Copyright (c) 2014, Emmanuel Benazera beniz@droidnik.fr, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * This is basically a Fibonacci heap with an added fast store for retrieving
 * nodes, and decrease their key as needed. Useful for search algorithms (e.g.
 * Dijstra, heuristic, ...).
 */

#ifndef FIBOQUEUE_H
#define FIBOQUEUE_H

#include "fiboheap.h"
#include <unordered_map>
#include <algorithm>

template<class T> class FibQueue : public FibHeap<T> {
	/* atributes */
		private:
			std::unordered_multimap<T, typename FibHeap<T>::FibNode*> fstore;
	/* members */
		public:
			// constructors
				FibQueue();
			// destructor
				~FibQueue();
			// accessors
				auto find(T k);
				typename FibHeap<T>::FibNode* findNode(T k);
			// modifiers
				void decrease_key(typename FibHeap<T>::FibNode* x, int k);
				void pop();
				typename FibHeap<T>::FibNode* push(const T& k, void* pl);
				typename FibHeap<T>::FibNode* push(const T& k);
				typename FibHeap<T>::FibNode* push(T&& k, void* pl);
				typename FibHeap<T>::FibNode* push(T&& k);
};

#endif