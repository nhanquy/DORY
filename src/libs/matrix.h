//
//	matrix.h : header file to declare prototype of 2D vector
//	Created by NNQ
//	02 Feb 2016
//
#ifndef LIB_MATRIX_H_
#define LIB_MATRIX_H_
#include <iostream>
#include <vector>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#  include <iostream>
#  define DEBUG_LOG std::cout

#else
#  define DEBUG_LOG if(false) std::cout
#endif
//
// MATRIXS & VECTORS DEFINITIONS
//
typedef float QNum;
typedef int QInt;

typedef std::vector<QNum> numVec;
typedef std::vector<QInt> intVec;
typedef std::vector<numVec> numMat;
typedef std::vector<intVec> intMat;

typedef std::vector<bool> boolVec;
typedef std::vector<boolVec> boolMat;
//
//
// Matrix declaration example:
// intMat my_matrix(noRows,intVec(noCols));
//
// OBJECTIVE FUNCTION
QNum Objective_Function(const intVec& my_vector);
void promt_to_continue();
//
// ADDITIONAL FUNCTIONS FORS MATRIX CACULATION
//
// -------------------------------
// Summing all rows of a 2D matrix
// -------------------------------
template<typename vecT, typename matT>
vecT rows_sum(const matT& my_matrix) {
	int nrows = my_matrix.size();
	int ncols = my_matrix[0].size();
	vecT rowsum(ncols, 0);
	for (int i = 0; i < nrows; ++i)
		for (int j = 0; j < nrows; ++j)
			rowsum[j] += my_matrix[i][j];
	return (rowsum);
}
// ----------------------------------
// Summing all columns of a 2d matrix
// ----------------------------------
template<typename vecT, typename matT>
vecT cols_sum(const matT& my_matrix) {
	int nrows = my_matrix.size();
	int ncols = my_matrix[0].size();
	vecT colsum(nrows, 0);
	for (int i = 0; i < nrows; ++i)
		for (int j = 0; j < nrows; ++j)
			colsum[i] += my_matrix[i][j];
	return (colsum);
}
// --------------------------------------------------------
// Finding maximal value of a vector and its first position
// --------------------------------------------------------
template<typename T, typename vecT>
T max_vector(vecT my_vector, int& position) {
	T max = 0;
	for (unsigned int i = 0; i < my_vector.size(); ++i) {
		if (max < my_vector[i]) {
			max = my_vector[i];
			position = i;
		}
	}
	return max;
}
// -------------------
// Summing two vectors
// -------------------
template<typename vecT>
vecT vec_sum(const vecT& vecA, const vecT& vecB) {
	int size = vecA.size();
	vecT vecsum(size, 0);
	if (vecA.size() != vecB.size()) {
		std::cout << "Vectors dimension do not match! \n";
		return (vecsum);
	}
	for (int i = 0; i < size; ++i)
		vecsum[i] = vecA[i] + vecB[i];
	return (vecsum);
}
// ------------------------------------------------------------------------
// Taking sub vector by start and end (int) position of the original vector
// ------------------------------------------------------------------------
template<typename vecT>
vecT sub_vector(const vecT& vecA, int start, int end) {
	int size = end - start + 1;
	vecT subvec(size);
	for (int i = 0; i < size; ++i)
		subvec[i] = vecA[start + i];
	return (subvec);
}
// ----------------------------------
// Display a vector with label string
// ----------------------------------
template<typename vecT>
void disp_vector(const vecT& my_vector, std::string label = "") {
	if (label != "")
		std::cout << "\nVector: " << label << " \n";
	for (unsigned int i = 0; i < my_vector.size(); ++i)
		std::cout << my_vector[i] << " ";
	if (label != "")
		std::cout << "\n";
}
// ----------------------------------
// Display a vector with label string
// ----------------------------------
template<typename matT>
void disp_matrix(const matT& my_matrix, std::string label = "") {
	if (label != "")
		std::cout << "\nMatrix: " << label << " \n";
	for (unsigned int i = 0; i < my_matrix.size(); ++i) {
		disp_vector(my_matrix[i]);
		std::cout << "\n";
	}
}
// -----------------------------------------
// Summing all element of a numerical vector
// -----------------------------------------
template<typename T, typename vecT>
T sum_vector(const vecT& my_vector) {
	T sum = 0;
	for (unsigned int i = 0; i < my_vector.size(); ++i)
		sum += my_vector[i];
	return (sum);
}
template<typename matT, typename vecT>
// -------------------------------------------------------------------------------
// Push new vector row to a matrix, element by element to assure memory allocation
// (In case of first push to avoid heap error)
// -------------------------------------------------------------------------------
void push_row(matT& my_matrix, const vecT& my_row) {
	my_matrix.push_back(vecT());
	int index = my_matrix.size() - 1;
	for (int i = 0; i < my_row.size(); ++i)
		my_matrix[index].push_back(my_row[i]);
}
//
template<typename vecT1, typename vecT2> void get_vector(vecT1& copyArray,
		vecT2 sourceArray) {
	for (int i = 0; i < sourceArray.size(); ++i) {
		copyArray[i] = sourceArray[i];
	}
}
// END OF ADDITIONAL FUNCTION DEFINITIONS
#endif /* LIB_MATRIX_H_ */
