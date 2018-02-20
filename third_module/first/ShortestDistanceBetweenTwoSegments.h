#include <math.h>
#include <algorithm>

struct Point {
    double x, y, z;
    
    Point(double x, double y, double z);
    Point();
    double abs();
    Point & operator =(const Point & point);
};

double operator *(const Point & left, const Point & right);
Point operator *(const Point & point, double multiplier);

Point cross_product(const Point & left, const Point & right);

Point operator +(const Point & left, const Point & right);
Point operator -(const Point & left, const Point & right);

struct Segment {
    Point first, second;
    Point vector();
};

//Записываем уравнения отрезков в парам. виде:
//
//S(s) = first_.first + (first_.second - first_.first) * s
//T(t) = second_.first + (second_.second - second_.first) * t
//
//Получаем систему линейных уравнений
//Которая выглядит следующим образом:
//--------------------------------
//(b, c) * s - (b, b) * t = (a, b)
//(c, c) * s - (b, c) * t = (a, c)
//--------------------------------
//Где буковки обозначают следующее:
//a == second_.first - first_.first
//b == first_.vector
//c == second_.vector

class ShortestDistanceBetweenTwoSegments {
private:
    Segment first_, second_; //Два отрезка
    
    Point a = second_.first - first_.first;
    Point b = first_.vector();
    Point c = second_.vector();
    
public:
    double shortest_distance;

    ShortestDistanceBetweenTwoSegments(Segment first, Segment second);
    
private:
    double calculate_distance(double t, double s);

    //Найдем детерминанты ЛСУ
    double det_ = (b * b) * (c * c) - (b * c) * (b * c);
    double det_t_ = (a * b) * (c * c) - (a * c) * (b * c);
    double det_s_ = (-1) * (a * c) * (b * b) + (a * b) * (b * c);
    
    void search_for_shortest_distance();

    void check_borders(double & t);
};
