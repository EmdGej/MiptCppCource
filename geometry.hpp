#include <algorithm>

#include "math.h"

class Vector;
class IShape;
class Point;
class Segment;
class Line;
class Ray;
class Circle;

// -------------------------------------------------------- //
// ---------------------- VECTOR -------------------------- //
// -------------------------------------------------------- //
class Vector {
 public:
  Vector();

  Vector(int64_t x_cor, int64_t y_cor);

  Vector(const Vector& other);

  Vector& operator=(const Vector& other);

  Vector& operator+=(const Vector& other);

  Vector operator+(const Vector& other) const;

  Vector& operator-=(const Vector& other);

  Vector operator-(const Vector& other) const;

  Vector& operator*=(int64_t num);

  int64_t operator*(const Vector& other) const;

  int64_t operator^(const Vector& other) const;

  Vector operator-() const;

  int64_t GetX() const;
  int64_t GetY() const;

  ~Vector();

 private:
  int64_t x_cor_;
  int64_t y_cor_;
};

Vector operator*(const Vector& vector, int64_t num);
Vector operator*(int64_t num, const Vector& vector);

// -------------------------------------------------------- //
// ---------------------- ISHAPE -------------------------- //
// -------------------------------------------------------- //
class IShape {
 public:
  virtual void Move(const Vector& vector) = 0;
  virtual bool ContainsPoint(const Point& point) const = 0;
  virtual bool CrossSegment(const Segment&) const = 0;
  virtual IShape* Clone() const = 0;
  virtual ~IShape();
};

// -------------------------------------------------------- //
// ---------------------- POINT --------------------------- //
// -------------------------------------------------------- //
class Point : public IShape {
 public:
  Point(int64_t x_cor, int64_t y_cor);

  Point(const Point& other);
  Point& operator=(const Point& other);

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  IShape* Clone() const override;

  int64_t GetX() const;
  int64_t GetY() const;

  Vector operator-(const Point& other) const;

  ~Point();

 private:
  int64_t x_cor_;
  int64_t y_cor_;
};

// -------------------------------------------------------- //
// --------------------- SEGMENT -------------------------- //
// -------------------------------------------------------- //
class Segment : public IShape {
 public:
  Segment(const Point& point1, const Point& point2);

  Segment(const Segment& other);
  Segment& operator=(const Segment& other);

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  IShape* Clone() const override;

  Point GetA() const;
  Point GetB() const;

  ~Segment();

 private:
  Point a_p_;
  Point b_p_;
};

// -------------------------------------------------------- //
// ------------------------ LINE -------------------------- //
// -------------------------------------------------------- //
class Line : public IShape {
 public:
  Line(const Point& point1, const Point& point2);

  Line(const Line& other);
  Line& operator=(const Line& other);

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  IShape* Clone() const override;

  int64_t GetA() const;
  int64_t GetB() const;
  int64_t GetC() const;

  ~Line();

 private:
  Point a_p_;
  Point b_p_;
  int64_t a_coeff_;
  int64_t b_coeff_;
  int64_t c_coeff_;
};

// -------------------------------------------------------- //
// ------------------------- RAY -------------------------- //
// -------------------------------------------------------- //
class Ray : public IShape {
 public:
  Ray(const Point& point1, const Point& point2);

  Ray(const Ray& other);
  Ray& operator=(const Ray& other);

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  IShape* Clone() const override;

  Point GetA() const;
  Vector GetVector() const;

  ~Ray();

 private:
  Point a_p_;
  Vector vector_;
};

// -------------------------------------------------------- //
// ---------------------- CIRCLE -------------------------- //
// -------------------------------------------------------- //
class Circle : public IShape {
 public:
  Circle(const Point& point, size_t radius);

  Circle(const Circle& other);
  Circle& operator=(const Circle& other);

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  IShape* Clone() const override;

  Point GetCentre() const;
  size_t GetRadius() const;

  ~Circle();

 private:
  Point centre_;
  size_t radius_;

  bool IsCrossed(double a_c, double b_c, double c_c,
                 const Segment& segment) const {
    double det_main = -a_c * a_c - b_c * b_c;
    double det_x =
        c_c * a_c - b_c * (b_c * centre_.GetX() - a_c * centre_.GetY());
    double det_y =
        a_c * (b_c * centre_.GetX() - a_c * centre_.GetY()) + b_c * c_c;
    double dist_to_seg =
        (abs(a_c * centre_.GetX() + b_c * centre_.GetY() + c_c)) /
        sqrt((a_c * a_c + b_c * b_c));
    if (dist_to_seg <= static_cast<double>(radius_)) {
      if (ContainsPoint(segment.GetA()) && ContainsPoint(segment.GetB())) {
        return pow(segment.GetA().GetX() - centre_.GetX(), 2) +
                       pow(segment.GetA().GetY() - centre_.GetY(), 2) ==
                   static_cast<int64_t>(radius_ * radius_) ||
               pow(segment.GetB().GetX() - centre_.GetX(), 2) +
                       pow(segment.GetB().GetY() - centre_.GetY(), 2) ==
                   static_cast<int64_t>(radius_ * radius_);
      }
      if (ContainsPoint(segment.GetA()) && !ContainsPoint(segment.GetB()) ||
          !ContainsPoint(segment.GetA()) && ContainsPoint(segment.GetB())) {
        return true;
      }
      if (!ContainsPoint(segment.GetA()) && !ContainsPoint(segment.GetB())) {
        double x_cor = det_x / det_main;
        double y_cor = det_y / det_main;
        return (std::min(segment.GetA().GetX(), segment.GetB().GetX()) <=
                    x_cor &&
                x_cor <=
                    std::max(segment.GetA().GetX(), segment.GetB().GetX())) &&
               (std::min(segment.GetA().GetY(), segment.GetB().GetY()) <=
                    y_cor &&
                y_cor <=
                    std::max(segment.GetA().GetY(), segment.GetB().GetY()));
      }
    }
    return false;
  }
};