#pragma once

#include "libslic3r/Point.hpp"

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace Slic3r {

struct PlateLayoutInput
{
    std::string printer_preset;
    Vec2d minimum;
    Vec2d size;
    double label_height = 0.;
    double front_extension = 0.;
};

// Rows follow the first occurrence of a printer preset. Settings within a plate do not affect its row.
inline std::vector<Vec2d> layout_printer_plates(const std::vector<PlateLayoutInput>& plates,
                                               double horizontal_gap_ratio, double minimum_row_gap)
{
    struct Row {
        std::vector<size_t> plates;
        double depth = 0.;
        double label_height = 0.;
        double front_extension = 0.;
    };

    std::map<std::string, size_t> row_by_preset;
    std::vector<Row> rows;
    for (size_t index = 0; index < plates.size(); ++index) {
        const PlateLayoutInput& plate = plates[index];
        const auto [position, inserted] = row_by_preset.try_emplace(plate.printer_preset, rows.size());
        if (inserted)
            rows.emplace_back();
        Row& row = rows[position->second];
        row.plates.push_back(index);
        row.depth = std::max(row.depth, plate.size.y());
        row.label_height = std::max(row.label_height, plate.label_height);
        row.front_extension = std::max(row.front_extension, plate.front_extension);
    }

    std::vector<Vec2d> offsets(plates.size());
    if (plates.empty())
        return offsets;

    double top = plates.front().minimum.y() + plates.front().size.y();
    double previous_extension = 0.;
    for (size_t row_index = 0; row_index < rows.size(); ++row_index) {
        const Row& row = rows[row_index];
        if (row_index > 0)
            top -= std::max(minimum_row_gap, previous_extension + row.label_height + 4.);

        double left = 0.;
        for (size_t index : row.plates) {
            const PlateLayoutInput& plate = plates[index];
            offsets[index] = Vec2d(left - plate.minimum.x(), top - plate.minimum.y() - plate.size.y());
            left += plate.size.x() * (1. + horizontal_gap_ratio);
        }
        top -= row.depth;
        previous_extension = row.front_extension;
    }
    return offsets;
}

} // namespace Slic3r
