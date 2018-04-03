#include "fiboqueue.h"

using namespace std;

// constructors
	template<class T> FibQueue<T>::FibQueue() : FibHeap<T>() {}
// destructor
	template<class T> FibQueue<T>::~FibQueue() {}
// accessors
	template<class T> typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator FibQueue<T>::find(T k) {
		typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator mit = fstore.find(k);
		return mit;
	}
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::findNode(T k) {
		typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator mit = find(k);
		return mit->second;
	}
// modifiers
	template<class T> void  FibQueue<T>::decrease_key(typename FibHeap<T>::FibNode* x, int k) {
		typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator mit = find(x->key);
		fstore.erase(mit);
		fstore.insert(pair<T, typename FibHeap<T>::FibNode*>(k, x));
		FibHeap<T>::decrease_key(x, k);
	}
	template<class T> void  FibQueue<T>::pop() {
		if (FibHeap<T>::empty())
			return;
		typename FibHeap<T>::FibNode* x = FibHeap<T>::extract_min();
		if(!x)
			return; // should not happen.
		auto range = fstore.equal_range(x->key);
		auto mit = find_if(range.first, range.second,
			[x](const pair<T, typename FibHeap<T>::FibNode*> &ele) {
				return ele.second == x;
			}
		);
		if(mit != range.second)
			fstore.erase(mit);
		else cerr << "[Error]: key " << x->key << " cannot be found in FiboQueue fast store" << endl;
		delete x;
	}
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::push(T k, void* pl) {
		typename FibHeap<T>::FibNode* x = FibHeap<T>::push(k, pl);
		fstore.insert(pair<T, typename FibHeap<T>::FibNode*>(k, x));
		return x;
	}
	template<class T> typename FibHeap<T>::FibNode* FibQueue<T>::push(T k) { return push(k, NULL); }