// compile as:
// g++ -g -std=c++11 -pthread sc.cpp IndexedHeap.o -lX11 -pg
// gprof a.out gmon.out > profiling

#define cimg_display 1
#include "CImg/CImg.h"

using namespace cimg_library;

#include <limits>
#include <vector>
#include <algorithm>
#include <iostream>

#include "IndexedHeap.hpp"

// energy calculation for initial, original image
int calc_energy(CImg<unsigned char> & orig, int x, int y){
	int t1 = 0;
	int t2;
	int x0, x2; // left and right pixels
	int y0, y2; // top and bottom pixels

	int w = orig.width();
	int h = orig.height();
	// i = r, g, or b colour
	for(int i = 0; i < 3; i++){
		x0 = (x-1+w)%w;
		x2 = (x+1)%w;

		t2 = orig(x0, y, i);
		t2 -= orig(x2, y, i);
		t1 += t2*t2;

		y0 = (y-1+h)%h;
		y2 = (y+1)%h;
		t2 = orig(x, y0, i);
		t2 -= orig(x, y2, i);
		t1 += t2*t2;
	}

	return t1;
}



int main(){
	CImg<unsigned char> image("HJoceanSmall.png");
	CImg<int> energy(image.width(), image.height(), 1, 1, 0); // initial energy

	int w = image.width();
	int h = image.height();
	Pix * pixels[w*h]; // index goes across x first, then down y

	// energy calculation
	int ind = 0;
	int max_int = std::numeric_limits<int>::max();
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			energy(x, y) = calc_energy(image, x, y);
			
			Pix* p = new Pix();
			p->x = x;
			p->y = y;
			p->energy = energy(x, y);
			p->dist = max_int/2;
			p->path = NULL;
			p->r = image(x, y, 0);
			p->g = image(x, y, 1);
			p->b = image(x, y, 2);

			pixels[ind] = p;

			ind++;
		}
	}

	ind = 0;
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			Pix * p = pixels[ind];
			p->top = pixels[(ind - w + w*h)%(w*h)];
			p->bottom = pixels[(ind + w)%(w*h)];
			p->right = pixels[(x+1)%w + y*w];
			p->left = pixels[(x-1 + w)%w + y*w];

			ind++;
		}
	}

	// set pixels of first row as source
	for(int x = 0; x < w; x++)
		pixels[x]->dist = pixels[x]->energy;

	IndexedHeap unvisited;
	for(int i = 0; i < w*h; i++){
		unvisited.push(pixels[i]);
	}
	unvisited.heapify();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// need to recalculate energy values before each iteration!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for(int i = 0; i < 5; i++){
		// reset distance vals
		for(int j = 0; j < w*h; j++){
			pixels[j]->dist = max_int/2;
		}

		// set dist values for source nodes
		for(int j = 0; j < w; j++){
			pixels[j]->dist = pixels[j]->energy;
		}

		unvisited.restart();

		Pix * current = unvisited.pop_hide();
		// find "shortest" path
		while(current->y != h-1){
			Pix * child;

			// child directly below
			child = current->bottom;
			if(child->dist > child->energy + current->dist){
				unvisited.decrease_key(child, child->energy + current->dist);
				energy(child->x, child->y) = child->dist;
				child->path = current;
			}

			// child below and left
			child = current->bottom->left;
			if(child->dist > child->energy + current->dist && current->x != 0){
				unvisited.decrease_key(child, child->energy + current->dist);
				energy(child->x, child->y) = child->dist;
				child->path = current;
			}

			// child below and right
			child = current->bottom->right;
			if(child->dist > child->energy + current->dist && current->x != w-1){
				unvisited.decrease_key(child, child->energy + current->dist);
				energy(child->x, child->y) = child->dist;
				child->path = current;
			}

			current = unvisited.pop_hide();
		}

		Pix * final = current;
		int max_dist = current->dist;
		while(current->path != NULL){
			unvisited.pop_element(current);

			energy(current->x, current->y) = max_dist;

			current->left->right = current->right;
			current->right->left = current->left;

			// fix link from top to bottom
			Pix * next = current->top;
			if(current->path != next){
				if(current->path->left == next){
					next->bottom = current->right;
					current->right->top = next;
				} else if (current->path->right == next){
					next->bottom = current->left;
					current->left->top = current->top;
				}
			}

			current = current->path;
		}

		// top element in path
		unvisited.pop_element(current);

		current->left->right = current->right;
		current->right->left = current->left;

		// top element out of place with bottom element in path
		if(current->top->x < final->x){
			current->top->bottom = current->right;
			current->right->top = current->top;

			// what if final is last x?
			current = current->right;
			while(current->top->right->x < final->x && current->top->x < current->top->right->x){
				// loop invariant:
				// right and top are pointing properly, i.e.,
				// 	1. top->bottom = right
				// 	2. right->top = top
				// and if top->right not final then top->right->bottom = this
				// else top->right->bottom = right

				current->top->right->bottom = current->right;
				current->right->top = current->top->right;

				current = current->right;
			}
		} else if(current->top->x > final->x){
			current->top->bottom = current->left;
			current->left->top = current->top;

			// what if final is first x?
			current = current->left;
			while(current->top->left->x > final->x && current->top->x > current->top->left->x){
				current->top->left->bottom = current->left;
				current->left->top = current->top->left;

				current = current->left;
			}
		}
	}

	CImgDisplay main_disp(image, "Original");
	CImgDisplay ener_disp(energy, "Energy", 1);

	while(!main_disp.is_closed() && !ener_disp.is_closed()){
		main_disp.wait();
	}

	return 0;
}