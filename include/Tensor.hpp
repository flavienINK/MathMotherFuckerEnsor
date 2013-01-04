#ifndef __TENSOR_HPP__
#define __TENSOR_HPP__

#include <Eigen/Dense>

class Tensor{
	static const int NB_VALUES = 27;
	
	public:
		Tensor();
		~Tensor();
		void compute(const Eigen::MatrixXd& list1, const Eigen::MatrixXd& list2, const Eigen::MatrixXd& list3);
		
	private:
		Eigen::VectorXd m_values;
};

#endif
