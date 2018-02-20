#include <vector>
#include <string>

struct Point {
    double x;
    double y;

    Point();
    Point(double x, double y);

    Point operator- (const Point & second) const;  
    Point operator+ (const Point & second) const;
    Point & operator= (const Point & point);
};


class MinkowskiSum {
private:
    std::vector<Point> polygon_A_;
    std::vector<Point> polygon_B_;
    std::vector<Point> polygon_to_find_;
    Point last_edge_; //последнее добавленное в сумму ребро
                      //используется для сравнения углов на очередном шаге
    
    //индексы текущих вершин в соответствующих многоугольниках
    int index_A_;
    int index_B_;

    std::string has_zero_;

public:
    MinkowskiSum(const std::vector<Point> & array_A, const std::vector<Point> & array_B);

    const std::vector<Point> & get_final_polygon() const;

    const std::string & has_zero() const;

private:

    Point edge_to_vector(const std::vector<Point> & polygon, int index);

    double angle_cos_between_edges(const std::vector<Point> & polygon, int current_vertex_index);
    double angle_cos(const Point & first, const Point & second);

    void invert_the_polygon(std::vector<Point> & polygon);

    void find_next_point();

    void move_the_points_cyclically(std::vector<Point> & polygon, int start_index);

    void build_polygon();


    void add_another_point();

    static int find_start_point_index(const std::vector<Point> & polygon);
    bool is_in_polygon();
};
