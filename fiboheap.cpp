#include "fiboheap.h"

using namespace std;

// public
	// constructors
		template<class T> FibHeap<T>::FibHeap() : n(0), min(nullptr) {}
	// destructor
		template<class T> FibHeap<T>::~FibHeap() noexcept { delete_fibnodes(min); }
	// getters
		template<class T> bool FibHeap<T>::empty() const noexcept { return n == 0; }
		template<class T> size_t FibHeap<T>::size() const noexcept { return n; }
	// accessors
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::topNode() { return minimum(); }
		template<class T> T FibHeap<T>::top() const { return minimum()->key; }
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::minimum() { return min; }
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::extract_min() {
			FibNode* z, x, next;

			z = min;
			if(z != nullptr) {
				x = z->child;
				if(x != nullptr) {
					array<FibNode*, z->degree> childList;
					next = x;
					for(auto& element : childList) {
						element = next;
						next = next->right;
					}
					for(int i = 0; i < z->degree; ++i) {
						min->left->right = x = childList.at(i);
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
		template<class T> void FibHeap<T>::pop() {
			if(empty())
				return;
			FibNode* x = extract_min();
			if(x)
				delete x;
		}
		template<class T> void FibHeap<T>::insert(FibNode* x) {
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
		template<class T> typename FibHeap<T>::FibHeap* FibHeap<T>::union_fibheap(FibHeap* H1, FibHeap* H2) {
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
		template<class T> void FibHeap<T>::cut(FibNode* x, FibNode* y) {
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
		template<class T> void FibHeap<T>::cascading_cut(FibNode* y) {
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
		template<class T> void FibHeap<T>::remove_fibnode(FibNode* x) {
			decrease_key(x, numeric_limits<T>::min());
			delete extract_min();
		}
		template<class T> void FibHeap<T>::fib_heap_link(FibNode* y, FibNode* x) {
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
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::push(const T& k, void* pl) {
			FibNode* x = new FibNode(k, pl);
			insert(x);
			return x;
		}
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::push(const T& k) { return push(k, nullptr); }
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::push(T&& k, void* pl) {
			FibNode* x = new FibNode(k, pl);
			insert(x);
			k = nullptr;
			return x;
		}
		template<class T> typename FibHeap<T>::FibNode* FibHeap<T>::push(T&& k) { return push(k, nullptr); }
		template<class T> void FibHeap<T>::decrease_key(FibNode* x, int k) {
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
	template<class T> void FibHeap<T>::delete_fibnodes(FibNode* x) {
		if(!x)
			return;
		FibNode* cur = x;
		while(true) {
			// cerr << "cur: " << cur << endl << "x: " << x << endl;
			if(cur->left && cur->left != x) {
				// cerr << "\tcur left: " << cur->left << endl;
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
	template<class T> void FibHeap<T>::consolidate() {
		FibNode* w, next, x, y;
		int d, rootSize, max_degree = static_cast<int>(
			floor(
				log(static_cast<double>(n)) / log(0.5 * (1.0 + sqrt(5.0)))
			) + 2
		);

		vector<FibNode*> A(max_degree);
		fill_n(A, max_degree, nullptr);
		rootSize = 0;
		next = w = min;
		do {
			++rootSize;
			next = next->right;
		} while(next != w);

		vector<FibNode*> rootList(rootSize);
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