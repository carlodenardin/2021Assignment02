#include <omp.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

struct Point {
    private:
        double x;
        double y;

    public:

        Point () = default;

        Point (double x, double y) : x{x}, y{y} {}

        double getX () const { return x; }
        double getY () const { return y; }
};

std::ostream& operator<<(std::ostream &out, const Point &point)
{
  return out << "Point (" << point.getX() << ", " << point.getY() << ")";
}

struct Node {
    private:
        Point split;
        Node * left;
        Node * right;
    
    public:
        Node () = default;

        Node(Point split) : split{split}, left{nullptr}, right{nullptr} {}

        Node * getLeft () { return left; }
        Node * getRight () { return right; }
        void setLeft (Node * _left) { left = _left; }
        void setRight (Node * _right) { left = _right; }
        Point getPoint () { return split; }
    
};

template <typename Iterator>
void print(Iterator start, Iterator end) 
{   
    for (auto it = start; it != end; ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n\n\n";
}

template <typename Iterator>
Point chooseSplit(Iterator start, Iterator median, Iterator end, int axis) 
{   
    if (axis == 0) {
        std::nth_element(start, median, end,
            [](const Point &point1, const Point &point2) { 
                return point1.getX() < point2.getX(); 
            }
        );
    } else {
        std::nth_element(start, median, end,
            [](const Point &point1, const Point &point2) { 
                return point1.getY() < point2.getY(); 
            }
        );
    }

    return *median;
}

Node * buildTree (std::vector<Point> &points, int start, int end, int dim, int axis) {

    if (end - start <= 0) {
        Node * node = new Node(points[start]);
        node -> setLeft(nullptr);
        node -> setRight(nullptr);
        return node;
    }

    int median = (start + end) / 2;
    
    Point split = chooseSplit(points.begin() + start, points.begin() + median, points.begin() + end + 1, axis);
    
    Node * node = new Node(split);

    int _axis = (axis + 1) % dim;

    #pragma omp task shared(points)
    {
        node -> setLeft(buildTree(points, start, median - 1, dim, _axis));
    }

    #pragma omp task shared(points)
    {
        node -> setRight(buildTree(points, median + 1, end, dim, _axis));
    }

    return node;
}

int main (int argc, char *argv[]) {

    std::vector<Point> points = {Point {30, 40}, Point {5, 25}, Point {10, 12}, Point {70, 70}, Point {50, 30}};
    
    #pragma omp parallel shared(points)
    {
        #pragma omp single
        {
            Node * n = buildTree(points, 0, points.size() - 1, 2, 0);
        }
    }

    return 0;
}
