/*
 * PointNode.h
 *
 */

#ifndef POINTNODE_H_
#define POINTNODE_H_

#include "GeoNode.h"

enum PointType {
	INDEPENDENT,
	ON_LINE,
	ON_CIRCLE,
	POINT_POINT_MIDPOINT,
	LINE_LINE_INTERSECTION,
	LINE_CIRCLE_FIRST_INTERSECTION,
	LINE_CIRCLE_SECOND_INTERSECTION,
	CIRCLE_CIRCLE_FIRST_INTERSECTION,
	CIRCLE_CIRCLE_SECOND_INTERSECTION
};

class PointNode: public GeoNode {

public:
	PointNode();
	PointNode(PointType type, double x, double y);
	PointNode(PointType type, GeoNode* geo1, double x, double y);
	PointNode(PointType type, GeoNode* geo1, GeoNode* geo2);

	virtual ~PointNode();

private:
	double x{0}, y{0};
    QCPGraph *point {nullptr};

	virtual void print() const override;

    virtual void display(Ui::MainWindow* ui) override;

	virtual void access(double data[]) const override;
	virtual void mutate(double data[]) override;

	virtual void update() override;
	void (PointNode::*definition)() {nullptr};

	void independent();
	void on_line();
	void on_circle();
	void point_point_midpoint();
	void line_line_intersection();
	void line_circle_first_intersection();
	void line_circle_second_intersection();
	void circle_circle_first_intersection();
	void circle_circle_second_intersection();

};

#endif /* POINTNODE_H_ */
