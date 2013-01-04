#include "PointList.hpp"

#include <vector>
#include <stdint.h>
#include <Eigen/Dense>

namespace leydef{
	
	PointList::PointList(uint32_t color) : m_color(color){
	}
	
	PointList::~PointList(){
	}
	
	void PointList::addPoint(const Eigen::VectorXd& newPoint){
		m_points.push_back(newPoint);
	}
	
	int PointList::getSize() const{
		return m_points.size();
	}
	
}
