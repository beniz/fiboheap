#include "fiboqueue.h"

using namespace std;

// constructors
	template<class T> FibQueue<T>::FibQueue() : FibHeap<T>() {}
// destructor
	template<class T> FibQueue<T>::~FibQueue() {}
// accessors
	template<class T> auto FibQueue<T>::find(T k) { return fstore.find(k); }
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::findNode(T k) { return find(k)->second; }
// modifiers
	template<class T> void  FibQueue<T>::decrease_key(typename FibHeap<T>::FibNode* x, int k) {
		fstore.erase(find(x->key));
		fstore.emplace(k, x);
		FibHeap<T>::decrease_key(x, k);
	}
	template<class T> void FibQueue<T>::pop() {
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
			cerr << "[Error]: key " << x->key << " cannot be found in FiboQueue fast store" << endl;
		delete x;
	}
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::push(const T& k, void* pl) {
		auto x = FibHeap<T>::push(k, pl);
		fstore.emplace(k, x);
		return x;
	}
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::push(const T& k) { return push(k, nullptr); }
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::push(T&& k, void* pl) {
		auto x = FibHeap<T>::push(k, pl);
		fstore.emplace(k, x);
		k = nullptr;
		return x;
	}
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::push(T&& k) { return push(k, nullptr); }