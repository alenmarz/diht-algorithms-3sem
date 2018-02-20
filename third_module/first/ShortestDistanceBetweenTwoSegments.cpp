#include "ShortestDistanceBetweenTwoSegments.h"

Point::Point(double x, double y, double z): x(x), y(y), z(z) 
{}

Point::Point()
{}

double Point::abs() {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));  
}

Point & Point::operator =(const Point & point) {
    x = point.x;
    y = point.y;
    z = point.z;
    return * this;
}

//Скалярное произведение
double operator *(const Point & left, const Point & right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

Point operator *(const Point & point, double multiplier) {
    return Point(point.x * multiplier, point.y * multiplier, point.z * multiplier);
}

//Векторное произведение
Point cross_product(const Point & left, const Point & right) {
    return Point(left.y * right.z - left.z * right.y,
                 left.z * right.x - left.x * right.z,
                 left.x * right.y - left.y * right.x);
}

Point operator +(const Point & left, const Point & right) {
    return Point(left.x + right.x, left.y + right.y, left.z + right.z);
}

Point operator -(const Point & left, const Point & right) {
    return Point(left.x - right.x, left.y - right.y, left.z - right.z);
}

Point Segment::vector() {
    Point vector = second - first;
    return vector;
}

    

ShortestDistanceBetweenTwoSegments::ShortestDistanceBetweenTwoSegments(Segment first, Segment second)
    : first_(first), second_(second)
{
    search_for_shortest_distance();
}
    
double ShortestDistanceBetweenTwoSegments::calculate_distance(double t, double s) {
    Point shortest_vector_between = a - (first_.second - first_.first) * t
                                    + (second_.second - second_.first) * s;
    return shortest_vector_between.abs();
}
    
//Проверим теперь, находятся ли параметры в пределах [0, 1]
void ShortestDistanceBetweenTwoSegments::search_for_shortest_distance() {
    //Пусть переменная counter обозначает количество параметров,
    //попадающих в интервал [0, 1]
    
    //случай точка-точка
    if (b.abs() == 0 && c.abs() == 0) {
        Point vector_between(first_.first - second_.first);
        shortest_distance = vector_between.abs();
        return;
    }
    //случай точка-отрезок
    if (b.abs() == 0 || c.abs() == 0) {
        //нормируем вектор, соответствующий ненулевому отрезку
        Point not_zero_segment;
        not_zero_segment = (b.abs() != 0 ? b : c);
        Point vector_between(cross_product(not_zero_segment * (1 / not_zero_segment.abs()), a));
        if (pow(a.abs(), 2) - pow(vector_between.abs(), 2) <= pow(not_zero_segment.abs(), 2)) {
            shortest_distance = vector_between.abs();
        } else {
            Point vector_to_border(not_zero_segment - a);
            shortest_distance = std::min(vector_to_border.abs(), a.abs());
        }
        return;
    }
    //случай отрезок-отрезок
    double s, t;
    size_t counter = 0;
    
    if (det_ > 0) {
        if (det_s_ > 0 && det_s_ < det_) {
            ++counter;
        }
        if (det_t_ > 0 && det_t_ < det_) {
            ++counter;
        }
    }
    if (det_ < 0) {
        if (det_s_ < 0 && det_s_ > det_) {
            ++counter;
        }
        if (det_t_ < 0 && det_t_ > det_) {
            ++counter;
        }
    }
    //тогда если counter == 2 оба конца кратчайшего отрезка
    //между отрезками лежат внутри этих отрезков
    //и мы можем рассчитать параметры
    if (counter == 2) {
        s = det_s_ / det_;
        t = det_t_ / det_;
        shortest_distance = calculate_distance(t, s);
    } else {
        //если мы попали сюда, значит, как минимум одно из значений
        //параметров лежит на границе интервала
        //рассматриваем отдельно все четыре случая
        //1) s == 0
        t = (a * b) / (b * b);
        check_borders(t);
        double distance_1 = calculate_distance(t, 0);
        //2) s == 1
        t = ((a * b) + (b * c)) / (b * b);
        check_borders(t);
        double distance_2 = calculate_distance(t, 1);
        //3) t == 0
        s = (-1) * (a * c) / (c * c);
        check_borders(s);
        double distance_3 = calculate_distance(0, s);
        //4) t == 1
        s = ((b * c) - (a * c)) / (c * c);
        check_borders(s);
        double distance_4 = calculate_distance(1, s);
        
        shortest_distance = std::min(std::min(distance_1, distance_2),
                                        std::min(distance_3, distance_4));
    }
}
//если значения параметра находятся за границами интервала [0, 1]
//присваиваем ему значение ближайшей граничной точки
void ShortestDistanceBetweenTwoSegments::check_borders(double & t) { 
    if (t <= 0) {
        t = 0;
    }
    if (t >= 1) {
        t = 1;
    }
}
