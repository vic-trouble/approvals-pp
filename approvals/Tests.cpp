#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <gtest/gtest.h>

#include "approvals.h"

using namespace approvals;

TEST(Approvals, PrimitiveTypes)
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
  os << (boost::format("Point(x=%.3f, y=%.3f)") % point.x_ % point.y_).str();
  return os;
}

TEST(Approvals, UserObject)
{
  verify(Point(), "test.user-object");
}

TEST(Approvals, Collection)
{
  using boost::assign::list_of;
  std::list<std::string> list = list_of<std::string>("alpha")("bravo")("charlie");
  verify(list, "test.list");

  std::vector<int> vec = list_of<int>(13)(15)(-1);
  verify(vec, "test.vector");

  std::set<double> set = list_of<double>(3.14)(2.79);
  verify(set, "test.set");
}

TEST(Approvals, AssociativeCollection)
{
  using boost::assign::map_list_of;
  std::map<std::string, Point> map = map_list_of("alpha", Point(1, 2))("bravo", Point(3, 4));
  verify(map, "test.map");
}

TEST(Approvals, Functions)
{
  using boost::assign::list_of;
  std::list<double> range = list_of<double>(0)(4)(9)(10);
  auto sqrt_double = [](double x) -> double { return sqrt(x); };
  verify(sqrt_double, range, "test.function.1-arg");

  std::list<double> range_x = list_of<double>(0)(1)(3);
  std::list<double> range_y = list_of<double>(0)(1)(4);
  verify(hypot, range_x, range_y, "test.function.2-arg");

  std::list<std::string> range1 = list_of<std::string>("hello")("see")("Walter");
  std::list<std::string> range2 = list_of<std::string>("cruel")("you")(" Edmund ");
  std::list<std::string> range3 = list_of<std::string>("world")("later")("White");
  auto concat = [](const std::string &left, const std::string &mid, const std::string &right) -> std::string
  {
    return left + mid + right;
  };
  verify(concat, range1, range2, range3, "test.function-3.arg");
}

TEST(Approvals, Exceptions)
{
  auto safe_sqrt = [](double x) -> double
  {
    if (x < 0)
      throw std::domain_error((boost::format("negative value (%f)") % x).str());

    return sqrt(x);
  };
  std::vector<double> range = boost::assign::list_of(0)(1)(-1)(2);
  verify(safe_sqrt, range, "test.exceptions");
}

void main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
}