#include "geometry.hpp"

IShape::~IShape(){};

// ========================================================= //
// ==================== DEFAULT CONSTRUCTOR ================ //
// ========================================================= //
Vector::Vector() : x_cor_(0), y_cor_(0) {}

// ========================================================= //
// ==================== CONSTRUCTOR X Y ==================== //
// ========================================================= //
Vector::Vector(int64_t x_cor, int64_t y_cor) : x_cor_(x_cor), y_cor_(y_cor) {}

// ========================================================= //
// ==================== COPY CONSTRUCTOR =================== //
// ========================================================= //
Vector::Vector(const Vector& other)
    : x_cor_(other.x_cor_), y_cor_(other.y_cor_) {}

// ========================================================= //
// ====================== OPERATOR "=" ===================== //
// ========================================================= //
Vector& Vector::operator=(const Vector& other) {
  x_cor_ = other.x_cor_;
  y_cor_ = other.y_cor_;

  return *this;
}

// ========================================================= //
// ====================== OPERATOR "+=" ==================== //
// ========================================================= //
Vector& Vector::operator+=(const Vector& other) {
  x_cor_ += other.x_cor_;
  y_cor_ += other.y_cor_;

  return *this;
}

// ========================================================= //
// ====================== OPERATOR "+" ===================== //
// ========================================================= //
Vector Vector::operator+(const Vector& other) const {
  Vector copy = *this;
  copy += other;

  return copy;
}

// ========================================================= //
// ====================== OPERATOR "-=" ==================== //
// ========================================================= //
Vector& Vector::operator-=(const Vector& other) {
  x_cor_ -= other.x_cor_;
  y_cor_ -= other.y_cor_;

  return *this;
}

// ========================================================= //
// ====================== OPERATOR "-" ===================== //
// ========================================================= //
Vector Vector::operator-(const Vector& other) const {
  Vector copy = *this;
  copy -= other;

  return copy;
}

// ========================================================= //
// ====================== OPERATOR "*=" ==================== //
// ========================================================= //
Vector& Vector::operator*=(int64_t num) {
  x_cor_ *= num;
  y_cor_ *= num;

  return *this;
}

// ========================================================= //
// ====================== OPERATOR "*" ===================== //
// ========================================================= //
Vector operator*(const Vector& vector, int64_t num) {
  Vector copy = vector;
  copy *= num;

  return copy;
}

Vector operator*(int64_t num, const Vector& vector) {
  Vector copy = vector;
  copy *= num;

  return copy;
}

// ========================================================= //
// ======================== SCALAR "*" ===================== //
// ========================================================= //
int64_t Vector::operator*(const Vector& other) const {
  return x_cor_ * other.x_cor_ + y_cor_ * other.y_cor_;
}

// ========================================================= //
// ======================== VECTOR "^" ===================== //
// ========================================================= //
int64_t Vector::operator^(const Vector& other) const {
  return x_cor_ * other.y_cor_ - y_cor_ * other.x_cor_;
}

// ========================================================= //
// ======================== UNARY "-" ====================== //
// ========================================================= //
Vector Vector::operator-() const {
  Vector copy = *this;
  copy *= -1;

  return copy;
}

// ========================================================= //
// ========================== GetX ========================= //
// ========================================================= //
int64_t Vector::GetX() const { return x_cor_; }

// ========================================================= //
// ========================== GetY ========================= //
// ========================================================= //
int64_t Vector::GetY() const { return y_cor_; }

// ========================================================= //
// ======================= DESTRUCTOR ====================== //
// ========================================================= //
Vector::~Vector() = default;

// -------------------------------------------------------- //
// ---------------------- POINT --------------------------- //
// -------------------------------------------------------- //
Point::Point(int64_t x_cor, int64_t y_cor) : x_cor_(x_cor), y_cor_(y_cor) {}

Point::Point(const Point& other) : x_cor_(other.x_cor_), y_cor_(other.y_cor_) {}

Point& Point::operator=(const Point& other) {
  x_cor_ = other.x_cor_;
  y_cor_ = other.y_cor_;

  return *this;
}

int64_t Point::GetX() const { return x_cor_; }
int64_t Point::GetY() const { return y_cor_; }

Vector Point::operator-(const Point& other) const {
  Vector result(x_cor_ - other.x_cor_, y_cor_ - other.y_cor_);

  return result;
}

void Point::Move(const Vector& vector) {
  x_cor_ += vector.GetX();
  y_cor_ += vector.GetY();
}

bool Point::ContainsPoint(const Point& point) const {
  return (x_cor_ == point.x_cor_) && (y_cor_ == point.y_cor_);
}

bool Point::CrossSegment(const Segment& segment) const {
  int64_t a_coeff = segment.GetB().GetY() - segment.GetA().GetY();
  int64_t b_coeff = segment.GetA().GetX() - segment.GetB().GetX();
  int64_t c_coeff = segment.GetA().GetY() * segment.GetB().GetX() -
                    segment.GetA().GetX() * segment.GetB().GetY();

  return (a_coeff * x_cor_ + b_coeff * y_cor_ + c_coeff == 0) &&
         (std::min(segment.GetA().GetX(), segment.GetB().GetX()) <= x_cor_ &&
          x_cor_ <= std::max(segment.GetA().GetX(), segment.GetB().GetX())) &&
         (std::min(segment.GetA().GetY(), segment.GetB().GetY()) <= y_cor_ &&
          y_cor_ <= std::max(segment.GetA().GetY(), segment.GetB().GetY()));
}

IShape* Point::Clone() const {
  IShape* ptr = new Point(*this);

  return ptr;
}

Point::~Point() = default;

// -------------------------------------------------------- //
// ---------------------- SEGMENT ------------------------- //
// -------------------------------------------------------- //
Segment::Segment(const Point& point1, const Point& point2)
    : a_p_(point1), b_p_(point2) {}

Segment::Segment(const Segment& other) : a_p_(other.a_p_), b_p_(other.b_p_) {}

Segment& Segment::operator=(const Segment& other) {
  a_p_ = other.a_p_;
  b_p_ = other.b_p_;

  return *this;
}

Point Segment::GetA() const { return a_p_; }
Point Segment::GetB() const { return b_p_; }

void Segment::Move(const Vector& vector) {
  a_p_ = Point(a_p_.GetX() + vector.GetX(), a_p_.GetY() + vector.GetY());
  b_p_ = Point(b_p_.GetX() + vector.GetX(), b_p_.GetY() + vector.GetY());
}

bool Segment::ContainsPoint(const Point& point) const {
  return point.CrossSegment(*this);
}

bool Segment::CrossSegment(const Segment& segment) const {
  int64_t a1_c = b_p_.GetY() - a_p_.GetY();
  int64_t b1_c = a_p_.GetX() - b_p_.GetX();
  int64_t c1_c = a_p_.GetY() * b_p_.GetX() - a_p_.GetX() * b_p_.GetY();

  int64_t a2_c = segment.b_p_.GetY() - segment.a_p_.GetY();
  int64_t b2_c = segment.a_p_.GetX() - segment.b_p_.GetX();
  int64_t c2_c = segment.a_p_.GetY() * segment.b_p_.GetX() -
                 segment.a_p_.GetX() * segment.b_p_.GetY();

  int64_t det_main = a1_c * b2_c - a2_c * b1_c;
  int64_t det_x = c1_c * b2_c - b1_c * c2_c;
  int64_t det_y = a1_c * c2_c - a2_c * c1_c;

  if (det_main == 0 && det_x == 0 && det_y == 0) {
    return ContainsPoint(segment.GetA()) || ContainsPoint(segment.GetB()) ||
           segment.ContainsPoint(a_p_) || segment.ContainsPoint(b_p_);
  }

  return ((a1_c * segment.a_p_.GetX() + b1_c * segment.a_p_.GetY() + c1_c >=
               0 &&
           a1_c * segment.b_p_.GetX() + b1_c * segment.b_p_.GetY() + c1_c <=
               0) ||
          (a1_c * segment.a_p_.GetX() + b1_c * segment.a_p_.GetY() + c1_c <=
               0 &&
           a1_c * segment.b_p_.GetX() + b1_c * segment.b_p_.GetY() + c1_c >=
               0)) &&
         ((a2_c * a_p_.GetX() + b2_c * a_p_.GetY() + c2_c >= 0 &&
           a2_c * b_p_.GetX() + b2_c * b_p_.GetY() + c2_c <= 0) ||
          (a2_c * a_p_.GetX() + b2_c * a_p_.GetY() + c2_c <= 0 &&
           a2_c * b_p_.GetX() + b2_c * b_p_.GetY() + c2_c >= 0));
}

IShape* Segment::Clone() const {
  IShape* ptr = new Segment(*this);
  return ptr;
}

Segment::~Segment() = default;

// -------------------------------------------------------- //
// ------------------------ LINE -------------------------- //
// -------------------------------------------------------- //
Line::Line(const Point& point1, const Point& point2)
    : a_p_(point1), b_p_(point2) {
  a_coeff_ = b_p_.GetY() - a_p_.GetY();
  b_coeff_ = a_p_.GetX() - b_p_.GetX();
  c_coeff_ = a_p_.GetY() * b_p_.GetX() - a_p_.GetX() * b_p_.GetY();
}

Line::Line(const Line& other)
    : a_p_(other.a_p_),
      b_p_(other.b_p_),
      a_coeff_(other.a_coeff_),
      b_coeff_(other.b_coeff_),
      c_coeff_(other.c_coeff_) {}

Line& Line::operator=(const Line& other) {
  a_p_ = other.a_p_;
  b_p_ = other.b_p_;
  a_coeff_ = other.a_coeff_;
  b_coeff_ = other.b_coeff_;
  c_coeff_ = other.c_coeff_;

  return *this;
}

int64_t Line::GetA() const { return a_coeff_; }
int64_t Line::GetB() const { return b_coeff_; }
int64_t Line::GetC() const { return c_coeff_; }

void Line::Move(const Vector& vector) {
  a_p_ = Point(a_p_.GetX() + vector.GetX(), a_p_.GetY() + vector.GetY());
  b_p_ = Point(b_p_.GetX() + vector.GetX(), b_p_.GetY() + vector.GetY());
  a_coeff_ = b_p_.GetY() - a_p_.GetY();
  b_coeff_ = a_p_.GetX() - b_p_.GetX();
  c_coeff_ = a_p_.GetY() * b_p_.GetX() - a_p_.GetX() * b_p_.GetY();
}

bool Line::ContainsPoint(const Point& point) const {
  return a_coeff_ * point.GetX() + b_coeff_ * point.GetY() + c_coeff_ == 0;
}

bool Line::CrossSegment(const Segment& segment) const {
  return (a_coeff_ * segment.GetA().GetX() + b_coeff_ * segment.GetA().GetY() +
                  c_coeff_ >=
              0 &&
          a_coeff_ * segment.GetB().GetX() + b_coeff_ * segment.GetB().GetY() +
                  c_coeff_ <=
              0) ||
         (a_coeff_ * segment.GetA().GetX() + b_coeff_ * segment.GetA().GetY() +
                  c_coeff_ <=
              0 &&
          a_coeff_ * segment.GetB().GetX() + b_coeff_ * segment.GetB().GetY() +
                  c_coeff_ >=
              0);
}

IShape* Line::Clone() const {
  IShape* ptr = new Line(*this);
  return ptr;
}

Line::~Line() = default;

// -------------------------------------------------------- //
// ------------------------ RAY --------------------------- //
// -------------------------------------------------------- //
Ray::Ray(const Point& point1, const Point& point2)
    : a_p_(point1),
      vector_(Vector((point2 - point1).GetX(), (point2 - point1).GetY())) {}

Ray::Ray(const Ray& other) : a_p_(other.a_p_), vector_(other.vector_) {}

Ray& Ray::operator=(const Ray& other) {
  a_p_ = other.a_p_;
  vector_ = other.vector_;

  return *this;
}

Point Ray::GetA() const { return a_p_; }
Vector Ray::GetVector() const { return vector_; }

void Ray::Move(const Vector& vector) {
  a_p_ = Point(a_p_.GetX() + vector.GetX(), a_p_.GetY() + vector.GetY());
}

bool Ray::ContainsPoint(const Point& point) const {
  Vector vector_to_point(point.GetX() - a_p_.GetX(),
                         point.GetY() - a_p_.GetY());
  return (vector_.GetY() * point.GetX() - vector_.GetX() * point.GetY() +
              (a_p_.GetY() * vector_.GetX() - a_p_.GetX() * vector_.GetY()) ==
          0) &&
         (vector_to_point * vector_ >= 0);
}

bool Ray::CrossSegment(const Segment& segment) const {
  Vector vector_to_a(segment.GetA().GetX() - a_p_.GetX(),
                     segment.GetA().GetY() - a_p_.GetY());
  Vector vector_to_b(segment.GetB().GetX() - a_p_.GetX(),
                     segment.GetB().GetY() - a_p_.GetY());
  Vector result_vector = vector_to_a + vector_to_b;

  return (((vector_.GetY() * segment.GetA().GetX() -
                vector_.GetX() * segment.GetA().GetY() +
                (a_p_.GetY() * vector_.GetX() - a_p_.GetX() * vector_.GetY()) >=
            0) &&
           (vector_.GetY() * segment.GetB().GetX() -
                vector_.GetX() * segment.GetB().GetY() +
                (a_p_.GetY() * vector_.GetX() - a_p_.GetX() * vector_.GetY()) <=
            0)) ||
          ((vector_.GetY() * segment.GetA().GetX() -
                vector_.GetX() * segment.GetA().GetY() +
                (a_p_.GetY() * vector_.GetX() - a_p_.GetX() * vector_.GetY()) <=
            0) &&
           (vector_.GetY() * segment.GetB().GetX() -
                vector_.GetX() * segment.GetB().GetY() +
                (a_p_.GetY() * vector_.GetX() - a_p_.GetX() * vector_.GetY()) >=
            0))) &&
         (result_vector.GetX() * vector_.GetX() >= 0 &&
          result_vector.GetY() * vector_.GetY() >= 0);
}

IShape* Ray::Clone() const {
  IShape* ptr = new Ray(*this);
  return ptr;
}

Ray::~Ray() = default;

// -------------------------------------------------------- //
// ---------------------- CIRCLE -------------------------- //
// -------------------------------------------------------- //
Circle::Circle(const Point& point, size_t radius)
    : centre_(point), radius_(radius) {}

Circle::Circle(const Circle& other)
    : centre_(other.centre_), radius_(other.radius_) {}

Circle& Circle::operator=(const Circle& other) {
  centre_ = other.centre_;
  radius_ = other.radius_;

  return *this;
}

Point Circle::GetCentre() const { return centre_; }
size_t Circle::GetRadius() const { return radius_; }

void Circle::Move(const Vector& vector) {
  centre_ =
      Point(centre_.GetX() + vector.GetX(), centre_.GetY() + vector.GetY());
}

bool Circle::ContainsPoint(const Point& point) const {
  return (centre_.GetX() - point.GetX()) * (centre_.GetX() - point.GetX()) +
             (centre_.GetY() - point.GetY()) *
                 (centre_.GetY() - point.GetY()) <=
         static_cast<int64_t>(radius_ * radius_);
}

bool Circle::CrossSegment(const Segment& segment) const {
  double a_c = segment.GetB().GetY() - segment.GetA().GetY();
  double b_c = segment.GetA().GetX() - segment.GetB().GetX();
  double c_c = segment.GetA().GetY() * segment.GetB().GetX() -
               segment.GetA().GetX() * segment.GetB().GetY();

  return IsCrossed(a_c, b_c, c_c, segment);
}

IShape* Circle::Clone() const {
  IShape* ptr = new Circle(*this);
  return ptr;
}

Circle::~Circle() = default;
