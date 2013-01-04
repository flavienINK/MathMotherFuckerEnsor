#ifndef __TENSOR_HPP__
#define __TENSOR_HPP__

#include <Eigen/Dense>

class Tensor{
	static const int NB_VALUES = 27;
	
	public:
		Tensor();
		~Tensor();
		
	private:
		Eigen::VectorXd m_values;
};

#endif
