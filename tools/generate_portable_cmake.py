from pathlib import Path
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
project = ET.parse(ROOT / "aetheris.vcxproj").getroot()
ns = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}
sources = []
for node in project.findall(".//m:ClCompile", ns):
    value = node.attrib.get("Include")
    if value:
        sources.append(value.replace("\\", "/"))

required = {"project/entry.cpp"}
missing = required.difference(sources)
if missing:
    raise SystemExit(f"project source list is missing: {sorted(missing)}")

source_lines = "\n".join(f'    "{value}"' for value in sources)
c_sources = "\n".join(f'    "{value}"' for value in sources if value.endswith(".c"))
content = f'''cmake_minimum_required(VERSION 3.29)
cmake_policy(SET CMP0091 NEW)
project(aetheris_portable LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

set(AETHERIS_SOURCES
{source_lines}
)

add_library(aetheris SHARED ${{AETHERIS_SOURCES}})
target_include_directories(aetheris PRIVATE
    "${{CMAKE_CURRENT_SOURCE_DIR}}/project"
    "${{CMAKE_CURRENT_SOURCE_DIR}}/project/external/phnt"
    "${{CMAKE_CURRENT_SOURCE_DIR}}/project/external/lua"
    "${{CMAKE_CURRENT_SOURCE_DIR}}/vcpkg_installed/x64-windows-static/include"
)
target_link_directories(aetheris PRIVATE
    "${{CMAKE_CURRENT_SOURCE_DIR}}/vcpkg_installed/x64-windows-static/lib"
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
)
target_precompile_headers(aetheris PRIVATE "project/pch/pch.hpp")
set_source_files_properties(
{c_sources}
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)
set_target_properties(aetheris PROPERTIES
    PREFIX ""
    OUTPUT_NAME "aetheris"
    RUNTIME_OUTPUT_DIRECTORY "${{CMAKE_CURRENT_SOURCE_DIR}}/bin-portable"
    LIBRARY_OUTPUT_DIRECTORY "${{CMAKE_CURRENT_SOURCE_DIR}}/bin-portable"
    ARCHIVE_OUTPUT_DIRECTORY "${{CMAKE_CURRENT_SOURCE_DIR}}/bin-portable"
)
'''
(ROOT / "CMakeLists.txt").write_text(content, encoding="utf-8", newline="\n")
print(f"generated CMakeLists.txt with {len(sources)} sources")
