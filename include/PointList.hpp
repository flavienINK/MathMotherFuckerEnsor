#ifndef __POINT_LIST_HPP__
#define __POINT_LIST_HPP__

#include <stdint.h>
#include <vector>
#include <Eigen/Dense>

namespace leydef{

	class PointList{
		public:
			PointList(uint32_t color);
			~PointList();
			int getSize() const;
			void addPoint(const Eigen::VectorXd& newPoint);
		
		private:
			std::vector<Eigen::VectorXd> m_points;
			uint32_t m_color;
	};

}

#endif
