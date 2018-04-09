#include "fiboheap.h"

using namespace std;

// protected
	template<class T> void FibHeap<T>::delete_Nodes(Node* x) {
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
	template<class T> void FibHeap<T>::consolidate() {
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