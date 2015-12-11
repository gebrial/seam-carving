//Pix.hpp
#ifndef _PIX_HPP
#define _PIX_HPP

struct Pix{
	int x, y; // position in image
	unsigned char r, g, b; // colours of original image
	int energy, dist; // energy and summed energy in path
	Pix * path; // backwards path to top
	Pix *left, *right, *top, *bottom;
	int index;

	bool operator<(const Pix& pix) const{
		return this->dist < pix.dist;
	}

	bool operator>(const Pix& pix) const{
		return this->dist > pix.dist;
	}
};

#endif