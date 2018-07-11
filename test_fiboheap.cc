/**
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

#include "fiboheap.h"
#include "fiboqueue.h"
#include <stdlib.h>
#include <assert.h>
#include <queue>

using namespace std;

struct lowerI {
	bool operator()(const int& d1, const int& d2) { return d2 < d1; }
};

void fill_heaps(FibHeap<int>& fh, priority_queue<int, vector<int>, lowerI>& pqueue, const int& n) {
	for(unsigned int i = 0; i < n; ++i) {
		int r = rand();
		fh.push(r);
		pqueue.push(r);
		// pqueue.push((fh.push(rand()))->key));
	}
	assert(fh.size() == n);
	assert(pqueue.size() == n);
}

bool match_heaps(FibHeap<int>& fh, priority_queue<int, vector<int>, lowerI>& pqueue) {
	while(!pqueue.empty()) {
		int i1 = pqueue.top(), i2 = fh.top();
		//cerr << "i1: " << i1 << " -- i2: " << i2 << endl;
		assert(i1 == i2);
		pqueue.pop();
		fh.pop();
	}
	assert(fh.empty());
}

void fill_queues(FibQueue<int>& fh, priority_queue<int, vector<int>,lowerI>& pqueue, const int& n) {
	for(unsigned int i = 0; i < n; ++i) {
		int r = rand();
		fh.push(r);
		pqueue.push(r);
		// pqueue.push((fh.push(rand()))->key));
	}
	assert(fh.size() == n);
	assert(pqueue.size() == n);
}

bool match_queues(FibQueue<int>& fh, priority_queue<int, vector<int>, lowerI>& pqueue) { // greater<int> ?
	while(!pqueue.empty()) {
		int i1 = pqueue.top(), i2 = fh.top();
		//cerr << "i1: " << i1 << " -- i2: " << i2 << endl;
		assert(i1 == i2);
		pqueue.pop();
		fh.pop();
	}
	assert(fh.empty());
}

int main(int argc, char *argv[]) {
	FibHeap<int> fh;
	unsigned int n = 10;
	std::priority_queue<int, vector<int>, lowerI> pqueue;

	//srand(time(0));

	fill_heaps(fh, pqueue, n);
	assert(match_heaps(fh, pqueue));

	fill_heaps(fh, pqueue, n);
	//cerr << "top pqueue: " << pqueue.top() << " -- top fh: " << fh.top() << endl;
	int r = pqueue.top()-1;
	//cerr << "old val: " << pqueue.top() << " -- new val: " << r << endl;
	pqueue.pop();
	pqueue.push(r);
	make_heap(const_cast<int*>(&pqueue.top()), const_cast<int*>(&pqueue.top()) + pqueue.size(), lowerI());
	fh.decrease_key(fh.topNode(), r);
	assert(match_heaps(fh, pqueue));

	FibQueue<int> fq;
	fill_queues(fq, pqueue, n);
	match_queues(fq, pqueue);

	fill_queues(fq, pqueue, n);
	r = rand();
	fq.push(r);
	FibHeap<int>::FibNode* x = fq.findNode(r);
	assert(x != NULL);
	int nr = r - rand() / 2;
	fq.decrease_key(x, nr);
	pqueue.push(nr);
	match_queues(fq, pqueue);
}
