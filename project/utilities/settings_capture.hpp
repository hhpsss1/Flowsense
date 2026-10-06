#pragma once

#include "crash_report.hpp"


namespace settings_capture {

using namespace crash_report;

inline settings_snapshot capture_all_settings() {
    settings_snapshot snapshot;


    snapshot.add("Ragebot", "Enabled", "true", true);
    snapshot.add("Ragebot", "Silent Aim", "true");
    snapshot.add("Ragebot", "No Spread", "false");
    snapshot.add("Ragebot", "Double Tap", "false", true);
    snapshot.add("Ragebot", "Resolver", "true", true);
    snapshot.add("Ragebot", "Rapid Fire", "false", true);

    snapshot.add("Visuals", "ESP Enabled", "true");
    snapshot.add("Visuals", "Box ESP", "true");
    snapshot.add("Visuals", "Name ESP", "true");
    snapshot.add("Visuals", "Health Bar", "true");
    snapshot.add("Visuals", "Weapon ESP", "true");
    snapshot.add("Visuals", "Glow", "false");
    snapshot.add("Visuals", "Chams", "false", true);
    snapshot.add("Visuals", "Skeleton", "false");
    snapshot.add("Visuals", "Grenade ESP", "false");
    snapshot.add("Visuals", "Bomb Timer", "true");
    snapshot.add("Visuals", "Money", "true");
    snapshot.add("Visuals", "Radar", "false");
    snapshot.add("Visuals", "Night Mode", "false");
    snapshot.add("Visuals", "World Modulation", "false");
    snapshot.add("Visuals", "No Flash", "false", true);
    snapshot.add("Visuals", "No Smoke", "false", true);
    snapshot.add("Visuals", "No Scope", "false", true);
    snapshot.add("Visuals", "Viewmodel FOV", "false");

    snapshot.add("Skins", "Knife Enabled", "false", true);
    snapshot.add("Skins", "Glove Enabled", "false");
    snapshot.add("Skins", "Weapon Skins", "false");

    snapshot.add("Misc", "Bhop", "true");
    snapshot.add("Misc", "AutoStrafe", "false");
    snapshot.add("Misc", "Edge Jump", "false");
    snapshot.add("Misc", "Fast Duck", "false");
    snapshot.add("Misc", "Jump Throw", "false");
    snapshot.add("Misc", "Clan Tag", "false");
    snapshot.add("Misc", "Kill Say", "false");
    snapshot.add("Misc", "Chat Spam", "false");
    snapshot.add("Misc", "Auto Defuse", "false", true);
    snapshot.add("Misc", "Purchase List", "false");
    snapshot.add("Misc", "Grenade Prediction", "false");

    snapshot.add("System", "Config Name", "default");
    snapshot.add("System", "Build Version", "1.0.0");

    return snapshot;
}

struct settings_registrar {
    settings_registrar() {
        crash_report::register_settings_getter(capture_all_settings);
    }
};

static settings_registrar g_settings_registrar;

}
