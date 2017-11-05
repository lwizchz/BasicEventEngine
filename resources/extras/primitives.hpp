/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_EXTRAS_PRIMITIVES_H
#define RES_EXTRAS_PRIMITIVES_H 1

enum class E_PRIMITIVE {
	LINE,
	TRIANGLE,
	QUAD,
	POLYGON,
	ARC,
	CIRCLE
};

struct Primitive {
	virtual ~Primitive() {};
	virtual int draw(const glm::vec3&, const glm::vec3&, bee::RGBA) {return 0;};
};
struct PrimitiveLine : public Primitive {
	glm::vec3 p1;
	glm::vec3 p2;

	PrimitiveLine(const glm::vec3&, const glm::vec3&);
	int draw(const glm::vec3&, const glm::vec3&, bee::RGBA);
};
struct PrimitiveTriangle : public Primitive {
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;

	PrimitiveTriangle(const glm::vec3&, const glm::vec3&, const glm::vec3&);
	int draw(const glm::vec3&, const glm::vec3&, bee::RGBA);
};
struct PrimitiveQuad : public Primitive {
	glm::vec3 pos;
	glm::vec3 size;

	PrimitiveQuad(const glm::vec3&, const glm::vec3&);
	int draw(const glm::vec3&, const glm::vec3&, bee::RGBA);
};
struct PrimitivePolygon : public Primitive {
	glm::vec3 pos;
	double radius;
	double angle_start;
	double angle_span;
	unsigned int segment_amount;

	PrimitivePolygon(const glm::vec3&, double, double, double, unsigned int);
	virtual ~PrimitivePolygon() {};
	int draw(const glm::vec3&, const glm::vec3&, bee::RGBA);
};
struct PrimitiveArc : public PrimitivePolygon {
	PrimitiveArc(const glm::vec3&, double, double, double);
};
struct PrimitiveCircle : public PrimitiveArc {
	PrimitiveCircle(const glm::vec3&, double);
};

class VectorSprite {
		std::string path;
		std::vector<Primitive*> primitives;
	public:
		VectorSprite(const std::string&);
		//VectorSprite(const VectorSprite&);
		virtual ~VectorSprite();

		//VectorSprite& operator=(const VectorSprite&);

		int draw(const glm::vec3&, const glm::vec3&, bee::RGBA);
};

#endif // RES_EXTRAS_PRIMITIVES_H
