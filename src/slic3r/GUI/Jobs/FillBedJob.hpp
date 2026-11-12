#ifndef FILLBEDJOB_HPP
#define FILLBEDJOB_HPP

#include "ArrangeJob.hpp"
#include "slic3r/GUI/Jobs/Job.hpp"
#include "libslic3r/Arrange.hpp"
#include "libslic3r/Point.hpp"
#include <exception>

namespace Slic3r { namespace GUI {

class Plater;

class FillBedJob : public Job
{
    int     m_object_idx = -1;

    using ArrangePolygon  = arrangement::ArrangePolygon;
    using ArrangePolygons = arrangement::ArrangePolygons;

    ArrangePolygons m_selected;
    ArrangePolygons m_unselected;
    //BBS: add partplate related logic
    ArrangePolygons m_locked;;

    Points m_bedpts;

    arrangement::ArrangeParams params;

    int m_status_range = 0;
    Plater *m_plater;

    bool m_instances;
    bool m_grouped_layout = false;
    int m_plate_index = -1;
    size_t m_template_instance = 0;
    DynamicPrintConfig m_plate_config;

public:

    void prepare();
    void process(Ctl &ctl) override;

    FillBedJob(bool instances = false);

    int status_range() const
    {
        return m_status_range;
    }

    void finalize(bool canceled, std::exception_ptr &e) override;
};

}} // namespace Slic3r::GUI

#endif // FILLBEDJOB_HPP
