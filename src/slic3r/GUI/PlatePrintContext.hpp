#ifndef slic3r_GUI_PlatePrintContext_hpp_
#define slic3r_GUI_PlatePrintContext_hpp_

#include "PartPlate.hpp"
#include "Plater.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "slic3r/Utils/NetworkAgentFactory.hpp"

namespace Slic3r::GUI {

inline PartPlate* selected_print_plate(Plater* plater)
{
    return plater ? plater->get_partplate_list().get_selected_plate() : nullptr;
}

inline std::string plate_printer_agent_id(const PartPlate* plate, PresetBundle& bundle)
{
    const bool assigned = plate && plate->printer_profile().assigned();
    const auto& config = assigned ? plate->printer_profile().config : bundle.printers.get_edited_preset().config;
    const auto* agent = config.option<ConfigOptionString>("printer_agent");
    if (agent && !agent->value.empty())
        return agent->value;
    const bool is_bambu = assigned ? plate->printer_profile().is_bambu : bundle.is_bbl_vendor();
    return is_bambu ? BBL_PRINTER_AGENT_ID : ORCA_PRINTER_AGENT_ID;
}

inline std::string plate_printer_model_id(const DynamicPrintConfig& config, const PresetBundle& bundle)
{
    const auto* model = config.option<ConfigOptionString>("printer_model");
    if (model != nullptr)
        for (const auto& [name, vendor] : bundle.vendors)
            for (const auto& printer : vendor.models)
                if (printer.name == model->value)
                    return printer.model_id;
    return {};
}

inline DynamicPrintConfig* plate_print_host_config(const PartPlate* plate, PresetBundle& bundle)
{
    auto& edited = bundle.printers.get_edited_preset();
    if (plate == nullptr || !plate->printer_profile().assigned())
        return &edited.config;

    // Project files carry slicing settings. Use the matching installed preset for the connection.
    const auto& profile = plate->printer_profile();
    const auto& name = profile.config.opt_string("printer_settings_id");
    Preset* local = edited.name == name ? &edited : bundle.printers.find_preset(name);
    if (local == nullptr || local->name != name)
        return nullptr;

    for (const char* key : {"printer_model", "printer_variant", "gcode_flavor", "nozzle_diameter",
                            "printable_area", "printable_height", "belt_printer", "belt_printer_infinite_y",
                            "belt_rotation_axis", "belt_angle"}) {
        const ConfigOption* saved = profile.config.option(key);
        const ConfigOption* installed = local->config.option(key);
        if ((saved == nullptr) != (installed == nullptr) ||
            (saved && (saved->type() != installed->type() || !(*saved == *installed))))
            return nullptr;
    }
    return &local->config;
}

} // namespace Slic3r::GUI

#endif
