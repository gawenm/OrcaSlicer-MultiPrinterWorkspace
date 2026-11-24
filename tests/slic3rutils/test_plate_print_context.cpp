#include <catch2/catch_test_macros.hpp>

#include "libslic3r/Config.hpp"
#include "libslic3r/Format/bbs_3mf.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "libslic3r/PrintConfig.hpp"
#include "slic3r/GUI/PartPlate.hpp"
#include "slic3r/GUI/DeviceManager.hpp"
#include "slic3r/GUI/PlatePrintContext.hpp"

using namespace Slic3r;
using namespace Slic3r::GUI;

TEST_CASE("Inherited plates retain their project printer when a mixed workspace is created", "[PlatePrinterProfile]")
{
    DynamicPrintConfig config;
    config.apply(FullPrintConfig::defaults());
    config.opt_string("printer_settings_id", true) = "Project P1S";
    config.opt_string("printer_model", true) = "Bambu Lab P1S";
    config.opt_string("printhost_apikey", true) = "local-secret";
    config.opt_string("machine_start_gcode", true) = "P1S_START";
    config.set_key_value("printer_technology", new ConfigOptionEnum<PrinterTechnology>(ptFFF));
    config.set_key_value("printable_height", new ConfigOptionFloat(250.));
    config.set_key_value("printable_area", new ConfigOptionPoints({{0, 0}, {256, 0}, {256, 256}, {0, 256}}));
    config.set_key_value("nozzle_diameter", new ConfigOptionFloats({0.4}));
    config.set_key_value("filament_type", new ConfigOptionStrings({"ABS"}));
    PlatePrinterProfile project{config, true};
    REQUIRE(project.valid());

    PartPlate first;
    PartPlate second;
    PartPlate belt;
    first.set_index(0);
    second.set_index(1);
    belt.set_index(2);
    auto belt_config = config;
    belt_config.opt_string("printer_settings_id", true) = "CR-30";
    belt.set_printer_profile({belt_config, false});

    REQUIRE(capture_inherited_plate_profiles({&first, &second, &belt}, project));
    project.config.opt_string("printer_settings_id", true) = "Changed project printer";
    CHECK(first.printer_profile().config.opt_string("printer_settings_id") == "Project P1S");
    CHECK(second.printer_profile().config.opt_string("printer_settings_id") == "Project P1S");
    CHECK(belt.printer_profile().config.opt_string("printer_settings_id") == "CR-30");
    CHECK(first.printer_profile().is_bambu);
    CHECK_FALSE(first.printer_profile().config.has("printhost_apikey"));
    PresetBundle bundle;
    CHECK(first.build_print_config(bundle).opt_string("printer_model") == "Bambu Lab P1S");
    CHECK(first.build_print_config(bundle).opt_string("machine_start_gcode") == "P1S_START");

    PartPlate uncaptured;
    uncaptured.set_index(3);
    CHECK_FALSE(capture_inherited_plate_profiles({&uncaptured}, {}));
    CHECK_FALSE(uncaptured.printer_profile().assigned());
}

TEST_CASE("Assigned plates use the installed printer preset for networking", "[printer_agent][plate]")
{
    PresetBundle bundle;
    auto& bambu_model = bundle.vendors["BBL"].models.emplace_back();
    bambu_model.name = "Test Bambu";
    bambu_model.model_id = "C11";

    const auto install = [&](const std::string& name, const std::string& model, const std::string& host) {
        auto config = bundle.printers.default_preset().config;
        config.opt_string("printer_model", true) = model;
        config.opt_string("printer_settings_id", true) = name;
        config.opt_string("print_host", true) = host;
        bundle.printers.load_preset("", name, std::move(config), false);
        const auto* preset = bundle.printers.find_preset(name, false);
        REQUIRE(preset != nullptr);
        PlatePrinterProfile profile{preset->config, model == "Test Bambu"};
        profile.remove_local_settings();
        return profile;
    };

    auto bambu = install("Test P1S", "Test Bambu", "");
    auto belt = install("Test Belt", "Test Klipper", "https://belt.example.test");
    auto ender = install("Test Ender", "Test Klipper", "ender.example.test");
    bundle.printers.select_preset_by_name("Test P1S", true);

    auto* belt_config = plate_print_host_config(&belt, bundle);
    REQUIRE(belt_config != nullptr);
    CHECK(plate_print_host_device_id(*belt_config) == "belt.example.test");
    CHECK(plate_printer_agent_id(&bambu, bundle) == BBL_PRINTER_AGENT_ID);
    CHECK(plate_printer_agent_id(&belt, bundle) == ORCA_PRINTER_AGENT_ID);

    belt_config->opt_string("printer_agent", true) = MOONRAKER_PRINTER_AGENT_ID;
    belt_config->opt_string("printhost_port", true) = "7125";
    CHECK(plate_printer_agent_id(&belt, bundle) == MOONRAKER_PRINTER_AGENT_ID);
    CHECK(plate_print_host_device_id(*belt_config) == "belt.example.test:7125");

    auto* ender_config = plate_print_host_config(&ender, bundle);
    REQUIRE(ender_config != nullptr);
    ender_config->opt_string("printer_agent", true) = MOONRAKER_PRINTER_AGENT_ID;
    CHECK(plate_printer_agent_id(&ender, bundle) == MOONRAKER_PRINTER_AGENT_ID);
    CHECK(plate_print_host_device_id(*ender_config) == "ender.example.test");

    bundle.printers.select_preset_by_name("Test Ender", true);
    auto& edited = bundle.printers.get_edited_preset().config;
    edited.opt_string("print_host", true) = "new-ender.example.test";
    CHECK(plate_printer_agent_id(&bambu, bundle) == BBL_PRINTER_AGENT_ID);
    CHECK(plate_printer_model_id(bambu.config, bundle) == "C11");
    MachineObject selected(nullptr, nullptr, "Test", "test-device", "192.0.2.1");
    selected.printer_type = "C11";
    CHECK(plate_device_compatibility(bambu, bundle, selected) == PlateDeviceCompatibility::Compatible);
    selected.printer_type = "test-belt-model";
    CHECK(plate_device_compatibility(bambu, bundle, selected) == PlateDeviceCompatibility::Incompatible);
    selected.printer_type.clear();
    CHECK(plate_device_compatibility(bambu, bundle, selected) == PlateDeviceCompatibility::Unknown);
    CHECK(plate_print_host_device_id(*plate_print_host_config(&ender, bundle)) == "new-ender.example.test");
    CHECK(plate_print_host_device_id(*plate_print_host_config(&belt, bundle)) == "belt.example.test:7125");

    edited.opt_string("printer_model", true) = "Test Bambu";
    REQUIRE(plate_print_host_config(&ender, bundle) != nullptr);
    CHECK(plate_print_host_device_id(*plate_print_host_config(&ender, bundle)) == "ender.example.test");
    CHECK(plate_printer_agent_id(&ender, bundle) == MOONRAKER_PRINTER_AGENT_ID);
    CHECK(plate_print_host_device_id(*plate_print_host_config(&belt, bundle)) == "belt.example.test:7125");
    edited.opt_string("printer_model", true) = "Test Klipper";
    CHECK(plate_print_host_device_id(*plate_print_host_config(&ender, bundle)) == "new-ender.example.test");

    auto* bambu_host_config = plate_print_host_config(&bambu, bundle);
    REQUIRE(bambu_host_config != nullptr);
    bambu_host_config->set_key_value("bbl_use_printhost", new ConfigOptionBool(true));
    CHECK(plate_printer_agent_id(&bambu, bundle) == ORCA_PRINTER_AGENT_ID);

    ender.config.opt_string("printer_variant", true) = "mismatched variant";
    CHECK(plate_print_host_config(&ender, bundle) == nullptr);
    CHECK(plate_printer_agent_id(&ender, bundle).empty());
    belt.config.opt_string("printer_settings_id", true) = "Unknown preset";
    CHECK(plate_print_host_config(&belt, bundle) == nullptr);
}
