/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_EXTRAS_PRIMITIVES
#define RES_EXTRAS_PRIMITIVES 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

PrimitiveLine::PrimitiveLine(const glm::vec3& _p1, const glm::vec3& _p2) :
	p1(_p1),
	p2(_p2)
{}
int PrimitiveLine::draw(const glm::vec3& offset, const glm::vec3& rot, bee::RGBA color) {
	return bee::draw_line(p1+offset, p2+offset, color);
}

PrimitiveTriangle::PrimitiveTriangle(const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3) :
	p1(_p1),
	p2(_p2),
	p3(_p3)
{}
int PrimitiveTriangle::draw(const glm::vec3& offset, const glm::vec3& rot, bee::RGBA color) {
	return bee::draw_triangle(p1+offset, p2+offset, p3+offset, color, false);
}

PrimitiveQuad::PrimitiveQuad(const glm::vec3& _pos, const glm::vec3& _size) :
	pos(_pos),
	size(_size)
{}
int PrimitiveQuad::draw(const glm::vec3& offset, const glm::vec3& rot, bee::RGBA color) {
	return bee::draw_quad(pos+offset, size, 1, color);
}

PrimitivePolygon::PrimitivePolygon(const glm::vec3& _pos, double _radius, double _angle_start, double _angle_span, unsigned int _segment_amount) :
	pos(_pos),
	radius(_radius),
	angle_start(_angle_start),
	angle_span(_angle_span),
	segment_amount(_segment_amount)
{}
int PrimitivePolygon::draw(const glm::vec3& offset, const glm::vec3& rot, bee::RGBA color) {
	return bee::draw_polygon(pos+offset, radius, angle_start, angle_span, segment_amount, 1, color);
}

PrimitiveArc::PrimitiveArc(const glm::vec3& _pos, double _radius, double _angle_start, double _angle_span) :
	PrimitivePolygon(_pos, _radius, _angle_start, _angle_span, _angle_span/4)
{}

PrimitiveCircle::PrimitiveCircle(const glm::vec3& _pos, double _radius) :
	PrimitiveArc(_pos, _radius, 0, 360)
{}

VectorSprite::VectorSprite(const std::string& _path) :
	path(_path),
	primitives()
{
	std::string datastr = file_get_contents(path);
	std::istringstream data_stream (datastr);

	while (!data_stream.eof()) {
		std::string tmp;
		getline(data_stream, tmp);

		if (tmp.empty()) {
			continue;
		}

		std::vector<std::string> p = splitv(trim(tmp), '\t', false);
		std::vector<std::string> params;
		for (auto& e : p) { // Remove empty values
			if (!e.empty()) {
				params.push_back(e);
			}
		}

		if (params[0][0] == '#') {
			continue;
		} else if (params[0] == "line") {
			primitives.push_back(new PrimitiveLine(
				glm::vec3(
					std::stod(params[1]),
					std::stod(params[2]),
					std::stod(params[3])
				), glm::vec3(
					std::stod(params[4]),
					std::stod(params[5]),
					std::stod(params[6])
				)
			));
		} else if (params[0] == "triangle") {
			primitives.push_back(new PrimitiveTriangle(
				glm::vec3(
					std::stod(params[1]),
					std::stod(params[2]),
					std::stod(params[3])
				), glm::vec3(
					std::stod(params[4]),
					std::stod(params[5]),
					std::stod(params[6])
				), glm::vec3(
					std::stod(params[7]),
					std::stod(params[8]),
					std::stod(params[9])
				)
			));
		} else if (params[0] == "quad") {
			primitives.push_back(new PrimitiveQuad(
				glm::vec3(
					std::stod(params[1]),
					std::stod(params[2]),
					std::stod(params[3])
				), glm::vec3(
					std::stod(params[4]),
					std::stod(params[5]),
					std::stod(params[6])
				)
			));
		} else if (params[0] == "polygon") {
			primitives.push_back(new PrimitivePolygon(
				glm::vec3(
					std::stod(params[1]),
					std::stod(params[2]),
					std::stod(params[3])
				),
				std::stod(params[4]),
				std::stod(params[5]),
				std::stod(params[6]),
				std::stoi(params[7])
			));
		} else if (params[0] == "arc") {
			primitives.push_back(new PrimitiveArc(
				glm::vec3(
					std::stod(params[1]),
					std::stod(params[2]),
					std::stod(params[3])
				),
				std::stod(params[4]),
				std::stod(params[5]),
				std::stod(params[6])
			));
		} else if (params[0] == "circle") {
			primitives.push_back(new PrimitiveCircle(
				glm::vec3(
					std::stod(params[1]),
					std::stod(params[2]),
					std::stod(params[3])
				), std::stod(params[4])
			));
		}
	}
}
/*VectorSprite::VectorSprite(const VectorSprite& other) :
	path(other.path),
	primitives()
{
	for (auto& p : other.primitives) {
		//primitives.push_back(new Primitive(p));
	}
}*/
VectorSprite::~VectorSprite() {
	for (auto& p : primitives) {
		delete p;
	}
}

/*VectorSprite& VectorSprite::operator=(const VectorSprite& rhs) {
	if (this != &rhs) {
		this->path = rhs.path;

		for (auto& p : primitives) {
			delete p;
		}
		this->primitives.clear();

		for (auto& p : rhs.primitives) {
			//this->primitives.push_back(new Primitive(p));
		}
	}
	return *this;
}*/

int VectorSprite::draw(const glm::vec3& pos, const glm::vec3& rot, bee::RGBA color) {
	for (auto& p : primitives) {
		p->draw(pos, rot, color);
	}
	return 0;
}

#endif // RES_EXTRAS_PRIMITIVES
