#ifndef HMATRIX_CPP
#define HMATRIX_CPP

#include "h_Matriz.h"

template<class T>
h_Matriz<T>::h_Matriz()
{
}

template<class T>
h_Matriz<T>::~h_Matriz()
{
}

template<class T>
h_Matriz<T>::h_Matriz(size_t n) { 
	Init(n, n); 
}

template<class T>
h_Matriz<T>::h_Matriz(size_t m, size_t n) { 
	Init(m, n); 
}

template<class T>
h_Matriz<T>::h_Matriz(cv::Mat img){
	Init(img.rows, img.cols);
	for (int i = 0; i < row; i++)
		memcpy(&(ptr[i*col]), img.ptr<T>(i, 0), col * sizeof(T));
}

template<class T>
void h_Matriz<T>::Init(size_t m, size_t n){
	row = m;
	col = n;
	ptr = new T[row * col];
}

template<class T>
inline T h_Matriz<T>::Get(size_t r, size_t c) { 
	return *(ptr + r*(col)+c); 
}

template<class T>
inline void h_Matriz<T>::Set(size_t r, size_t c, T val) {
	*(ptr + r*(col)+c) = val; 
}

template<class T>
void h_Matriz<T>::Set_Matriz(h_Matriz<T> mat){
	delete ptr;
	Init(mat.row, mat.col);
	memcpy(&(ptr[0]), &(mat.ptr[0]), row*col * sizeof(T));
}

template<class T>
void h_Matriz<T>::Get_Matriz(h_Matriz<T> *mat){
	if (mat->row == row && mat->col == col)
		memcpy(&mat->ptr[0], &(ptr[0]), row*col * sizeof(T));
}

template<class T>
void h_Matriz<T>::h_Matriz2Mat(cv::Mat *img){
	if (img->rows == row && img->cols == col)
		for (size_t i = 0; i < row; i++)
			memcpy(img->ptr<T>(i, 0), &(ptr[i*col]), col * sizeof(T));
}

template<class T>
void h_Matriz<T>::Imprimir(){
	for (size_t i = 0; i < row; i++){
		for (size_t j = 0; j < col; j++)
			std::cout << ptr[i*col + j] << "  ";
		std::cout << endl;
	}
}

#endif // !HMATRIX_CPP