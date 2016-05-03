#include "matrix.h"

QNum Objective_Function(const intVec& my_vector) {
	for (unsigned int k = 0; k < my_vector.size(); ++k)
		if (my_vector[k] == 1)
			return (k);
	return (-1); //Error
}

void promt_to_continue() {
#ifdef DEBUG
	std::cout<< "Press ENTER to continue...";
	std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
#endif
}
