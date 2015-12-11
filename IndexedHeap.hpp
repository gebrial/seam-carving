// IndexedHeap.hpp
#ifndef _INDEXEDHEAP_HPP
#define _INDEXEDHEAP_HPP

#include <vector>
#include "Pix.hpp"

class IndexedHeap{
public:
	IndexedHeap();
	~IndexedHeap();
	Pix* pop();
	Pix* pop_hide();
	Pix* pop_element(Pix * element);
	void push_heap(Pix * element);
	void push(Pix * element);
	void decrease_key(Pix * element, int new_dist);
	void heapify();
	void restart();

private:
	std::vector<Pix *> heap;
	void bubble_down(int i);
	void bubble_up(int i);
	int size;
};

#endif