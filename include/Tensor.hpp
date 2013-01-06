#ifndef __TENSOR_HPP__
#define __TENSOR_HPP__

#include <Eigen/Dense>

namespace leydef{

	class Tensor{
		static const int NB_VALUES = 27;
		
		public:
			Tensor();
			~Tensor();
			void compute(const Eigen::MatrixXd& list1, const Eigen::MatrixXd& list2, const Eigen::MatrixXd& list3);
			const Eigen::VectorXd doTransfert(const Eigen::VectorXd& p1, const Eigen::VectorXd& p2);
			
		private:
			Eigen::VectorXd m_values;
	};
	
	//Different kind of transfert (ex : TRANSF13 to click on image 1 and 3, and guess on image 2)
	enum TransfertType{
		TRANSF12,
		TRANSF23,
		TRANSF13
	};
	
}

#endif
