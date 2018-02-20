#include <cmath>
#include <algorithm>
#include "MinkowskiSum.h"

Point::Point() {}

Point::Point(double x, double y) :
    x(x),
    y(y)
{}


Point Point::operator- (const Point & second) const {
    return Point(x - second.x, y - second.y);
}

Point Point::operator+ (const Point & second) const {
    return Point(x + second.x, y + second.y);
}

Point & Point::operator= (const Point & point) {
    this->x = point.x;
    this->y = point.y;
    return *this;
}


MinkowskiSum::MinkowskiSum(const std::vector<Point> & array_A, const std::vector<Point> & array_B)
    : polygon_A_(array_A),
      polygon_B_(array_B)
{
    invert_the_polygon(polygon_B_);
    build_polygon();
    has_zero_ = is_in_polygon() ? "YES" : "NO";
}

const std::vector<Point> & MinkowskiSum::get_final_polygon() const {
    return polygon_to_find_;
}

const std::string & MinkowskiSum::has_zero() const {
    return has_zero_;
}

//вектор между двумя точками
Point MinkowskiSum::edge_to_vector(const std::vector<Point> & polygon, int index) {
    return polygon[index] - polygon[index - 1];
}
//ищем косинус угла между последним добавленным ребром и новым ребром
double MinkowskiSum::angle_cos_between_edges(const std::vector<Point> & polygon, int current_vertex_index) {
 
    return angle_cos(
            last_edge_,
            edge_to_vector(polygon, current_vertex_index + 1)
            );                    
}

//косинус между двумя векторами
double MinkowskiSum::angle_cos(const Point & first, const Point & second) {
    
    return (first.x * second.x + first.y * second.y) / (
        sqrt(pow(first.x, 2) + pow(first.y, 2)) *
        sqrt(pow(second.x, 2) + pow(second.y, 2)));
}

//инвертируем точки многоугольника
void MinkowskiSum::invert_the_polygon(std::vector<Point> & polygon) {
    for (int index = 0; index < polygon.size(); ++index) {
        polygon[index].x = (-1) * polygon[index].x;
        polygon[index].y = (-1) * polygon[index].y;
    }
}

void MinkowskiSum::find_next_point() {
    
    if (index_A_ == polygon_A_.size() - 1) {
        ++index_B_;
    } else if (index_B_ == polygon_B_.size() - 1) {
        ++index_A_;
    } else {
    
        if (angle_cos_between_edges(polygon_A_, index_A_) > angle_cos_between_edges(polygon_B_, index_B_)) {
            last_edge_ = polygon_A_[index_A_ + 1] - polygon_A_[index_A_];
            ++index_A_;
    
        } else if (angle_cos_between_edges(polygon_A_, index_A_) < angle_cos_between_edges(polygon_B_, index_B_)) {
            last_edge_ = polygon_B_[index_B_ + 1] - polygon_B_[index_B_];
            ++index_B_;  

        } else {
            
            last_edge_ = polygon_A_[index_A_ + 1] - polygon_A_[index_A_];
            ++index_A_;
            ++index_B_;
        }
    }
}

//сдвигаем циклически на start_index влево
void MinkowskiSum::move_the_points_cyclically(std::vector<Point> & polygon, int start_index) {
    std::rotate(polygon.begin(), polygon.begin() + start_index, polygon.end());
}

void MinkowskiSum::build_polygon() {
    //ищем стартовые точки
    index_A_ = find_start_point_index(polygon_A_);
    index_B_ = find_start_point_index(polygon_B_);
    //сдвигаем векторы со вершинами
    move_the_points_cyclically(polygon_A_, index_A_);
    move_the_points_cyclically(polygon_B_, index_B_);
    
    index_A_ = 0;
    index_B_ = 0;
    //за начальное направление принимаем (-1, 0)
    last_edge_.x = -1;
    last_edge_.y = 0;

    polygon_A_.push_back(polygon_A_[0]);
    polygon_B_.push_back(polygon_B_[0]);


    while (index_A_ + 1 < polygon_A_.size() || index_B_ + 1 < polygon_B_.size()) {
        add_another_point();
        find_next_point();
    }
}


void MinkowskiSum::add_another_point() {
    polygon_to_find_.push_back(polygon_A_[index_A_] + polygon_B_[index_B_]);
}

//ищем стартовые точки -- нижние левые
int MinkowskiSum::find_start_point_index(const std::vector<Point> & polygon) {
    int minimum = 0;
    for (int index = 1; index < polygon.size(); ++index) {
        if (polygon[index].y < polygon[minimum].y || polygon[index].y == polygon[minimum].y && polygon[index].x < polygon[minimum].x) {
            minimum = index;
        }
    }
    return minimum;
}

bool MinkowskiSum::is_in_polygon() { //входит ли (0, 0) в итоговый многоугольник
    bool result = false;
    for (int i = 0, j = polygon_to_find_.size() - 1; i < polygon_to_find_.size(); j = i++) {
        if ((((polygon_to_find_[i].y <= 0) && (0 < polygon_to_find_[j].y)) || ((polygon_to_find_[j].y <= 0) && (0 < polygon_to_find_[i].y)))
            && (0 > (polygon_to_find_[j].x - polygon_to_find_[i].x) * (0 - polygon_to_find_[i].y) 
            / (polygon_to_find_[j].y - polygon_to_find_[i].y)+ polygon_to_find_[i].x)) {
                
                result = !result;
        }
    }
    return result;
}
