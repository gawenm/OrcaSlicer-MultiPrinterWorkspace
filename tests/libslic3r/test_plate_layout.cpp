#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "libslic3r/Geometry/PlateLayout.hpp"

using namespace Slic3r;
using Catch::Matchers::WithinAbs;

TEST_CASE("Printer rows follow preset identity and retain plate order", "[PlateLayout]")
{
    const std::vector<PlateLayoutInput> plates{
        {"Belt custom", Vec2d(0, 0), Vec2d(220, 900)},
        {"P1S fine", Vec2d(0, 0), Vec2d(256, 256)},
        {"Belt custom", Vec2d(0, 0), Vec2d(220, 900)},
        {"P1S fine", Vec2d(0, 0), Vec2d(256, 256)},
        {"P1S fast", Vec2d(0, 0), Vec2d(256, 256)},
        {"A1 mini", Vec2d(0, 0), Vec2d(180, 180)}
    };
    const double horizontal_gap = 0.2;
    const double row_gap = 24.;
    const auto offsets = layout_printer_plates(plates, horizontal_gap, row_gap);

    REQUIRE(offsets.size() == plates.size());
    CHECK_THAT(offsets[0].y(), WithinAbs(offsets[2].y(), 1e-6));
    CHECK_THAT(offsets[1].y(), WithinAbs(offsets[3].y(), 1e-6));
    CHECK_THAT(offsets[2].x(), WithinAbs(plates[0].size.x() * (1. + horizontal_gap), 1e-6));
    CHECK_THAT(offsets[3].x(), WithinAbs(plates[1].size.x() * (1. + horizontal_gap), 1e-6));
    CHECK_THAT(offsets[1].y() + plates[1].size.y(), WithinAbs(-row_gap, 1e-6));
    CHECK_THAT(offsets[4].y() + plates[4].size.y(), WithinAbs(offsets[1].y() - row_gap, 1e-6));
    CHECK_THAT(offsets[5].y() + plates[5].size.y(), WithinAbs(offsets[4].y() - row_gap, 1e-6));
}

TEST_CASE("Printer rows reserve room for both labels and bed extensions", "[PlateLayout]")
{
    const double labels = 30.;
    const double extension = 12.;
    const std::vector<PlateLayoutInput> plates{
        {"First", Vec2d(-100, -50), Vec2d(200, 100), labels, extension},
        {"Second", Vec2d(15, 20), Vec2d(80, 60), labels}
    };
    const auto offsets = layout_printer_plates(plates, 0.2, 24.);
    CHECK_THAT(offsets[0].x() + plates[0].minimum.x(), WithinAbs(0., 1e-6));
    const double first_bottom = offsets[0].y() + plates[0].minimum.y();
    const double second_top = offsets[1].y() + plates[1].minimum.y() + plates[1].size.y();
    CHECK(first_bottom - second_top >= extension + labels);
}

TEST_CASE("An empty plate layout produces no offsets", "[PlateLayout]")
{
    CHECK(layout_printer_plates({}, 0.2, 24.).empty());
}

TEST_CASE("A new plate on the selected printer row keeps that row's position", "[PlateLayout]")
{
    const std::vector<PlateLayoutInput> plates{
        {"P1S", Vec2d(0, 0), Vec2d(256, 256)},
        {"CR-30", Vec2d(0, 0), Vec2d(220, 800)},
        {"P1S", Vec2d(0, 0), Vec2d(256, 256)},
        {"CR-30", Vec2d(0, 0), Vec2d(220, 800)}
    };
    const auto offsets = layout_printer_plates(plates, 0.2, 24.);
    REQUIRE(offsets.size() == 4);
    CHECK_THAT(offsets[3].y(), WithinAbs(offsets[1].y(), 1e-6));
    CHECK_THAT(offsets[3].x(), WithinAbs(220. * 1.2, 1e-6));
    CHECK(offsets[3].y() != offsets[0].y());
}
