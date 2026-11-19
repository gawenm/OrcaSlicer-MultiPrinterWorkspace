#include "BeltGCode.hpp"
#include "GCodeWriter.hpp"
#include "GCode/BeltKinematics.hpp"
#include "BeltTransform.hpp"
#include "Print.hpp"
#include "Point.hpp"
#include "PrintConfig.hpp"
#include "libslic3r.h"
#include <cstdlib>

namespace Slic3r {

void BeltGCode::init_belt_writer(Print &print)
{
    // Axis remap and build volume max are set by base GCode after init_belt_writer
    // returns; set_kinematics() replays them, so install order does not matter.
    install_belt_kinematics(m_writer, print.config());
    m_writer.set_force_normal_lift(true);
}

void BeltGCode::write_belt_header(GCodeOutputStream &file, const Print &print)
{
    const auto &full_cfg = print.full_print_config();
    // Slicing rotation: the belt tilt (axis + angle) and the single source of truth
    // for the physical tilt the G-code viewer uses to enable belt view.
    file.write_format("; belt_slice_rotation = %s\n", full_cfg.opt_serialize("belt_slice_rotation").c_str());
    file.write_format("; belt_slice_rotation_angle = %.1f\n", print.config().belt_slice_rotation_angle.value);
    // Machine-frame transform: shear (cot) + scale (1/|sin|) derived from the belt
    // tilt angle (or belt_frame_tilt_angle when decoupled).
    file.write_format("; belt_frame_tilt_decouple = %d\n", print.config().belt_frame_tilt_decouple.value ? 1 : 0);
    file.write_format("; belt_frame_tilt_angle = %.1f\n", print.config().belt_frame_tilt_angle.value);
}

void BeltGCode::on_set_origin(const PrintObject *obj, const Point & /*inst_shift*/)
{
    // Matches the per-instance Z-offset added in PrintObjectSlice.cpp: transform
    // the origin through the belt pipeline so that back_transform(T * origin) =
    // origin (correct machine position). The back_transform applied during
    // G-code emission is the inverse of the forward transform.

    // GCode stores origins in workspace XY, and the writer subtracts the
    // unrotated plate origin before applying belt kinematics. Rotate only the
    // plate-local part; restore the workspace offset for the writer to remove.
    // This also matches the plate-local Z compensation in PrintObjectSlice.
    const Transform3d T = BeltTransformPipeline::build_forward_transform(m_config);
    const Vec2d plate_origin = obj->print()->get_plate_origin().head<2>();
    const Vec2d local_origin = this->origin() - plate_origin;
    const Vec3d adjusted = T.linear() * Vec3d(local_origin.x(), local_origin.y(), 0.);
    this->set_origin(Vec2d(adjusted.x(), adjusted.y()) + plate_origin);
}

} // namespace Slic3r
