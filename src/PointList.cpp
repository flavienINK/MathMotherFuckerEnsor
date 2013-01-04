#include "PointList.hpp"

#include <vector>
#include <stdint.h>
#include <Eigen/Dense>
#include <SDL/SDL.h>

#include "MathIO.hpp"
#include "draw.hpp"

namespace leydef{
	
	PointList::PointList(uint32_t color) : m_color(color){
	}
	
	PointList::~PointList(){
	}
	
	void PointList::load(const std::string& filename){
		Eigen::MatrixXd mat;
		kn::loadMatrix(mat, filename);
		
		for(int i=0;i<mat.rows();++i){
			Eigen::VectorXd tmpVec(3);
			tmpVec(0) = mat(i, 0);
			tmpVec(1) = mat(i, 1);
			tmpVec(2) = mat(i, 2);
			addPoint(tmpVec);
		}
	}
	
	void PointList::save(const std::string& filename){
		Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(m_points.size(), 3);
		int cpt=0;
		for(std::vector<Eigen::VectorXd>::iterator i=m_points.begin();i!=m_points.end();++i){
				Eigen::VectorXd tmpVec = *i;
				mat(cpt, 0) = tmpVec(0);
				mat(cpt, 1) = tmpVec(1);
				mat(cpt, 2) = tmpVec(2);
				cpt++;
		}
		
		kn::saveMatrix(mat, filename, true, "Super Liste de points!!!");
	}
	
	void PointList::addPoint(const Eigen::VectorXd& newPoint){
		m_points.push_back(newPoint);
	}
	
	const Eigen::MatrixXd PointList::getData(){
		Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(m_points.size(), 3);
		int cpt=0;
		for(std::vector<Eigen::VectorXd>::iterator i=m_points.begin();i!=m_points.end();++i){
				Eigen::VectorXd tmpVec = *i;
				mat(cpt, 0) = tmpVec(0);
				mat(cpt, 1) = tmpVec(1);
				mat(cpt, 2) = tmpVec(2);
				cpt++;
		}
		
		return mat;
	}
	
	int PointList::getSize() const{
		return m_points.size();
	}
	
	void PointList::draw(SDL_Surface* screen){
		for(std::vector<VectorXd>::iterator i=m_points.begin();i!=m_points.end();++i){
			Eigen::VectorXd tmpVec = *i;
			fill_circle(screen, tmpVec(0), tmpVec(1), 3, m_color);
		}
	}
}
