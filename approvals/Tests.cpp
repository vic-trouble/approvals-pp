namespace testing { void Assert(bool); }
#define APPROVALS_ASSERT testing::Assert
#include "approvals.h"

#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>

using namespace approvals;

namespace testing // poor man's tests
  {
  namespace detail
    {
    bool succeeded = true;
    }

  bool Succeeded()
    {
    return detail::succeeded;
    }

  void Assert(bool outcome)
    {
    if (outcome)
      return;

    std::cerr << "failed\n";
    detail::succeeded = false;
    }
  }

void TestPrimitiveTypes()
{
  verify(13, "test.primitive.int");
  verify(3.14, "test.primitive.double");
  verify("hello", "test.primitive.string");
}

class Point
{
  friend std::ostream &operator << (std::ostream &os, const Point &ellipse);

public:
  explicit Point(float x = 0, float y = 0)
    : x_(x), y_(y)
  {
  }

private:
  float x_, y_;
};

std::ostream &operator << (std::ostream &os, const Point &point)
{
  os << "Point(x=" << std::fixed << std::setprecision(3) << point.x_ << ", y=" << point.y_ << ")";
  return os;
}

void TestUserObject()
{
  verify(Point(), "test.user-object");
}

void TestCollection()
{
  std::list<std::string> list = { "alpha", "bravo", "charlie" };
  verify(list, "test.list");

  std::vector<int> vec = { 13, 15, -1};
  verify(vec, "test.vector");

  std::set<double> set = { 3.14,2.79 };
  verify(set, "test.set");
}

void TestAssociativeCollection()
{
  std::map<std::string, Point> map = { {"alpha", Point(1, 2)},{"bravo", Point(3, 4)} };
  verify(map, "test.map");
}

void TestFunctions()
{
  std::list<double> range = { 0, 4, 9, 10 };
  auto sqrt_double = [](double x) -> double { return sqrt(x); };
  verify(sqrt_double, range, "test.function.1-arg");
  
  std::list<double> range_x = { 0, 1, 3 };
  std::list<double> range_y = { 0, 1, 4 };
  double(*phypot)(double, double) = std::hypot;
  verify(phypot, range_x, range_y, std::string("test.function.2-arg"));
    
  std::list<std::string> range1 = { "hello", "see", "Walter" };
  std::list<std::string> range2 = { "cruel", "you", " Edmund " };
  std::list<std::string> range3 = { "world", "later", "White" };
  auto concat = [](const std::string &left, const std::string &mid, const std::string &right) -> std::string
  {
    return left + mid + right;
  };
  verify(concat, range1, range2, range3, "test.function-3.arg");
}

void TestExceptions()
{
  auto safe_sqrt = [](double x) -> double
  {
    if (x < 0)
      {
      std::stringstream ss;
      ss << "negative value (" << std::fixed << x << ")";
      throw std::domain_error(ss.str());
      }

    return sqrt(x);
  };
  std::vector<double> range = { 0, 1, -1, 2 };
  verify(safe_sqrt, range, "test.exceptions");
}

int main(int argc, char **argv)
{
  set_diff("winmergeU");

  TestPrimitiveTypes();
  TestUserObject();
  TestCollection();
  TestAssociativeCollection();
  TestFunctions();
  TestExceptions();
  if (!testing::Succeeded())
    return 1;

  std::cout << "all ok\n";
  return 0;
}
