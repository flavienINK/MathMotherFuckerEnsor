#ifndef __POINT_LIST_HPP__
#define __POINT_LIST_HPP__

#include <stdint.h>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <SDL/SDL.h>

namespace leydef{

	class PointList{
		public:
			PointList(uint32_t color);
			~PointList();
			void load(const std::string& filename);
			void save(const std::string& filemane);
			const Eigen::MatrixXd getData();
			int getSize() const;
			const Eigen::VectorXd getLastPoint(); 
			void addPoint(const Eigen::VectorXd& newPoint);
			void draw(SDL_Surface* screen, int offsetX);
		
		private:
			std::vector<Eigen::VectorXd> m_points;
			uint32_t m_color;
	};

}

#endif
