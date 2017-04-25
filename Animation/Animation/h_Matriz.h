#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

template<class T>
class h_Matriz
{
public:
	T *ptr;
	size_t row, col;

	h_Matriz();
	h_Matriz(size_t n);
	h_Matriz(size_t m, size_t n);
	h_Matriz(cv::Mat img);
	~h_Matriz();

	void Init(size_t, size_t);
	inline T Get(size_t, size_t);
	inline void Set(size_t, size_t, T);
	void Set_Matriz(h_Matriz<T> );
	void Get_Matriz(h_Matriz<T> *);
	void h_Matriz2Mat(cv::Mat *);
	void Imprimir();
	
};

#include "H_Matriz.cpp"

