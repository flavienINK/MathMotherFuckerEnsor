#include "Tensor.hpp"

#include <stdexcept>
#include <stdint.h>
#include <Eigen/Dense>
#define NB_POINTS_NEEDED 7

namespace leydef{

	Tensor::Tensor(){
		m_values = Eigen::VectorXd::Zero(NB_VALUES);
	}

	Tensor::~Tensor(){
	}

	void Tensor::compute(const Eigen::MatrixXd& list1, const Eigen::MatrixXd& list2, const Eigen::MatrixXd& list3){
		//Verifie qu'il y a assez de points dans les listes envoyées
		if(NB_POINTS_NEEDED > list1.rows() || NB_POINTS_NEEDED > list2.rows() || NB_POINTS_NEEDED > list3.rows()){
			throw std::invalid_argument("not enough points to compute the tensor.");
		}
		
		/* Calcul de la matrice A pour calculer le tenseur */
		Eigen::MatrixXd A = Eigen::MatrixXd::Zero(4*NB_POINTS_NEEDED, 27);
		
		for(uint32_t p=0;p<NB_POINTS_NEEDED;++p){
			for(uint32_t i=0;i<2;++i){
				for(uint32_t l=0;l<2;++l){
					for(uint32_t k=0;k<3;++k){
						A(4*p+2*i+l, 9*k+3*l+2) += list1(p, k) * list2(p, i)* list3(p, 2); 
						A(4*p+2*i+l, 9*k+3*l+i) += -list1(p, k) * list2(p, 2)* list3(p, 2); 
						A(4*p+2*i+l, 9*k+3*2+2) += -list1(p, k) * list2(p, i)* list3(p, l); 
						A(4*p+2*i+l, 9*k+3*2+i) += list1(p, k) * list2(p, 2)* list3(p, l); 
					}
				}
			}
		}
		
		/* Computing the SVD of A */
		Eigen::JacobiSVD<Eigen::MatrixXd> mySVD(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
		Eigen::MatrixXd V = mySVD.matrixV();
		
		/* The tensor */
		m_values = V.col(26);
	}

	Eigen::VectorXd Tensor::doTransfert(const Eigen::VectorXd& p1, const Eigen::VectorXd& p2) const{
		/* create the Aprime matrix */
		Eigen::MatrixXd Aprime = Eigen::MatrixXd::Zero(4, 2);
		Eigen::VectorXd res = Eigen::VectorXd::Zero(4);
		
		for(uint32_t i=0;i<2;++i){
			for(uint32_t l=0;l<2;++l){
				for(uint32_t k=0;k<3;++k){
					Aprime(2*i+l, l) += p1(k) * (p2(2)*m_values(9*k+3*2+i) - p2(i)*m_values(9*k+3*2+2));
					res(2*i+l) += -p1(k) * (p2(i)*m_values(9*k+3*l+2) - p2(2)*m_values(9*k+3*l+i));
				}
			}
		}
		
		/* Computing the SVD of Aprime */
		Eigen::JacobiSVD<Eigen::MatrixXd> trSVD(Aprime, Eigen::ComputeThinU | Eigen::ComputeThinV);
		
		Eigen::VectorXd guessPoint = trSVD.solve(res);
		
		Eigen::VectorXd p3(3);
		p3(0) = guessPoint(0);
		p3(1) = guessPoint(1);
		p3(2) = 1;
		
		return p3;
	}

}
