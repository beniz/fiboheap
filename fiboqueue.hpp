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
 * vertices, and decrease their key as needed. Useful for search algorithms (e.g.
 * Dijstra, heuristic, ...).
 */

#ifndef FIBOQUEUE_HPP
#define FIBOQUEUE_HPP

#include "fiboheap.hpp"
#include <unordered_map>
#include <algorithm>

template<class T> class FibQueue : public FibHeap<T> {
	/* atributes */
		private:
			std::unordered_multimap<T, typename FibHeap<T>::FibNode*> fstore;
	/* members */
		public:
			// constructors
				FibQueue() {}
				/*
				Foo(const Foo& other);
				Foo(Foo&& other) noexcept;
				*/
			// destructor
				~FibQueue() noexcept {}
			// operators
				/*
				Foo& operator=(const Foo& other);
				Foo& operator=(Foo&& other) noexcept;
				*/
			// accessors
				auto find(T k) { return fstore.find(k); }
				typename FibHeap<T>::FibNode* findNode(T k) { return find(k)->second; }
			// modifiers
				void decrease_key(typename FibHeap<T>::FibNode* x, int k) {
					fstore.erase(find(x->key));
					fstore.emplace(k, x);
					FibHeap<T>::decrease_key(x, k);
				}
				void pop() {
					if(FibHeap<T>::empty())
						return;
					typename FibHeap<T>::FibNode* x = FibHeap<T>::extract_min();
					auto range = fstore.equal_range(x->key), mit = find_if(range.first, range.second,
						[x](const auto& ele) {
							return ele.second == x;
						}
					);
					if(mit != range.second)
						fstore.erase(mit);
					else
						std::cerr << "[Error]: key " << x->key << " cannot be found in FiboQueue fast store" << std::endl;
					delete x;
				}
				typename FibHeap<T>::FibNode* push(const T& k, void* pl) {
					auto x = FibHeap<T>::push(k, pl);
					fstore.emplace(k, x);
					return x;
				}
				typename FibHeap<T>::FibNode* push(const T& k) { return push(k, nullptr); }
				typename FibHeap<T>::FibNode* push(T&& k, void* pl) {
					auto x = FibHeap<T>::push(k, pl);
					fstore.emplace(k, x);
					k = nullptr;
					return x;
				}
				typename FibHeap<T>::FibNode* push(T&& k) { return push(k, nullptr); }
};

#endif