// Copyright: 2014, 2015, Ableton AG, Berlin. All rights reserved.

#include <atria/xform/concepts.hpp>
#include <atria/xform/into.hpp>
#include <atria/xform/reducing/first_rf.hpp>
#include <atria/xform/transducer/take.hpp>
#include <atria/xform/transducer/cat.hpp>
#include <atria/xform/transducer/map.hpp>

#include <atria/testing/spies.hpp>
#include <atria/testing/gtest.hpp>

namespace atria {
namespace xform {

TEST(take, concept)
{
  meta::check<Transparent_transducer_spec(decltype(take(42)))>();
}

TEST(take, take)
{
  auto v = std::vector<int> { 1, 2, 3, 4, 5 };

  auto res = into(std::vector<int> {}, take(3), v);
  EXPECT_EQ(res, (std::vector<int> { 1, 2, 3 }));
}

TEST(take, take_cat_terminates_early)
{
  auto v = std::vector<std::vector<int>> { { 1, 2 }, { 3 }, { 4, 5, 6 } };

  auto res = into(
    std::vector<int> {},
    comp(cat, map([] (int x) {
          EXPECT_LT(x, 5);
          return x;
        }), take(4)),
    v);
  EXPECT_EQ(res, (std::vector<int> { 1, 2, 3, 4 }));
}

TEST(take, take_stops_early_enough)
{
  auto v = std::vector<int> { 1, 2, 3, 4, 5, 6 };

  auto res = into(
    std::vector<int> {},
    comp(
      map([](int x) {
          if (x > 4)
            throw std::runtime_error("bad!");
          return x;
        }),
      take(3)),
    v);
  EXPECT_EQ(res, (std::vector<int> { 1, 2, 3 }));
}

TEST(take, take_stops_early_enough2)
{
  auto v = std::vector<int> { 1, 2, 3, 4, 5, 6 };

  auto res = into(
    std::vector<int> {},
    comp(
      take(3),
      map([](int x) {
          if (x > 4)
            throw std::runtime_error("bad!");
          return x;
        })),
    v);
  EXPECT_EQ(res, (std::vector<int> { 1, 2, 3 }));
}

TEST(take, take_moves_the_state_through)
{
  auto v = std::vector<int> { 1, 2, 3, 4, 5 };
  auto spy = reduce(take(5)(first_rf), testing::copy_spy<>{}, v);
  EXPECT_EQ(spy.copied.count(), 0);
}

namespace impure {

TEST(take_impure, take)
{
  auto v = std::vector<int> { 1, 2, 3, 4, 5 };

  auto res = impure::into(std::vector<int> {}, impure::take(3), v);
  EXPECT_EQ(res, (std::vector<int> { 1, 2, 3 }));
}

TEST(take_impure, take_stops_early_enough2)
{
  auto v = std::vector<int> { 1, 2, 3, 4, 5, 6 };

  auto res = impure::into(
    std::vector<int> {},
    comp(
      impure::take(3),
      map([](int x) {
          if (x > 4)
            throw std::runtime_error("bad!");
          return x;
        })),
    v);
  EXPECT_EQ(res, (std::vector<int> { 1, 2, 3 }));
}

} // namespace impure

} // namespace xform
} // namespace atria
