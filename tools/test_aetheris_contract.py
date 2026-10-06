from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
entry = (ROOT / "project" / "entry.cpp").read_text("utf-8", errors="replace")
project = (ROOT / "aetheris.vcxproj").read_text("utf-8", errors="replace")

sources = []
for p in (ROOT / "project").rglob("*"):
    if p.suffix.lower() in (".cpp", ".hpp", ".h", ".inl"):
        if "external" in p.parts:
            continue
        try:
            sources.append(p.read_text("utf-8", errors="replace"))
        except OSError:
            pass
source = "\n".join(sources)

menu_core = (ROOT / "project" / "core" / "rendering" / "impl" / "menu" / "menu.core.cpp").read_text(
    "utf-8", errors="replace"
)
rendering_hpp = (ROOT / "project" / "core" / "rendering" / "rendering.hpp").read_text(
    "utf-8", errors="replace"
)
menu_sources = menu_core + rendering_hpp

checks = {
    "no legitbot sources": all(
        not (ROOT / p).exists()
        for p in (
            "project/core/features/combat/impl/legit.cpp",
            "project/core/rendering/impl/menu/menu.legitbot.cpp",
            "project/core/systems/impl/legit_input.cpp",
        )
    ),
    "no legitbot symbols": all(
        marker not in source
        for marker in (
            "m_legitbot",
            "g_legit",
            "legit_input",
            "draw_legitbot",
            "legitbot - ",
            '"legitbot"',
            "tab_legit",
            "Legitbot",
        )
    ),
    "menu: no search system": all(
        marker not in menu_sources
        for marker in (
            "m_search_open",
            "m_search_query",
            "m_search_entries",
            "search_entry",
            "rebuild_search_index",
            "draw_search_results",
            "close_search",
            "activate_search_result",
        )
    ),
    "menu: no white theme": all(
        marker not in menu_sources
        for marker in (
            "menu_theme",
            "theme_switcher",
            "White",
            '"Dark"',
            "draw_rail",
            "draw_subtabs",
        )
    ),
    "menu: lua tab present": all(
        marker in menu_sources
        for marker in ("draw_lua", "lua, count", '"Scripts"')
    )
    and (ROOT / "project" / "core" / "rendering" / "impl" / "menu" / "menu.lua.cpp").exists()
    and (ROOT / "project" / "core" / "features" / "misc" / "impl" / "lua.cpp").exists()
    and (ROOT / "project" / "external" / "lua" / "lapi.c").exists()
    and (ROOT / "project" / "external" / "sol2" / "include" / "sol" / "sol.hpp").exists()
    and "g_lua" in source
    and "m_lua" in source,
    "menu: 7 tabs, rage first": ("entry tabs[ 7 ]" in rendering_hpp)
    and ("ragebot, player, world, skins, misc, config, lua, count" in rendering_hpp)
    and ("tabs[ 7 ]" in menu_core or "const char*, 7" in menu_core),
    "menu: no settings popover/gear": all(
        marker not in menu_sources
        for marker in (
            "m_menu_settings_open",
            "apply_theme_preset",
            "m_theme_preset",
            "theme_color",
            "menu_settings_modal",
            "menu_gear_btn",
        )
    ),
    "menu: no safe mode": "safe_mode" not in menu_sources and "Safe mode" not in menu_sources,
    "menu: toggle switches + slider knob": ("draw_minimal_switch" in (ROOT / "project" / "external" / "xdraw" / "xui" / "xui.cpp").read_text("utf-8", errors="replace")),
    "menu: draggable scrollbar": ("s_sb_drag_tab" in menu_core) and ("grab_rect" in menu_core),
    "menu: hit/miss log toggles": ('"Hit logs"' in (ROOT / "project" / "core" / "rendering" / "impl" / "menu" / "menu.misc.cpp").read_text("utf-8", errors="replace"))
    and ('"Miss logs"' in (ROOT / "project" / "core" / "rendering" / "impl" / "menu" / "menu.misc.cpp").read_text("utf-8", errors="replace")),
    "hit logs anchored top-left": ("margin + log_y_offset" in (ROOT / "project" / "core" / "features" / "misc" / "impl" / "impacts.cpp").read_text("utf-8", errors="replace")),
    "ragebot: no doubletap": all(
        marker not in source
        for marker in (
            "doubletap",
            "double_tap",
            "g_exploits",
            "process_doubletap",
            "post_fire",
            "pre_think",
        )
    ),
    "ragebot: no quick revolver": all(
        marker not in source
        for marker in ("revolver_quick", "quick_revolver", "note_pending_shot", "on_engine_fire")
    ),
    "antiaim: render-camera intent": ("fresh_command" in source)
    and ("intent_angles" in source)
    and ("unreproject_moves" in source),
    "antiaim: phantom upgrade": ("yaw_mode" in source)
    and ("spinbot" in source)
    and ("advance_spin" in source)
    and ("local_view" in source),
    "death effects: trimmed styles": ("play_death_effect_halo" not in source)
    and ("molotov_air" not in source)
    and ("play_death_effect_blood" not in source)
    and ("play_death_effect_killstars" in source),
    "antiaim: trimmed modes": ("yaw_mode::spam" not in source)
    and ('"Local view"' not in source),
    "movement: valve strafer": ("g_valve_strafer" in source)
    and ("valve_strafer" in source)
    and ("engine_yaw" in source)
    and (ROOT / "project" / "core" / "features" / "movement" / "impl" / "valve_strafer.cpp").exists()
    and ("valve_strafer.cpp" in project),
    "ragebot: single auto scope": ("m_autos.scope" not in source),
    "ragebot: phantom port": ("auto_stop_mode" in source)
    and ("debug_multipoints" in source)
    and ("run_double_tap" not in source)
    and ("should_stop_between_shots" not in source)
    and ("draw_doubletap_indicator" not in source),
    "menu: flask logo, no palm": ("flask_conical" in menu_sources)
    and ("brand_logo" not in menu_sources)
    and (ROOT / "project" / "core" / "resources" / "icons" / "flask.hpp").exists(),
    "menu: watermark + indicators": ("m_watermark" in source)
    and ("m_indicators" in source),
    "font: jetbrains mono everywhere": ("jetbrains_mono::regular" in (ROOT / "project" / "external" / "xdraw" / "xdraw.cpp").read_text("utf-8", errors="replace"))
    and ("jetbrains_mono::regular" in (ROOT / "project" / "core" / "rendering" / "impl" / "fonts.cpp").read_text("utf-8", errors="replace"))
    and ("jetbrains_mono::bold" in (ROOT / "project" / "core" / "rendering" / "impl" / "fonts.cpp").read_text("utf-8", errors="replace")),
    "menu: no breadcrumb bar": ("tab_names" not in menu_sources),
    "accent is red, no green": ("229, 57, 53" in source)
    and all(marker not in source for marker in ("A8E51D", "168, 229, 29", '"lime"', "lime theme")),
    "no cylix branding": all(
        marker not in menu_sources
        for marker in ("cylix", '"cylix"', ".bond", '"legit"')
    ),
    "aetheris branding present": all(
        marker in source
        for marker in ("AETHERIS", "aetheris", "aetheris_dummy_window")
    ),
    "no nexoria branding": all(
        marker.lower() not in source.lower()
        for marker in ("nexoria", "nexoriacs2", "nexoriabeta", "nexoriamods.com", "nexoria.cx")
    ),
    "config dir rebranded": 'L"C:\\\\aetheris"' in (ROOT / "project" / "external" / "config.hpp").read_text(
        "utf-8", errors="replace"
    ),
    "project renamed": "aetheris.vcxproj" in (ROOT / "aetheris.sln").read_text(
        "utf-8", errors="replace"
    ),
    "RootNamespace rebranded": "<RootNamespace>aetheris</RootNamespace>" in project,
    "Export macro rebranded": "AETHERIS_EXPORTS" in project and "NEXORIACS2_EXPORTS" not in project,
    "Target names rebranded": "<TargetName>aetherisbeta</TargetName>" in project
    and "<TargetName>aetheris</TargetName>" in project,
    "Vcpkg manifest rebranded": '"aetheris"' in (ROOT / "vcpkg.json").read_text("utf-8", errors="replace"),
    "lua wired in build": "menu.lua.cpp" in project
    and "misc\\impl\\lua.cpp" in project
    and "external\\lua\\lapi.c" in project
    and "external/lua" in project,
    "Ship buffer security": "<BufferSecurityCheck>true</BufferSecurityCheck>" in project,
    "Ship CFG": "<ControlFlowGuard>Guard</ControlFlowGuard>" in project,
    "Ship warnings": "<WarningLevel>Level4</WarningLevel>" in project,
    "Ship exceptions": "<ExceptionHandling>Sync</ExceptionHandling>" in project,
    "WinHTTP dependency": "winhttp.lib" in project.lower(),
}

failed = [name for name, ok in checks.items() if not ok]
for name, ok in checks.items():
    print(("PASS" if ok else "FAIL"), name)

if failed:
    print(f"contract failures={len(failed)}")
    sys.exit(1)
print("payload contract: PASS")
