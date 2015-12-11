// g++ -std=c++11 -c IndexedHeap.cpp -pg

// IndexedHeap.cpp
#ifndef _INDEXEDHEAP_CPP
#define _INDEXEDHEAP_CPP

#include "IndexedHeap.hpp"
#include <utility>
#include <iostream>

IndexedHeap::IndexedHeap(){
	size = 0;
}

IndexedHeap::~IndexedHeap(){}

// return and delete min from heap
// and set index = -1
Pix* IndexedHeap::pop(){
	if(size == 0)
		return NULL;
	
	Pix * ret = heap[0]; // element to return

	// first swap min element with last element in array
	// then swap new first element with last element in heap
	// decrement size by 1 and bubble down first element
	std::swap(heap[0], heap.back());

	heap[0]->index = 0;
	heap[heap.size() - 1]->index = -1;
	heap.pop_back();

	// swap new first element with last element in heap
	std::swap(heap[0], heap[size-1]);

	heap[0]->index = 0;
	heap[size - 1]->index = size - 1;

	size--;
	bubble_down(0);

	return ret;
}

// return min from heap
// but retain it in array somewhere
Pix* IndexedHeap::pop_hide(){
	Pix * ret = heap[0]; // element to return

	// swap min with last element in heap
	// decrement size by 1
	// bubble_down
	std::swap(heap[0], heap[size - 1]);

	heap[0]->index = 0;
	heap[size - 1]->index = size - 1;

	size--;
	bubble_down(0);

	return ret;
}

void IndexedHeap::bubble_down(int i){
	int leftChild = 2 * i + 1;
  int rightChild = 2 * i + 2;
  int min = i;

  // find who holds smallest element of i and its two children
  if(leftChild < size && *heap[leftChild] < *heap[min])
    min = leftChild;
  if(rightChild < size && *heap[rightChild] < *heap[min])
    min = rightChild;

  // if a child holds smallest element, swap i's element to that child
  // and recurse.
  if(min != i) {
  	std::swap(heap[i], heap[min]);
  	heap[i]->index = i;
  	heap[min]->index = min;

    bubble_down(min);
  }
}

// initial buildup of array
// call heapify after finished calling push
// should not be called after restart
void IndexedHeap::push(Pix * element){
	heap.push_back(element);
	heap.back()->index = heap.size() - 1;
	size++;
}

// push into existing heap
void IndexedHeap::push_heap(Pix * element){
	// place element at back
	// swap with first element not in heap
	// increase size of heap by 1
	// bubble up
	heap.push_back(element);
	std::swap(heap.back(), heap[size++]);

	heap[heap.size() - 1]->index = heap.size() - 1;
	heap[size - 1]->index = size - 1;

	bubble_up(heap.size() - 1);
}

// heapify current heap bounds
void IndexedHeap::heapify(){
	for(int i = (size - 2) / 2; i >= 0; i--)
		bubble_down(i);
}

void IndexedHeap::bubble_up(int i){
	if(i == 0)
		return;

	int parent = (i-1)/2;

  if(*heap[i] < *heap[parent]) {
  	std::swap(heap[i], heap[parent]);
  	heap[i]->index = i;
  	heap[parent]->index = parent;

    bubble_up(parent);
  }
}

void IndexedHeap::decrease_key(Pix * element, int new_dist){
	element->dist = new_dist;
	bubble_up(element->index);
}

// make whole array heap again
void IndexedHeap::restart(){
	size = heap.size();
	heapify();
}

// remove element from array completely
// set index to -1
Pix* IndexedHeap::pop_element(Pix * element){
	// swap element with last heap element
	// bubble up and bubble down swapped element
	// swap element to remove with last element in array
	// heap.pop_back()

	// swap element with last heap element and size--
	std::swap(heap[element->index], heap[--size]);
	heap[element->index]->index = element->index;

	// swapped element: bubble up and down
	bubble_up(element->index);
	bubble_down(element->index);

	// swap element to remove with last element in array
	std::swap(heap[size], heap.back());
	heap[size]->index = size;

	heap[heap.size() - 1]->index = -1;
	heap.pop_back();
}


#endif