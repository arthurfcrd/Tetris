#pragma once

struct Point{
    int x;
    int y;
    // operator for adding points
    Point operator+(const Point& other) const;
    // operator for subtracting points
    Point operator-(const Point& other) const;
};
Point operator*(int scalar, const Point& point); // operator for scalar * Point