#include <catch2/catch_test_macros.hpp>

#include "slic3r/GUI/PlatePrintContext.hpp"

using namespace Slic3r;
using namespace Slic3r::GUI;

TEST_CASE("Assigned plates use the installed printer preset for networking", "[printer_agent][plate]")
{
    PresetBundle bundle;
    bundle.vendors["BBL"].models.emplace_back().name = "Test Bambu";

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
    CHECK(plate_print_host_device_id(*plate_print_host_config(&ender, bundle)) == "new-ender.example.test");
    CHECK(plate_print_host_device_id(*plate_print_host_config(&belt, bundle)) == "belt.example.test:7125");

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
