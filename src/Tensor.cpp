#include "Tensor.hpp"

#include <Eigen/Dense>

Tensor::Tensor(){
	m_values = Eigen::VectorXd::Zero(NB_VALUES);
}

Tensor::~Tensor(){
}
