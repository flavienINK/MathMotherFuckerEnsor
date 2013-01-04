#include "PointList.hpp"

namespace leydef{
	
	PointList::PointList(uint32_t color) : m_color(color){
	}
	
	PointList::~PointList(){
	}
	
	int PointList::getSize() const{
		return m_points.size();
	}
	
}
