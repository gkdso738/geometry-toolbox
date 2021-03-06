/*
 * PointNode.cpp
 *
 */

#include <cmath>
#include "PointNode.h"

PointNode::PointNode(PointType type, double x, double y) {

    // Identification of definition from PointType
    switch(type) {
    case PointType::INDEPENDENT: definition = &PointNode::independent; break;
    default: well_defined = false; return;
    }

    // Initialization of Data Members
    this->x = x;
    this->y = y;
}

PointNode::PointNode(PointType type, GeoNode* geo1, double x, double y) : GeoNode(1) {

    // Identification of definition from PointType
    switch(type) {
    case PointType::ON_LINE: definition = &PointNode::on_line; break;
    case PointType::ON_CIRCLE:	definition = &PointNode::on_circle; break;
    default: well_defined = false; return;
    }

    // Initialization of Data Members
    parents = new const GeoNode*[num_parents];
    parents[0] = geo1;
    this->x = x;
    this->y = y;

    update();
}

PointNode::PointNode(PointType type, GeoNode* geo1, GeoNode* geo2) : GeoNode(2) {

    // Identification of definition from PointType
    switch(type) {
    case PointType::POINT_POINT_MIDPOINT: definition = &PointNode::point_point_midpoint; break;
    case PointType::LINE_LINE_INTERSECTION: definition = &PointNode::line_line_intersection; break;
    case PointType::LINE_CIRCLE_FIRST_INTERSECTION: definition = &PointNode::line_circle_first_intersection; break;
    case PointType::LINE_CIRCLE_SECOND_INTERSECTION: definition = &PointNode::line_circle_second_intersection; break;
    case PointType::CIRCLE_CIRCLE_FIRST_INTERSECTION: definition = &PointNode::circle_circle_first_intersection; break;
    case PointType::CIRCLE_CIRCLE_SECOND_INTERSECTION: definition = &PointNode::circle_circle_second_intersection; break;
    default: well_defined = false; return;
    }

    // Initialization of Data Members
    parents = new const GeoNode*[num_parents];
    parents[0] = geo1;
    parents[1] = geo2;

    update();
}

PointNode::~PointNode() {
    if(point != nullptr)
        (point->parentPlot())->removeGraph(point);
}

void PointNode::print() const {
    cout << "----------------------------------------\n";
    cout << "Identifier: " << this->get_label() << endl;
    cout << "Data: " << x << '\t' << y << endl;
    cout << "Parents:";
    for(int i = 0; i < num_parents;++i)
        cout << " " << parents[i]->get_label();
    cout << endl;
}

void PointNode::display(Ui::MainWindow *ui) {
    if(point == nullptr){ //Initialization
        point = ui->custom_plot->addGraph();
        point->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
        point->setPen(QPen(QColor(120, 120, 120), 2));
        point->setLayer("front");
        point->setName(QString::fromStdString(this->get_label()));
    }

    point->setVisible(well_defined);
    point->data()->clear();
    point->addData(x, y);
}

void PointNode::access(double data[]) const {
    data[0] = x;
    data[1] = y;
}

void PointNode::mutate(double data[]) {
    x = data[0];
    y = data[1];
    update();
}

void PointNode::update() {
    well_defined = true;
    for (int i = 0; i < num_parents; ++i)
        well_defined &= parents[i]->get_well_defined();
    if (well_defined)
        (this->*definition)();
}

void PointNode::independent() {}

void PointNode::on_line() {

    // Data Access
    double line[3];
    parents[0]->access(line);

    // Mathematical Formula
    double t = (line[0] * x + line[1] * y + line[2])/(line[0] * line[0] + line[1] * line[1]);
    x -= line[0] * t;
    y -= line[1] * t;
}

void PointNode::on_circle() {

    // Data Access
    double circle[3];
    parents[0]->access(circle);

    // Mathematical Formula
    double current_radius = sqrt((x - circle[0]) * (x - circle[0]) + (y - circle[1]) * (y - circle[1]));

    if(current_radius < EPSILON) {
        well_defined = false;

    } else {
        x = circle[0] + (x - circle[0]) * circle[2] / current_radius;
        y = circle[1] + (y - circle[1]) * circle[2] / current_radius;

        well_defined = true;

    }
}

void PointNode::point_point_midpoint() {

    // Data Access
    double p1[2], p2[2];
    parents[0]->access(p1);
    parents[1]->access(p2);

    // Mathematical Formula
    x = (p1[0]+p2[0])/2;
    y = (p1[1]+p2[1])/2;
}

void PointNode::line_line_intersection() {

    // Data Access
    double delta, delta_x, delta_y;
    double line1[3], line2[3];
    parents[0]->access(line1);
    parents[1]->access(line2);

    // Mathematical Formula
    delta = line1[0] * line2[1] - line1[1] * line2[0];

    if(delta < EPSILON && delta > -EPSILON) {
        well_defined = false;

    } else {
        delta_x = - line1[2] * line2[1] + line1[1] * line2[2];
        delta_y = - line1[0] * line2[2] + line1[2] * line2[0];
        x = delta_x / delta;
        y = delta_y / delta;

        well_defined = true;

    }
}

void PointNode::line_circle_first_intersection() {

    // Data Access
    double line[3], circle[3];
    parents[0]->access(line);
    parents[1]->access(circle);

    // Mathematical Formula
    double project_x = circle[0], project_y = circle[1];
    double t = (line[0] * project_x + line[1] * project_y + line[2])/(line[0] * line[0] + line[1] * line[1]);

    project_x -= line[0] * t;
    project_y -= line[1] * t;

    double dist = sqrt((project_x - circle[0]) * (project_x - circle[0]) + (project_y - circle[1]) * (project_y - circle[1]));

    if(dist <= circle[2] + EPSILON) {
        double to_shift = circle[2] * circle[2] - dist * dist;
        if(to_shift < 0.0) {to_shift = 0;}
        to_shift = sqrt(to_shift);

        double normalize_factor = sqrt(line[0] * line[0] + line[1] * line[1]);
        x = project_x + to_shift * line[0] / normalize_factor;
        y = project_y + to_shift * line[1] / normalize_factor;

        well_defined = true;

    } else { well_defined = false; }
}

void PointNode::line_circle_second_intersection() {

    // Data Access
    double line[3], circle[3];
    parents[0]->access(line);
    parents[1]->access(circle);

    // Mathematical Formula
    double project_x = circle[0], project_y = circle[1];
    double t = (line[0] * project_x + line[1] * project_y + line[2])/(line[0] * line[0] + line[1] * line[1]);

    project_x -= line[0] * t;
    project_y -= line[1] * t;

    double dist = sqrt((project_x - circle[0]) * (project_x - circle[0]) + (project_y - circle[1]) * (project_y - circle[1]));

    if(dist <= circle[2] + EPSILON){
        double to_shift = circle[2] * circle[2] - dist * dist;
        if(to_shift < 0.0) { to_shift = 0; }
        to_shift = sqrt(to_shift);

        if(to_shift < EPSILON){
            well_defined = false;
            return;
        }

        double normalize_factor = sqrt(line[0] * line[0] + line[1] * line[1]);
        x = project_x - to_shift * line[0] / normalize_factor;
        y = project_y - to_shift * line[1] / normalize_factor;

        well_defined = true;

    } else { well_defined = false; }
}


void PointNode::circle_circle_first_intersection() {

    // Data Access
    double circle1[3], circle2[3];
    parents[0]->access(circle1);
    parents[1]->access(circle2);

    // Mathematical Formula
    double dist = sqrt((circle1[0] - circle2[0]) * (circle1[0] - circle2[0]) + (circle1[1] - circle2[1]) * (circle1[1] - circle2[1]));

    if(dist < abs(circle1[2] - circle2[2]) - EPSILON || dist > abs(circle1[2] - circle2[2]) + EPSILON) {
        well_defined = false;

    } else {

        if(circle1[2] < circle2[2]) {
            for(int i = 0; i < 3; i++)
                std::swap(circle1[i], circle2[i]);
        }

        double result_x = circle1[0], result_y = circle1[0];
        double R = circle1[2], r = circle2[2];
        double normalize_factor = dist;

        double shift_horizontal = (R * R - r * r) / dist;
        shift_horizontal = (dist + shift_horizontal) / 2;
        result_x = result_x + (circle2[0] - circle1[0]) * shift_horizontal / normalize_factor;
        result_y = result_y + (circle2[1] - circle1[1]) * shift_horizontal / normalize_factor;

        double shift_vertical = sqrt(R * R - shift_horizontal * shift_horizontal);
        result_x = result_x + (circle2[1] - circle1[1]) * shift_vertical / normalize_factor;
        result_y = result_y - (circle2[0] - circle1[0]) * shift_vertical / normalize_factor;

        x = result_x;
        y = result_y;

        well_defined = true;
    }
}

void PointNode::circle_circle_second_intersection() {

    // Data Access
    double circle1[3], circle2[3];
    parents[0]->access(circle1);
    parents[1]->access(circle2);

    // Mathematical Formula
    double dist = sqrt((circle1[0] - circle2[0]) * (circle1[0] - circle2[0]) + (circle1[1] - circle2[1]) * (circle1[1] - circle2[1]));

    if(dist < abs(circle1[2] - circle2[2]) - EPSILON || dist > abs(circle1[2] - circle2[2]) + EPSILON) {
        well_defined = false;

    } else {

        if(circle1[2] < circle2[2]) {
            for(int i = 0; i < 3; i++)
                std::swap(circle1[i], circle2[i]);
        }

        double result_x = circle1[0], result_y = circle1[0];
        double R = circle1[2], r = circle2[2];
        double normalize_factor = dist;

        double shift_horizontal = (R * R - r * r) / dist;
        shift_horizontal = (dist + shift_horizontal) / 2;
        result_x = result_x + (circle2[0] - circle1[0]) * shift_horizontal / normalize_factor;
        result_y = result_y + (circle2[1] - circle1[1]) * shift_horizontal / normalize_factor;

        double shift_vertical = -sqrt(R * R - shift_horizontal * shift_horizontal);
        result_x = result_x + (circle2[1] - circle1[1]) * shift_vertical / normalize_factor;
        result_y = result_y - (circle2[0] - circle1[0]) * shift_vertical / normalize_factor;

        x = result_x;
        y = result_y;

        well_defined = true;

    }
}

void PointNode::labels(vector<string>* point_labels, vector<string>*, vector<string>*, vector<string>*) const {
    point_labels->push_back(this->get_label());
}
