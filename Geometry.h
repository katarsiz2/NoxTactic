#pragma once
#include <string>
#include <sstream>
#include <math.h> 
using std::string;
using std::stringstream;
using std::abs;
using std::make_pair;
#define EPS 1E-9
#define tg225 0.414213562373095
#define tg675 2.41421356237309

enum Direction
{
	NO_DIRECTION,
	UP,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT,
	UP_LEFT
};
typedef std::pair<Direction, Direction> Direction2;
template<class T> struct Coord
{
	T x, y;
    typedef T valuetype;
    Coord(T x, T y): x(x), y(y) {}
    Coord(): x(), y() {}
    //directional vector
    Coord(Direction dir) {
        switch(dir){
        case NO_DIRECTION:
            x = 0; y = 0;
            break;
        case UP:
            x = 0; y = -1;
            break;
        case DOWN:
            x = 0; y = 1;
            break;
        case UP_LEFT:
            x = -1; y = -1;
            break;
        case DOWN_LEFT:
            x = -1; y = 1;
            break;
        case UP_RIGHT:
            x = 1; y = -1;
            break;
        case DOWN_RIGHT:
            x = 1; y = 1;
            break;
        case LEFT:
            x = -1; y = 0;
            break;
        case RIGHT:
            x = 1; y = 0;
        };
    }
	//Vector(const Vector &a); //generated automatically
	Coord operator+(const Coord<T> &a) const { return Coord<T>(x+a.x, y+a.y);}
	Coord operator*(const Coord<T> &a) const { return Coord<T>(x*a.x, y*a.y);}
	Coord operator/(const Coord<T> &a) const { return Coord<T>(x/a.x, y/a.y);}
    Coord operator-(const Coord<T> &a) const { return Coord<T>(x-a.x, y-a.y);}
    Coord& operator+=(const Coord<T> &a) { x+=a.x; y+=a.y; return *this;}
    Coord& operator*=(const Coord<T> &a) { x*=a.x; y*=a.y; return *this;}
    Coord& operator-=(const Coord<T> &a) { x -= a.x; y -= a.y; return *this; }
    Coord& operator/=(const Coord<T> &a) { x /= a.x; y /= a.y; return *this; }
    double length() { return sqrt(static_cast<double>(x*x + y*y));}
	int intlength(bool diagonal) {
        if (typeid(T) != typeid(int)) {
            throw("WRONG OPERATION");
        }
	    if (diagonal) {
		    return std::max(abs(x), abs(y));
	    } else {
		    return abs(x) + abs(y);
        }; 
    }
	string toString(bool squared = true) const {
        stringstream ss;
        if (squared) {
            ss << "[" << x << "," << y << "]";
        } else {
            ss << "(" << x << "," << y << ")";
        }
	    return ss.str();
    }
    bool operator==(const Coord<T> &a) const {
        if (std::is_integral<T>::value) {
            return x == a.x && y == a.y;
        } else {
            return abs(x-a.x) < EPS && abs(y-a.y) < EPS;
        }
        
    }
    bool operator!=(const Coord<T> &a) const { return !(*this == a); }
    Coord operator-() { return Coord<T>(-x, -y); }
	//~Vector();
    Direction getDirection(const Coord<T> &a) const {
        if (*this == a) {return NO_DIRECTION;}
        Coord<T> dif = a - *this;
        if (dif.x == 0 && dif.y > 0) { return DOWN; }
        if (dif.x == 0 && dif.y < 0) { return UP; }
        if (dif.y == 0 && dif.x > 0) { return RIGHT; }
        if (dif.y == 0 && dif.x < 0) { return LEFT; }
        
        double tg = abs(dif.y / dif.x);
        if (tg < tg225) { //horizontal
            if (dif.x > 0) {return RIGHT;} else {return LEFT;}
        } else if (tg <= tg675) { //diagonal
            if (dif.x > 0 && dif.y > 0) { return DOWN_RIGHT; }
            if (dif.x > 0 && dif.y < 0) { return UP_RIGHT; }
            if (dif.x < 0 && dif.y > 0) { return DOWN_LEFT; }
            if (dif.x < 0 && dif.y < 0) { return UP_LEFT; }
        } else { //vertical
            if (dif.y > 0) {return DOWN;} else {return UP;}
        }
        return NO_DIRECTION;
    }
    Direction2 getDirection2(const Coord<T> &a) const {
        if (*this == a) {return make_pair(NO_DIRECTION, NO_DIRECTION);}
        Coord<T> dif = a - *this;
        if (dif.x == 0 && dif.y > 0) { return make_pair(DOWN, DOWN); }
        if (dif.x == 0 && dif.y < 0) { return make_pair(UP, UP); }
        if (dif.y == 0 && dif.x > 0) { return make_pair(RIGHT, RIGHT); }
        if (dif.y == 0 && dif.x < 0) { return make_pair(LEFT, LEFT); }
        
        double tg = abs(dif.y / dif.x);
        if (tg < tg225) { //horizontal
            if (dif.x > 0 && dif.y > 0) { return make_pair(RIGHT, DOWN_RIGHT); }
            if (dif.x > 0 && dif.y < 0) { return make_pair(RIGHT, UP_RIGHT); }
            if (dif.x < 0 && dif.y > 0) { return make_pair(LEFT, DOWN_LEFT); }
            if (dif.x < 0 && dif.y < 0) { return make_pair(LEFT, UP_LEFT); }
        } else if (tg <= tg675) { //diagonal
            if (dif.x > 0 && dif.y > 0) { 
                if (tg > 1) { return make_pair(DOWN_RIGHT, DOWN); }
                else if (tg < 1) { return make_pair(DOWN_RIGHT, RIGHT); }
                else { return make_pair(DOWN_RIGHT, DOWN_RIGHT); }
            }
            if (dif.x > 0 && dif.y < 0) { 
                if (tg > 1) { return make_pair(UP_RIGHT, UP); }
                else if (tg < 1) { return make_pair(UP_RIGHT, RIGHT); }
                else { return make_pair(UP_RIGHT, UP_RIGHT); }
            }
            if (dif.x < 0 && dif.y > 0) { 
                if (tg > 1) { return make_pair(DOWN_LEFT, DOWN); }
                else if (tg < 1) { return make_pair(DOWN_LEFT, LEFT); }
                else { return make_pair(DOWN_LEFT, DOWN_LEFT); }
            }
            if (dif.x < 0 && dif.y < 0) { 
                if (tg > 1) { return make_pair(UP_LEFT, UP); }
                else if (tg < 1) { return make_pair(UP_LEFT, LEFT); }
                else { return make_pair(UP_LEFT, UP_LEFT); }
            }
        } else { //vertical
            if (dif.x > 0 && dif.y > 0) { return make_pair(DOWN, DOWN_RIGHT); }
            if (dif.x > 0 && dif.y < 0) { return make_pair(UP, UP_RIGHT); }
            if (dif.x < 0 && dif.y > 0) { return make_pair(DOWN, DOWN_LEFT); }
            if (dif.x < 0 && dif.y < 0) { return make_pair(UP, UP_LEFT); }
        }
        return make_pair(NO_DIRECTION, NO_DIRECTION);
    }
    bool isInside (const Coord<T>& v1, const Coord<T>& v2) const {
        return (x <= v2.x && x >= v1.x && y <= v2.y && y >= v1.y);
    }
    template<class T2> operator Coord<T2>() const { return Coord<T2>((T2)x, (T2)y); }
};

//template<class T1, class T2> operator Vector<T2>(const Vector<T1>& a) { return Vector<T2>((T2)a.x, (T2)a.y); }

template<class T> Coord<T> operator*(const T x, const Coord<T>& v) {
	return Coord<T>(v.x * x, v.y * x);
}
template<class T> Coord<T> operator*(const Coord<T>& v, const T x) {
	return Coord<T>(v.x * x, v.y * x);
}
template<class T> Coord<T> operator/(const T x, const Coord<T>& v) {
	return Coord<T>(v.x / x, v.y / x);
}
template<class T> Coord<T> operator/(const Coord<T>& v, const T x) {
	return Coord<T>(v.x / x, v.y / x);
}
#define PI 3.14159265
typedef Coord<int> CoordI;
typedef Coord<double> CoordD;
typedef Coord<long> CoordL;


enum ReflectType
{
    REFLECT_POINT,
    REFLECT_VERTICAL,
    REFLECT_HORIZONTAL
};
class Angle
{
    int degrees; //0 is at Ox direction; + is clock-wise
public:
    Angle(int degrees): degrees(degrees) {
        emboundDegrees();
    }
    Angle(): degrees(0) {}
    Angle(const CoordI& start, const CoordI& dest) {
        if (start == dest) {
            throw ("bad vector");
        }

        if (start.y == dest.y) {
            if (dest.x > start.x) {
                degrees = 0;
            } else {
                degrees = 180;
            }
            return;
        }
        if (start.x == dest.x) {
            if (dest.y > start.y) {
                degrees = 90;
            } else {
                degrees = 270;
            }
            return;
        }

        degrees = static_cast<int>(atan2(static_cast<double>(dest.y-start.y), dest.x-start.x) * 180 / PI);
        emboundDegrees();
    }
    Angle(const CoordD& start, const CoordD& dest) {
        if (start == dest) {
            throw ("bad vector");
        }

        if (start.y == dest.y) {
            if (dest.x > start.x) {
                degrees = 0;
            } else {
                degrees = 180;
            }
            return;
        }
        if (start.x == dest.x) {
            if (dest.y > start.y) {
                degrees = 90;
            } else {
                degrees = 270;
            }
            return;
        }

        degrees = static_cast<int>(atan2(static_cast<double>(dest.y-start.y), dest.x-start.x) * 180 / PI);
        emboundDegrees();
    }
    Angle(const Direction dir) {
        switch (dir) {
        case RIGHT:
            degrees = 0;
            break;
        case DOWN_RIGHT:
            degrees = 45;
            break;
        case DOWN:
            degrees = 90;
            break;
        case DOWN_LEFT:
            degrees = 135;
            break;
        case LEFT:
            degrees = 180;
            break;
        case UP_LEFT:
            degrees = 225;
            break;
        case UP:
            degrees = 270;
            break;
        case UP_RIGHT:
            degrees = 315;
            break;
        case NO_DIRECTION:
            throw("wrong direction");
        };
    }
    void emboundDegrees() {
        while (degrees < 0) {
            degrees += 360;
        }
        while (degrees >= 360) {
            degrees -= 360;
        }
    }

    void reflect(ReflectType type) {
        switch (type) {
        case REFLECT_POINT:
            degrees = degrees + 180;
            break;
        case REFLECT_VERTICAL:
            degrees = 360 - degrees;
            break;
        case REFLECT_HORIZONTAL:
            degrees = 180 - degrees;
            break;
        };
        emboundDegrees();
    }
    Direction getDirection() const {
        //dividing into quadrants and analyzing an in-quadrant deviation
        int tmp = degrees % 90;
        if (degrees < 90) {
            if (tmp < 22) {
                return RIGHT;
            } else if (tmp < 68) {
                return DOWN_RIGHT;
            } else {
                return DOWN;
            }
        } else if (degrees < 180) {
            if (tmp < 22) {
                return DOWN;
            } else if (tmp < 68) {
                return DOWN_LEFT;
            } else {
                return LEFT;
            }
        } else if (degrees < 270) {
            if (tmp < 22) {
                return LEFT;
            } else if (tmp < 68) {
                return UP_LEFT;
            } else {
                return UP;
            }
        } else {
            if (tmp < 22) {
                return UP;
            } else if (tmp < 68) {
                return UP_RIGHT;
            } else {
                return RIGHT;
            }
        }
    }
    Direction2 getDirection2() const {
        //dividing into quadrants and analyzing an in-quadrant deviation
        int tmp = degrees % 90;
        if (degrees < 90) {
            if (tmp == 0) {
                return make_pair(RIGHT, RIGHT);
            } else if (tmp < 22) {
                return make_pair(RIGHT, DOWN_RIGHT);
            } else if (tmp < 45) {
                return make_pair(DOWN_RIGHT, RIGHT);
            } else if (tmp < 68) {
                return make_pair(DOWN_RIGHT, DOWN);
            } else {
                return make_pair(DOWN, DOWN_RIGHT);
            }
        } else if (degrees < 180) {
            if (tmp == 0) {
                return make_pair(DOWN, DOWN);
            } else if (tmp < 22) {
                return make_pair(DOWN, DOWN_LEFT);
            } else if (tmp < 45) {
                return make_pair(DOWN_LEFT, DOWN);
            } else if (tmp < 68) {
                return make_pair(DOWN_LEFT, LEFT);
            } else {
                return make_pair(LEFT, DOWN_LEFT);
            }
        } else if (degrees < 270) {
            if (tmp == 0) {
                return make_pair(LEFT, LEFT);
            } else if (tmp < 22) {
                return make_pair(LEFT, UP_LEFT);
            } else if (tmp < 45) {
                return make_pair(UP_LEFT, LEFT);
            } else if (tmp < 68) {
                return make_pair(UP_LEFT, UP);
            } else {
                return make_pair(UP, UP_LEFT);
            }
        } else {
            if (tmp == 0) {
                return make_pair(UP, UP);
            } else if (tmp < 22) {
                return make_pair(UP, UP_RIGHT);
            } else if (tmp < 45) {
                return make_pair(UP_RIGHT, UP);
            } else if (tmp < 68) {
                return make_pair(UP_RIGHT, RIGHT);
            } else {
                return make_pair(RIGHT, UP_RIGHT);
            }
        }
    }
    Direction getMainDirection() const { 
        if (degrees < 45 || degrees > 315) { return RIGHT; }
        if (degrees < 135) { return DOWN; }
        if (degrees < 225) { return LEFT; }
        return UP;
    }
    double tg() const { 
        return tan(static_cast<double>(degrees) * PI / 180); 
    }
    int getDegrees() const { return degrees; }
    // returns degrees between two angles (by shortest way);
    int getDegreesBetween(const Angle& ang) const {
        int tmp = abs(degrees - ang.degrees);
        if (tmp > 180) {
            tmp = 360 - tmp;
        }
        return tmp;

    }
};
class Line
{
    double a, b, c;
public:
    Line(double a, double b, double c): a(a), b(b), c(c) {}
    Line(double k, double b): a(-k), b(1.0), c(b) {}
    Line(const CoordD& point1, const CoordD& point2) {
        if (point1.x == point2.x) {
            b = 0;
            a = 1;
            c = point1.x;
        } else {
            b = 1;
            a = (point2.y - point1.y) / (point1.x - point2.x);
            c = -(a * point1.x + point1.y);
        }
    }
    Line(const CoordD& point, Direction dir) {
        *this = Line(point, point+CoordD(dir));
    }
    Line(const CoordD& point, Angle angle) {
        if (angle.getDegrees() == 270 || angle.getDegrees() == 90) {
            *this = Line(point, UP);
        } else {
            *this = Line(point, point+CoordD(1.0, angle.tg()));
        }
    }
    Line(const Line& line, const CoordD& point) {
        *this = Line(point, line.angle());
    }
    Angle angle() const {
        if (b == 0) {
            return Angle(CoordD(0, 0), CoordD(0, 1));
        } else {
            return Angle(CoordD(0, -c/b), CoordD(1, (-c-a)/b));
        }
    }
    Angle reflect(Angle angle) const {
        return Angle(2*this->angle().getDegrees() - angle.getDegrees());
    }
    Line getOrtogonal() const { 
        if (a == 0) {
            return Line(1, 0, 0);
        } else {
            return Line(-b/a, 1, 0);
        }
    }
    Line getOrtogonal(const CoordD& point) {
        return Line(getOrtogonal(), point);
    }
    bool isParallel(const Line& line) {
        return (a * line.b - line.a * b) == 0;
    }
    CoordD getIntersectionPoint(const Line& line) {
        double y = (line.a * c - line.c * a) / (a*line.b - b * line.a);
        double x = (-c - b*y) / a;
        return CoordD(x, y);
    }
};
class RectangleIterator
{
    CoordI start, dest, coor;
public:
    RectangleIterator(const CoordI& start, const CoordI& dest): start(start), dest(dest), coor(start) {}
    void operator++() {
        if (isInside()) {
            coor.x++;
            if (coor.x > dest.x) {
                coor.x = start.x;
                coor.y++;
            }
        }
    }
    CoordI getCoor() const { return coor; }
    bool isInside() const { return coor.y <= dest.y; }
};
template<class T>
Coord<T> minCoord(const Coord<T>& a, const Coord<T>& b) {
    return Coord<T>(min(a.x, b.x), min(a.y, b.y));
}
template<class T>
Coord<T> maxCoord(const Coord<T>& a, const Coord<T>& b) {
    return Coord<T>(max(a.x, b.x), max(a.y, b.y));
}
const CoordI Coord1 = CoordI(1, 1);
const CoordI Coord0 = CoordI(0, 0);
const Angle AngleUp = Angle(270);
const Angle AngleLeft = Angle(180);
const Angle AngleDown = Angle(90);
const Angle AngleRight = Angle(0);
const Line LineVertical = Line(Coord0, UP);
const Line LineHorizontal = Line(Coord0, RIGHT);

//emulates projecting linear array into 2d grid;
//is used to automate getting coordinates of elements
//first expansive direction is called main
//example: main direction is RIGHT
//1 2 3 4
//5 6 7 8
template <class T>
class CoorArray2D
{
    Coord<T> coor, element_size;
    int main_size; //is counted in elements
    Coord<T> main_vector, secondary_vector;
public:
    CoorArray2D(Coord<T> coor, Coord<T> element_size, int main_size, Direction main_direction, Direction secondary_direction,
        T main_indent, T secondary_indent): coor(coor), element_size(element_size), main_size(main_size), 
        main_vector(Coord<T>(main_direction) * (Coord<T>(main_indent, main_indent) + element_size)),
        secondary_vector(Coord<T>(secondary_direction) * (Coord<T>(secondary_indent, secondary_indent) + element_size)) {}
    //starts with 0
    Coord<T> getCoor(int num) const {
        return coor + (num % main_size) * main_vector + (num / main_size) * secondary_vector;
    }
};
