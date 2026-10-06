cmake_minimum_required(VERSION 3.29)
cmake_policy(SET CMP0091 NEW)
project(aetheris_portable LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

set(AETHERIS_SOURCES
    "project/core/features/changer/impl/agents.cpp"
    "project/core/features/changer/impl/musickits.cpp"
    "project/core/features/changer/impl/guns.cpp"
    "project/core/features/changer/impl/gloves.cpp"
    "project/core/features/changer/impl/stickers.cpp"
    "project/core/features/changer/impl/econ_item_system.cpp"
    "project/core/features/changer/impl/knives.cpp"
    "project/core/features/combat/impl/extrapolation.cpp"
    "project/core/features/combat/impl/misc.cpp"
    "project/core/features/combat/impl/rage.cpp"
    "project/core/features/combat/impl/shared.cpp"
    "project/core/features/combat/impl/vac.cpp"
    "project/core/features/esp/item/item.chams.cpp"
    "project/core/features/esp/item/item.glow.cpp"
    "project/core/features/esp/item/item.overlay.cpp"
    "project/core/features/esp/other/other.overlay.cpp"
    "project/core/features/esp/player/player.chams.cpp"
    "project/core/features/esp/player/player.glow.cpp"
    "project/core/features/esp/player/player.overlay.cpp"
    "project/core/features/esp/projectile/projectile.tracers.cpp"
    "project/core/features/esp/projectile/projectile.overlay.cpp"
    "project/core/features/misc/impl/camera.cpp"
    "project/core/features/misc/impl/chat_logs.cpp"
    "project/core/features/misc/impl/dlight.cpp"
    "project/core/features/misc/impl/hud.cpp"
    "project/core/features/misc/impl/impacts.cpp"
    "project/core/features/misc/impl/lua.cpp"
    "project/core/features/misc/impl/other.cpp"
    "project/core/features/misc/impl/projectile.trajectory.cpp"
    "project/core/features/misc/impl/removals.cpp"
    "project/core/features/misc/impl/scoreboard_weapons.cpp"
    "project/core/features/movement/impl/airstrafe.cpp"
    "project/core/features/movement/impl/test_strafer.cpp"
    "project/core/features/movement/impl/bunnyhop.cpp"
    "project/core/features/movement/impl/edgebug.cpp"
    "project/core/features/movement/impl/edgejump.cpp"
    "project/core/features/movement/impl/edgestop.cpp"
    "project/core/features/movement/impl/fastladder.cpp"
    "project/core/features/movement/impl/jumpbug.cpp"
    "project/core/features/movement/impl/slowwalk.cpp"
    "project/core/features/world/impl/scene.cpp"
    "project/core/features/world/impl/smoke.cpp"
    "project/core/features/world/impl/weather.cpp"
    "project/core/hooks/impl/cheat.cpp"
    "project/core/hooks/impl/skins.cpp"
    "project/core/hooks/impl/utility.cpp"
    "project/core/rendering/impl/context.cpp"
    "project/core/rendering/impl/fonts.cpp"
    "project/core/rendering/impl/menu/menu.config.cpp"
    "project/core/rendering/impl/menu/menu.core.cpp"
    "project/core/rendering/impl/menu/menu.lua.cpp"
    "project/core/rendering/impl/menu/CModelChanger.cpp"
    "project/core/rendering/impl/menu/menu.misc.cpp"
    "project/core/rendering/impl/menu/menu.player.cpp"
    "project/core/rendering/impl/menu/menu.ragebot.cpp"
    "project/core/rendering/impl/menu/menu.skins.cpp"
    "project/core/rendering/impl/menu/menu.world.cpp"
    "project/core/rendering/impl/widgets.cpp"
    "project/core/systems/impl/bones.cpp"
    "project/core/systems/impl/bounds.cpp"
    "project/core/systems/impl/entities.cpp"
    "project/core/systems/impl/events.cpp"
    "project/core/systems/impl/frame_data.cpp"
    "project/core/systems/impl/hitboxes.cpp"
    "project/core/systems/impl/icons.cpp"
    "project/core/systems/impl/input.cpp"
    "project/core/systems/impl/local.cpp"
    "project/core/systems/impl/materials.cpp"
    "project/core/systems/impl/model_preview.cpp"
    "project/core/systems/impl/prediction.cpp"
    "project/core/systems/impl/schemas.cpp"
    "project/core/systems/impl/tracing.cpp"
    "project/core/systems/impl/view.cpp"
    "project/entry.cpp"
    "project/external/lz4/lz4.c"
    "project/external/lua/lapi.c"
    "project/external/lua/lauxlib.c"
    "project/external/lua/lbaselib.c"
    "project/external/lua/lcode.c"
    "project/external/lua/lcorolib.c"
    "project/external/lua/lctype.c"
    "project/external/lua/ldblib.c"
    "project/external/lua/ldebug.c"
    "project/external/lua/ldo.c"
    "project/external/lua/ldump.c"
    "project/external/lua/lfunc.c"
    "project/external/lua/lgc.c"
    "project/external/lua/linit.c"
    "project/external/lua/liolib.c"
    "project/external/lua/llex.c"
    "project/external/lua/lmathlib.c"
    "project/external/lua/lmem.c"
    "project/external/lua/loadlib.c"
    "project/external/lua/lobject.c"
    "project/external/lua/lopcodes.c"
    "project/external/lua/loslib.c"
    "project/external/lua/lparser.c"
    "project/external/lua/lstate.c"
    "project/external/lua/lstring.c"
    "project/external/lua/lstrlib.c"
    "project/external/lua/ltable.c"
    "project/external/lua/ltablib.c"
    "project/external/lua/ltm.c"
    "project/external/lua/lundump.c"
    "project/external/lua/lutf8lib.c"
    "project/external/lua/lvm.c"
    "project/external/lua/lzio.c"
    "project/external/stackwalker/stackwalker.cpp"
    "project/external/xdraw/xdraw.cpp"
    "project/external/xdraw/xui/xui.cpp"
    "project/external/zydis/zydis.c"
    "project/pch/pch.cpp"
    "project/protection/patterns.cpp"
    "project/utilities/addresses/impl/functions.cpp"
    "project/utilities/addresses/impl/globals.cpp"
    "project/utilities/addresses/impl/interfaces/engine_cvar.cpp"
    "project/utilities/addresses/impl/modules.cpp"
    "project/utilities/hooking/impl/allocator.cpp"
    "project/utilities/hooking/impl/jmp.cpp"
    "project/utilities/hooking/impl/manager.cpp"
    "project/utilities/logging/impl/console.cpp"
    "project/utilities/logging/impl/popup.cpp"
    "project/utilities/math/math.cpp"
    "project/utilities/memory/memory.cpp"
    "project/utilities/overrides.cpp"
    "project/utilities/proto/proto.cpp"
    "project/utilities/random/random.cpp"
    "project/utilities/security/impl/integrity.cpp"
    "project/utilities/security/impl/prologues.cpp"
    "project/utilities/security/impl/regions.cpp"
    "project/utilities/steam/impl/friends.cpp"
    "project/utilities/steam/impl/http.cpp"
    "project/utilities/steam/impl/user.cpp"
    "project/utilities/steam/impl/utils.cpp"
    "project/utilities/threadpool/threadpool.cpp"
)

add_library(aetheris SHARED ${AETHERIS_SOURCES})
target_include_directories(aetheris PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/project"
    "${CMAKE_CURRENT_SOURCE_DIR}/project/external/phnt"
    "${CMAKE_CURRENT_SOURCE_DIR}/project/external/lua"
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows-static/include"
)
target_link_directories(aetheris PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows-static/lib"
)
target_compile_definitions(aetheris PRIVATE
    NDEBUG
    AETHERIS_SHIP
    AETHERIS_EXPORTS
    _WINDOWS
    UNICODE
    _UNICODE
    _USRDLL
)
target_compile_options(aetheris PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:/EHsc>
    /O2
    /GS
    /guard:cf
    /arch:AVX2
    /W1
)
target_link_libraries(aetheris PRIVATE
    freetype.lib
    Synchronization.lib
    winhttp.lib
)
target_link_options(aetheris PRIVATE
    /ENTRY:entry
    /SUBSYSTEM:WINDOWS
    /GUARD:CF
    /OPT:REF
    /OPT:ICF
    /MAP
)
target_precompile_headers(aetheris PRIVATE "project/pch/pch.hpp")
set_source_files_properties(
    "project/external/lz4/lz4.c"
    "project/external/lua/lapi.c"
    "project/external/lua/lauxlib.c"
    "project/external/lua/lbaselib.c"
    "project/external/lua/lcode.c"
    "project/external/lua/lcorolib.c"
    "project/external/lua/lctype.c"
    "project/external/lua/ldblib.c"
    "project/external/lua/ldebug.c"
    "project/external/lua/ldo.c"
    "project/external/lua/ldump.c"
    "project/external/lua/lfunc.c"
    "project/external/lua/lgc.c"
    "project/external/lua/linit.c"
    "project/external/lua/liolib.c"
    "project/external/lua/llex.c"
    "project/external/lua/lmathlib.c"
    "project/external/lua/lmem.c"
    "project/external/lua/loadlib.c"
    "project/external/lua/lobject.c"
    "project/external/lua/lopcodes.c"
    "project/external/lua/loslib.c"
    "project/external/lua/lparser.c"
    "project/external/lua/lstate.c"
    "project/external/lua/lstring.c"
    "project/external/lua/lstrlib.c"
    "project/external/lua/ltable.c"
    "project/external/lua/ltablib.c"
    "project/external/lua/ltm.c"
    "project/external/lua/lundump.c"
    "project/external/lua/lutf8lib.c"
    "project/external/lua/lvm.c"
    "project/external/lua/lzio.c"
    "project/external/zydis/zydis.c"
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)
set_target_properties(aetheris PROPERTIES
    PREFIX ""
    OUTPUT_NAME "aetheris"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin-portable"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin-portable"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin-portable"
)
