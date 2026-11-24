#ifndef slic3r_GUI_PlatePrintContext_hpp_
#define slic3r_GUI_PlatePrintContext_hpp_

#include <wx/wxprec.h>

#include "DeviceManager.hpp"
#include "PartPlate.hpp"
#include "Plater.hpp"
#include "libslic3r/Config.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "slic3r/Utils/NetworkAgentFactory.hpp"

#include <utility>
#include <vector>

namespace Slic3r::GUI {

inline PartPlate* selected_print_plate(Plater* plater)
{
    return plater ? plater->get_partplate_list().get_selected_plate() : nullptr;
}

// Preserve the project printer when a workspace starts using plate printers.
// Prepare all copies before changing any plate so allocation errors cannot
// leave half of a row following a different printer.
inline bool capture_inherited_plate_profiles(const std::vector<PartPlate*>& plates,
                                             PlatePrinterProfile project_profile)
{
    if (!project_profile.valid())
        return false;
    project_profile.remove_local_settings();

    std::vector<std::pair<PartPlate*, PlatePrinterProfile>> captures;
    for (PartPlate* plate : plates)
        if (plate && !plate->printer_profile().assigned())
            captures.emplace_back(plate, project_profile);
    for (auto& [plate, profile] : captures)
        plate->set_printer_profile(std::move(profile), false);
    return true;
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

inline DynamicPrintConfig* plate_print_host_config(const PlatePrinterProfile* profile, PresetBundle& bundle)
{
    auto& edited = bundle.printers.get_edited_preset();
    if (profile == nullptr || !profile->assigned())
        return &edited.config;

    // Project files carry slicing settings. Use the matching installed preset for the connection.
    const auto* name = profile->config.option<ConfigOptionString>("printer_settings_id");
    if (name == nullptr || name->value.empty())
        return nullptr;
    Preset* local = bundle.printers.find_preset(name->value, false);
    if (local == nullptr || local->name != name->value || local->is_external || local->is_project_embedded)
        return nullptr;

    const auto matches_plate = [&](const DynamicPrintConfig& candidate) {
        for (const char* key : {"printer_model", "printer_variant", "gcode_flavor", "nozzle_diameter",
                                "printable_area", "printable_height", "belt_printer", "belt_printer_infinite_y",
                                "belt_rotation_axis", "belt_angle"}) {
            const ConfigOption* saved = profile->config.option(key);
            const ConfigOption* option = candidate.option(key);
            if ((saved == nullptr) != (option == nullptr) ||
                (saved && (saved->type() != option->type() || !(*saved == *option))))
                return false;
        }
        return true;
    };

    // A selected preset can have unsaved connection edits, but it may also
    // have unsaved changes to the hardware. Resolve the connection only from a
    // configuration that still matches the plate's captured hardware.
    if (bundle.printers.get_selected_preset().name == local->name && matches_plate(edited.config))
        return &edited.config;
    return matches_plate(local->config) ? &local->config : nullptr;
}

inline DynamicPrintConfig* plate_print_host_config(const PartPlate* plate, PresetBundle& bundle)
{
    return plate_print_host_config(plate ? &plate->printer_profile() : nullptr, bundle);
}

inline std::string plate_printer_agent_id(const PlatePrinterProfile* profile, PresetBundle& bundle)
{
    const auto* config = plate_print_host_config(profile, bundle);
    if (config == nullptr)
        return {};

    const auto* agent = config->option<ConfigOptionString>("printer_agent");
    const auto* third_party_host = config->option<ConfigOptionBool>("bbl_use_printhost");
    const bool is_bambu = bundle.get_vendor_type(*config) == VendorType::Marlin_BBL;
    const bool use_third_party_host = profile && profile->assigned() && is_bambu && third_party_host && third_party_host->value;
    if (agent && !agent->value.empty() && !(use_third_party_host && agent->value == BBL_PRINTER_AGENT_ID))
        return agent->value;
    return is_bambu && !use_third_party_host ? BBL_PRINTER_AGENT_ID : ORCA_PRINTER_AGENT_ID;
}

inline std::string plate_printer_agent_id(const PartPlate* plate, PresetBundle& bundle)
{
    return plate_printer_agent_id(plate ? &plate->printer_profile() : nullptr, bundle);
}

inline std::string plate_print_host_device_id(const DynamicPrintConfig& config)
{
    const auto& host = config.opt_string("print_host");
    return host.empty() ? std::string{} : MachineObject::dev_id_from_address(host, config.opt_string("printhost_port"));
}

} // namespace Slic3r::GUI

#endif
