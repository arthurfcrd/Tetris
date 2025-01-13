#include "point.hpp"

Point Point::operator+(const Point& other) const{
    return {x + other.x, y + other.y};
}

Point Point::operator-(const Point& other) const{
    return {x - other.x, y - other.y};
}

Point operator*(int scalar, const Point& point){ // operator for scalar * Point
    return {scalar * point.x, scalar * point.y};
}