/*
 * CircleNode.h
 *
 */

#ifndef CIRCLENODE_H_
#define CIRCLENODE_H_

#include "GeoNode.h"

class CircleNode: public GeoNode {
	double center_x, center_y, radius;
	void (*construct)();
	virtual void print() const override;
	virtual void display() const override;
	virtual void access(double data[]) const override;
	virtual void mutate(double data[]) override;
	virtual void update() override;
public:
	CircleNode();
	virtual ~CircleNode();
};

#endif /* CIRCLENODE_H_ */
