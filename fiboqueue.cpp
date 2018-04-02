#include "fiboqueue.h"

using namespace std;

// constructors
	FibQueue::FibQueue() : FibHeap<T>() {}
// destructor
	FibQueue::~FibQueue() {}
// accessors
	FibQueue::find(T k) {
		typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator mit = fstore.find(k);
		return mit;
	}
	FibQueue::findNode(T k) {
		typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator mit = find(k);
		return mit->second;
	}
// modifiers
	FibQueue::decrease_key(typename FibHeap<T>::FibNode* x, int k) {
		typename unordered_map<T, typename FibHeap<T>::FibNode*>::iterator mit = find(x->key);
		fstore.erase(mit);
		fstore.insert(pair<T, typename FibHeap<T>::FibNode*>(k, x));
		FibHeap<T>::decrease_key(x, k);
	}
	FibQueue::pop() {
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
	FibQueue::push(T k, void* pl) {
		typename FibHeap<T>::FibNode* x = FibHeap<T>::push(k, pl);
		fstore.insert(pair<T, typename FibHeap<T>::FibNode*>(k, x));
		return x;
	}
	FibQueue::push(T k) { return push(k, NULL); }