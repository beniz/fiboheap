#include "fiboheap.h"

using namespace std;

// nested class
	FibHeap::FibNode {
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
// public
	// constructors
		FibHeap::FibHeap() : n(0), min(nullptr) {}
	// destructor
		FibHeap::~FibHeap() { delete_fibnodes(min); }
	// getters
		FibHeap::empty() const { return n == 0; }
		FibHeap::size() const { return n; }
	// accessors
		FibHeap::topNode() { return minimum(); }
		FibHeap::top() { return minimum()->key; }
		FibHeap::minimum() { return min; }
		FibHeap::extract_min() {
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
						min->left->right = x = childList[i];
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
		FibHeap::pop() {
			if(empty())
				return;
			FibNode* x = extract_min();
			if(x)
				delete x;
		}
		FibHeap::insert(FibNode* x) {
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
		FibHeap::union_fibheap(FibHeap* H1, FibHeap* H2) {
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
		FibHeap::cut(FibNode* x, FibNode* y) {
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
		FibHeap::cascading_cut(FibNode* y) {
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
		FibHeap::remove_fibnode(FibNode* x) {
			decrease_key(x, numeric_limits<T>::min());
			FibNode* fn = extract_min();
			delete fn;
		}
		FibHeap::fib_heap_link(FibNode* y, FibNode* x) {
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
		FibHeap::push(T k, void* pl) {
			FibNode* x = new FibNode(k, pl);
			insert(x);
			return x;
		}
		FibHeap::push(T k) { return push(k, nullptr); }
		FibHeap::decrease_key(FibNode* x, int k) {
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
// protected
	FibHeap::delete_fibnodes(FibNode* x) {
		if(!x)
			return;
		FibNode* cur = x;
		while(true) {
			/*
			 * cerr << "cur: " << cur << endl;
			 * cerr << "x: " << x << endl;
			 */
			if (cur->left && cur->left != x) {
				// cerr << "cur left: " << cur->left << endl;
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
	FibHeap::consolidate() {
		FibNode* w, next, x, y;
		FibNode** A, rootList; // std::array
		int d, rootSize;
		int max_degree = static_cast<int>(floor(log(static_cast<double>(n)) / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2)));

		A = new FibNode*[max_degree + 2];
		fill_n(A, max_degree + 2, nullptr);
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
				if(y->key < x->key)
					x = x + y - (y = x);
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