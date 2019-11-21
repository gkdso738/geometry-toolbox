/*
 * PointNode.h
 *
 */

#ifndef POINTNODE_H_
#define POINTNODE_H_

#include "GeoNode.h"

class PointNode: public GeoNode {
	double x, y;
	void (*construct)();
	virtual void print() const override;
	virtual void print_GUI() const override;
	virtual void access(double data[]) const override;
	virtual void mutate(double data[]) override;
	virtual void update() override;
	void line_line_intersection(double prm[]); // mutator
public:
	PointNode();
	virtual ~PointNode();
};

#endif /* POINTNODE_H_ */