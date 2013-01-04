#ifndef __POINT_LIST_HPP__
#define __POINT_LIST_HPP__

#include <stdint.h>
#include <string>
#include <vector>
#include <Eigen/Dense>

namespace leydef{

	class PointList{
		public:
			PointList(uint32_t color);
			~PointList();
			void load(const std::string& filename);
			int getSize() const;
			void addPoint(const Eigen::VectorXd& newPoint);
		
		private:
			std::vector<Eigen::VectorXd> m_points;
			uint32_t m_color;
	};

}

#endif
