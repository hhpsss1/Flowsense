#pragma once

// Complete crash report system with full settings snapshot

#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <ctime>
#include <filesystem>

namespace crash_report {

// Helper to convert hex
inline std::string to_hex_string(std::uintptr_t value) {
    char buffer[32] = {};
    sprintf_s(buffer, "%llX", (unsigned long long)value);
    return buffer;
}

inline std::string to_string(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

inline std::wstring to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

// Crash severity levels
enum class severity {
    low, medium, high, critical
};

// Exception category
enum class crash_category {
    unknown,
    memory_violation,
    stack_overflow,
    division_by_zero,
    illegal_instruction,
    hook_failure,
    initialization_failure,
    pattern_scan_failure,
    renderer_error,
    network_error
};

// Exception information
struct exception_info {
    DWORD exception_code = 0;
    std::uintptr_t exception_address = 0;
    std::uintptr_t accessed_address = 0;
    DWORD access_type = 0;
    DWORD thread_id = 0;
    DWORD process_id = 0;
    crash_category category = crash_category::unknown;
    std::string category_name;
    std::string exception_name;
};

// System information
struct system_info {
    OSVERSIONINFOW os_version = {};
    SYSTEM_INFO cpu_info = {};
    MEMORYSTATUSEX memory = {};
    std::vector<std::pair<std::wstring, std::wstring>> loaded_modules;
    std::wstring computer_name;
    std::wstring user_name;
};

// Register state
struct register_state {
    std::uintptr_t rip = 0, rsp = 0, rbp = 0;
    std::uintptr_t rax = 0, rbx = 0, rcx = 0, rdx = 0;
    std::uintptr_t rsi = 0, rdi = 0;
    std::uintptr_t r8 = 0, r9 = 0, r10 = 0, r11 = 0;
    std::uintptr_t r12 = 0, r13 = 0, r14 = 0, r15 = 0;
};

// Stack frame
struct stack_frame_info {
    std::uintptr_t address = 0;
    std::string module_name;
    std::string function_name;
    std::string source_file;
    int source_line = 0;
};

// Fix suggestion
struct fix_suggestion {
    std::string title;
    std::string description;
    std::vector<std::string> steps;
};

// Enabled feature/setting
struct enabled_setting {
    std::string category;
    std::string name;
    std::string value;
    bool is_critical;
};

// Main crash report
struct crash_report_data {
    exception_info exception;
    system_info system;
    register_state registers;
    std::vector<stack_frame_info> stack_trace;
    std::vector<fix_suggestion> suggestions;
    std::vector<enabled_setting> enabled_settings;
    std::string report_path;
    std::string minidump_path;
    std::string build_version;
    std::string build_config;
    std::string game_version;
    std::string dll_path;
    std::uintptr_t dll_base = 0;
    std::uintptr_t dll_size = 0;
    std::string crash_time_str;
    severity crash_severity = severity::medium;
    bool is_in_nexoria_module = false;
    std::string faulting_module;
    std::string game_process_info;
    DWORD game_process_id = 0;
};

// ============================================================================
// Settings Snapshot - Capture what was enabled
// ============================================================================

struct settings_snapshot {
    std::vector<enabled_setting> enabled_features;

    void add(const std::string& category, const std::string& name, bool enabled, bool critical = false) {
        enabled_setting s;
        s.category = category;
        s.name = name;
        s.value = enabled ? "ENABLED" : "disabled";
        s.is_critical = critical;
        enabled_features.push_back(s);
    }

    void add(const std::string& category, const std::string& name, const std::string& value, bool critical = false) {
        enabled_setting s;
        s.category = category;
        s.name = name;
        s.value = value;
        s.is_critical = critical;
        enabled_features.push_back(s);
    }
};

// Global settings accessor - this will be called to capture current state
typedef settings_snapshot(*get_settings_fn)();
get_settings_fn g_settings_getter = nullptr;

inline void register_settings_getter(get_settings_fn fn) {
    g_settings_getter = fn;
}

inline settings_snapshot get_current_settings() {
    if (g_settings_getter) {
        return g_settings_getter();
    }
    return settings_snapshot{};
}

// ============================================================================
// Helper functions
// ============================================================================

inline const char* severity_to_string(severity s) {
    switch (s) {
        case severity::low: return "LOW";
        case severity::medium: return "MEDIUM";
        case severity::high: return "HIGH";
        case severity::critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

inline const char* category_to_string(crash_category c) {
    switch (c) {
        case crash_category::unknown: return "Unknown";
        case crash_category::memory_violation: return "Memory Violation";
        case crash_category::stack_overflow: return "Stack Overflow";
        case crash_category::division_by_zero: return "Division by Zero";
        case crash_category::illegal_instruction: return "Illegal Instruction";
        case crash_category::hook_failure: return "Hook Failure";
        case crash_category::initialization_failure: return "Initialization Failure";
        case crash_category::pattern_scan_failure: return "Pattern Scan Failure";
        case crash_category::renderer_error: return "Renderer Error";
        case crash_category::network_error: return "Network Error";
    }
    return "Unknown";
}

inline crash_category categorize_exception(DWORD code) {
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION: return crash_category::memory_violation;
        case EXCEPTION_STACK_OVERFLOW: return crash_category::stack_overflow;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO: return crash_category::division_by_zero;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION: return crash_category::illegal_instruction;
        default: return crash_category::unknown;
    }
}

inline const char* exception_code_to_name(DWORD code) {
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION: return "ACCESS_VIOLATION";
        case EXCEPTION_STACK_OVERFLOW: return "STACK_OVERFLOW";
        case EXCEPTION_INT_DIVIDE_BY_ZERO: return "INT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION";
        case EXCEPTION_PRIV_INSTRUCTION: return "PRIV_INSTRUCTION";
        default: return "UNKNOWN";
    }
}

inline bool is_nexoria_address(std::uintptr_t addr, std::uintptr_t base, std::uintptr_t size) {
    return addr >= base && addr < base + size;
}

// ============================================================================
// Directory Creation - uses Windows API for reliability
// ============================================================================

inline bool create_directory_recursive(const std::wstring& path) {
    if (path.empty()) return false;

    // Try Windows API first (more reliable than std::filesystem in crash context)
    if (CreateDirectoryW(path.c_str(), nullptr) == TRUE) {
        return true;
    }

    DWORD err = GetLastError();
    if (err == ERROR_ALREADY_EXISTS) {
        return true;
    }

    // If failed because parent doesn't exist, try to create parent
    if (err == ERROR_PATH_NOT_FOUND) {
        // Find the parent path
        size_t pos = path.find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            std::wstring parent = path.substr(0, pos);
            if (!parent.empty() && create_directory_recursive(parent)) {
                // Now try to create the full path again
                return CreateDirectoryW(path.c_str(), nullptr) == TRUE;
            }
        }
    }

    return false;
}

inline std::wstring get_crash_directory() {
    // Try primary directory first
    constexpr wchar_t k_primary_dir[] = L"C:\\nexoria\\log";

    if (create_directory_recursive(k_primary_dir)) {
        return std::wstring{ k_primary_dir };
    }

    // Fallback to temp directory if primary fails
    wchar_t temp_path[MAX_PATH] = {};
    if (GetTempPathW(MAX_PATH, temp_path)) {
        std::wstring fallback = std::wstring{temp_path} + L"nexoria_crashes\\";
        if (create_directory_recursive(fallback)) {
            return fallback;
        }
    }

    // Last resort - current directory
    return std::wstring{ L"." };
}

// ============================================================================
// System Information Collection
// ============================================================================

inline void collect_system_info(system_info& info) {
    info.os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&info.os_version);
    GetNativeSystemInfo(&info.cpu_info);
    info.memory.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&info.memory);

    // Get computer and user name
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    wchar_t computer_name[MAX_COMPUTERNAME_LENGTH + 1] = {};
    GetComputerNameW(computer_name, &size);
    info.computer_name = computer_name;

    size = MAX_PATH;
    wchar_t user_name[MAX_PATH] = {};
    GetUserNameW(user_name, &size);
    info.user_name = user_name;

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (snap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W me = { sizeof(MODULEENTRY32W) };
        if (Module32FirstW(snap, &me)) {
            do {
                info.loaded_modules.push_back({me.szModule, me.szExePath});
            } while (Module32NextW(snap, &me));
        }
        CloseHandle(snap);
    }
}

// Collect registers
inline void collect_registers(register_state& regs, CONTEXT* ctx) {
#if defined(_M_X64)
    regs.rip = ctx->Rip; regs.rsp = ctx->Rsp; regs.rbp = ctx->Rbp;
    regs.rax = ctx->Rax; regs.rbx = ctx->Rbx; regs.rcx = ctx->Rcx; regs.rdx = ctx->Rdx;
    regs.rsi = ctx->Rsi; regs.rdi = ctx->Rdi;
    regs.r8 = ctx->R8; regs.r9 = ctx->R9; regs.r10 = ctx->R10; regs.r11 = ctx->R11;
    regs.r12 = ctx->R12; regs.r13 = ctx->R13; regs.r14 = ctx->R14; regs.r15 = ctx->R15;
#endif
}

// Stack walking
inline void capture_stack_trace(std::vector<stack_frame_info>& frames, CONTEXT* ctx) {
    static constexpr int MAX_FRAMES = 32;
    static char sym_buf[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];

    static bool init = false;
    if (!init) {
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
        SymInitialize(GetCurrentProcess(), NULL, TRUE);
        init = true;
    }

    CONTEXT c = *ctx;
    STACKFRAME64 sf = {};

#if defined(_M_X64)
    sf.AddrPC.Offset = c.Rip; sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Offset = c.Rsp; sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = c.Rbp; sf.AddrFrame.Mode = AddrModeFlat;
#endif

    for (int i = 0; i < MAX_FRAMES; i++) {
        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(),
                         &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
            break;

        stack_frame_info frm = {};
        frm.address = sf.AddrPC.Offset;

        HMODULE mod = (HMODULE)SymGetModuleBase64(GetCurrentProcess(), frm.address);
        if (mod) {
            char path[MAX_PATH] = {};
            GetModuleFileNameA(mod, path, MAX_PATH);
            char* name = strrchr(path, '\\');
            frm.module_name = name ? name + 1 : path;
        }

        SYMBOL_INFO* si = (SYMBOL_INFO*)sym_buf;
        si->SizeOfStruct = sizeof(SYMBOL_INFO);
        si->MaxNameLen = MAX_SYM_NAME;
        if (SymFromAddr(GetCurrentProcess(), frm.address, NULL, si)) {
            frm.function_name = si->Name ? si->Name : "(unknown)";
        }

        IMAGEHLP_LINE64 line = {};
        DWORD disp = 0;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if (SymGetLineFromAddr64(GetCurrentProcess(), frm.address, &disp, &line)) {
            frm.source_file = line.FileName ? line.FileName : "";
            frm.source_line = line.LineNumber;
        }

        frames.push_back(frm);
        if (sf.AddrReturn.Offset == 0) break;
    }
}

// ============================================================================
// Generate fixes based on category
// ============================================================================

inline std::vector<fix_suggestion> generate_fixes(crash_category cat, exception_info& exc) {
    std::vector<fix_suggestion> fixes;

    fix_suggestion specific = {};

    switch (cat) {
        case crash_category::memory_violation:
            specific.title = "Memory Access Violation";
            specific.description = "Attempted to access invalid memory. Access type: " +
                std::string(exc.access_type == 1 ? "WRITE" : exc.access_type == 8 ? "EXECUTE" : "READ") +
                " at address 0x" + to_hex_string(exc.accessed_address);
            specific.steps = {
                "1. Check for null pointer dereferences",
                "2. Verify array bounds are not exceeded",
                "3. Check for use-after-free conditions",
                "4. Review memory allocation/deallocation patterns",
                "5. Disable recent features one by one to isolate the cause"
            };
            break;

        case crash_category::stack_overflow:
            specific.title = "Stack Overflow";
            specific.description = "Stack memory exhausted, likely from deep recursion or large allocations.";
            specific.steps = {
                "1. Check for infinite recursion in enabled features",
                "2. Disable features that process large amounts of data",
                "3. Reduce render distance or quality settings",
                "4. Disable ESP features on many entities"
            };
            break;

        case crash_category::division_by_zero:
            specific.title = "Division by Zero";
            specific.description = "Attempted to divide by zero.";
            specific.steps = {
                "1. Try different weapon or skin combinations",
                "2. Reset aimbot settings to default",
                "3. Check triggerbot conditions"
            };
            break;

        case crash_category::illegal_instruction:
            specific.title = "Illegal Instruction";
            specific.description = "CPU encountered invalid instruction.";
            specific.steps = {
                "1. Update graphics drivers",
                "2. Verify game files integrity",
                "3. Try disabling visual features"
            };
            break;

        case crash_category::hook_failure:
            specific.title = "Hook Installation Failed";
            specific.description = "Failed to install a function hook.";
            specific.steps = {
                "1. Update to latest game version",
                "2. Disable other injected mods",
                "3. Try running game as administrator"
            };
            break;

        case crash_category::initialization_failure:
            specific.title = "Initialization Failed";
            specific.description = "Failed to initialize a required component.";
            specific.steps = {
                "1. Verify game is fully loaded before injection",
                "2. Check if game version is supported",
                "3. Try disabling non-essential features"
            };
            break;

        case crash_category::pattern_scan_failure:
            specific.title = "Pattern Scan Failed";
            specific.description = "Failed to find required offsets. Game may have updated.";
            specific.steps = {
                "1. Check if game was updated",
                "2. Wait for offset update release",
                "3. Verify game version compatibility"
            };
            break;

        default:
            specific.title = "Unknown Crash";
            specific.description = "Analyze the stack trace and enabled features to determine root cause.";
            specific.steps = {
                "1. Review the stack trace location",
                "2. Note which features were enabled",
                "3. Try disabling features to isolate cause",
                "4. Check if reproducible with minimal features"
            };
    }
    fixes.push_back(specific);

    // General fix
    fix_suggestion general = {};
    general.title = "General Troubleshooting";
    general.description = "Steps that often resolve crash issues.";
    general.steps = {
        "1. Update to the latest version of Nexoria",
        "2. Run game and Nexoria as administrator",
        "3. Disable antivirus/firewall temporarily",
        "4. Verify game files integrity (Steam)",
        "5. Update graphics drivers",
        "6. Disable other injected mods/overlays",
        "7. Try reducing Nexoria quality settings"
    };
    fixes.push_back(general);

    return fixes;
}

// ============================================================================
// Report Generation
// ============================================================================

inline std::string generate_report(const crash_report_data& r) {
    std::string out;

    out += "================================================================================\n";
    out += "                    NEXORIA CS2 CRASH REPORT\n";
    out += "================================================================================\n\n";

    out += "[!] IMPORTANT: This report contains the state of Nexoria at crash time.\n";
    out += "    Please include this file when reporting crashes.\n\n";

    // Header
    out += "CRASH SUMMARY\n";
    out += "--------------------------------------------------------------------------------\n";
    out += "  Timestamp:      " + r.crash_time_str + "\n";
    out += "  Severity:       " + std::string(severity_to_string(r.crash_severity)) + "\n";
    out += "  Category:       " + r.exception.category_name + "\n";
    out += "  Exception:      " + r.exception.exception_name + " (0x" + to_hex_string(r.exception.exception_code) + ")\n";
    out += "  Fault Address:  0x" + to_hex_string(r.exception.exception_address) + "\n";
    out += "  Thread ID:      " + std::to_string(r.exception.thread_id) + "\n";
    out += "  Process ID:     " + std::to_string(r.exception.process_id) + "\n\n";

    // Game Info
    out += "GAME INFORMATION\n";
    out += "--------------------------------------------------------------------------------\n";
    out += "  Game:           Counter-Strike 2\n";
    out += "  Game Version:   " + r.game_version + "\n";
    out += "  Nexoria Ver:    " + r.build_version + " (" + r.build_config + ")\n";
    out += "  DLL Path:       " + r.dll_path + "\n";
    out += "  DLL Base:       0x" + to_hex_string(r.dll_base) + "\n";
    out += "  DLL Size:       0x" + to_hex_string(r.dll_size) + "\n";
    out += "  Game PID:       " + std::to_string(r.game_process_id) + "\n\n";

    // Fault Location
    out += "FAULT LOCATION\n";
    out += "--------------------------------------------------------------------------------\n";
    out += "  Faulting Module: " + r.faulting_module + "\n";
    out += "  In Nexoria DLL:  " + std::string(r.is_in_nexoria_module ? "YES [Likely Nexoria bug]" : "NO [May be game/other mod]") + "\n";
    if (r.exception.access_type) {
        out += "  Access Type:     " + std::string(r.exception.access_type == 1 ? "WRITE" : "READ") + "\n";
        out += "  Accessed Addr:    0x" + to_hex_string(r.exception.accessed_address) + "\n";
    }
    out += "\n";

    // Enabled Settings - THE IMPORTANT PART
    out += "================================================================================\n";
    out += "                    ENABLED FEATURES AT CRASH TIME\n";
    out += "================================================================================\n";
    out += "[!] This shows what was ACTIVE when the crash occurred.\n";
    out += "    Use this to identify which feature caused the crash.\n\n";

    if (r.enabled_settings.empty()) {
        out += "  [Settings snapshot not available]\n\n";
    } else {
        // Group by category
        std::unordered_map<std::string, std::vector<const enabled_setting*>> grouped;
        for (const auto& s : r.enabled_settings) {
            grouped[s.category].push_back(&s);
        }

        for (const auto& [category, settings] : grouped) {
            out += "  " + category + ":\n";
            for (const auto* s : settings) {
                std::string marker = s->is_critical ? " [!]" : "";
                std::string status = (s->value == "ENABLED" || s->value == "enabled") ? "[ON] " : "[--] ";
                out += "    " + status + s->name + " = " + s->value + marker + "\n";
            }
            out += "\n";
        }
    }

    // Register State
    out += "REGISTER STATE (x64)\n";
    out += "--------------------------------------------------------------------------------\n";
#if defined(_M_X64)
    out += "  RIP: 0x" + to_hex_string(r.registers.rip) + "  RSP: 0x" + to_hex_string(r.registers.rsp) + "\n";
    out += "  RBP: 0x" + to_hex_string(r.registers.rbp) + "  RAX: 0x" + to_hex_string(r.registers.rax) + "\n";
    out += "  RBX: 0x" + to_hex_string(r.registers.rbx) + "  RCX: 0x" + to_hex_string(r.registers.rcx) + "\n";
    out += "  RDX: 0x" + to_hex_string(r.registers.rdx) + "  RSI: 0x" + to_hex_string(r.registers.rsi) + "\n";
    out += "  RDI: 0x" + to_hex_string(r.registers.rdi) + "  R8:  0x" + to_hex_string(r.registers.r8) + "\n";
    out += "  R9:  0x" + to_hex_string(r.registers.r9) + "   R10: 0x" + to_hex_string(r.registers.r10) + "\n";
    out += "  R11: 0x" + to_hex_string(r.registers.r11) + "  R12: 0x" + to_hex_string(r.registers.r12) + "\n";
    out += "  R13: 0x" + to_hex_string(r.registers.r13) + "  R14: 0x" + to_hex_string(r.registers.r14) + "\n";
    out += "  R15: 0x" + to_hex_string(r.registers.r15) + "\n";
#endif
    out += "\n";

    // Stack Trace
    out += "STACK TRACE\n";
    out += "--------------------------------------------------------------------------------\n";
    if (r.stack_trace.empty()) {
        out += "  [No stack trace available]\n";
    } else {
        for (size_t i = 0; i < r.stack_trace.size(); i++) {
            const auto& f = r.stack_trace[i];
            std::string indent = (i < 10) ? " " : "";
            out += "  #" + indent + std::to_string(i) + " 0x" + to_hex_string(f.address) + " " + f.module_name + "!" + f.function_name + "\n";
            if (!f.source_file.empty()) {
                out += "         at " + f.source_file + ":" + std::to_string(f.source_line) + "\n";
            }
        }
    }
    out += "\n";

    // Loaded Modules (filtered to relevant)
    out += "LOADED MODULES (Game Related)\n";
    out += "--------------------------------------------------------------------------------\n";
    std::vector<std::string> game_modules = {"cs2", "engine2", "shaderapidx9", "tier0", "vstdlib",
                                              "materialsystem", "vguimatsurface", "vgui2", "inputsystem"};
    for (const auto& m : r.system.loaded_modules) {
        std::string name = to_string(m.first);
        for (const auto& gm : game_modules) {
            if (name.find(gm) != std::string::npos) {
                out += "  " + name + "\n";
                break;
            }
        }
    }
    out += "\n";

    // System Information
    out += "SYSTEM INFORMATION\n";
    out += "--------------------------------------------------------------------------------\n";
    char buf[256] = {};
    sprintf_s(buf, "  Computer:       %ls", r.system.computer_name.c_str());
    out += buf; out += "\n";
    sprintf_s(buf, "  User:           %ls", r.system.user_name.c_str());
    out += buf; out += "\n";
    sprintf_s(buf, "  OS:             Windows %lu.%lu.%lu",
        r.system.os_version.dwMajorVersion,
        r.system.os_version.dwMinorVersion,
        r.system.os_version.dwBuildNumber);
    out += buf; out += "\n";
    sprintf_s(buf, "  CPU:            %lu logical processors",
        r.system.cpu_info.dwNumberOfProcessors);
    out += buf; out += "\n";
    sprintf_s(buf, "  RAM:            %llu MB total, %llu MB available",
        r.system.memory.ullTotalPhys / (1024 * 1024),
        r.system.memory.ullAvailPhys / (1024 * 1024));
    out += buf; out += "\n\n";

    // Fix Suggestions
    out += "================================================================================\n";
    out += "                          FIX SUGGESTIONS\n";
    out += "================================================================================\n\n";

    for (size_t i = 0; i < r.suggestions.size(); i++) {
        const auto& s = r.suggestions[i];
        out += "[" + std::to_string(i + 1) + "] " + s.title + "\n";
        out += "--------------------------------------------------------------------------------\n";
        out += s.description + "\n\n";
        out += "Steps to resolve:\n";
        for (const auto& step : s.steps) {
            out += "  " + step + "\n";
        }
        out += "\n";
    }

    // Files
    out += "================================================================================\n";
    out += "                          FILES GENERATED\n";
    out += "================================================================================\n\n";
    out += "  Full Report:  " + r.report_path + "\n";
    out += "  Minidump:     " + r.minidump_path + "\n\n";
    out += "================================================================================\n";
    out += "           Generated by Nexoria Crash Reporter\n";
    out += "           Please include this file when reporting bugs\n";
    out += "================================================================================\n";

    return out;
}

// Write crash report file
inline bool write_report(const crash_report_data& data) {
    // Try primary path first
    std::string final_path = data.report_path;

    // Ensure directory exists
    std::wstring dir_path = get_crash_directory();

    // Generate report content
    std::string content = generate_report(data);

    // Try writing with the original path
    HANDLE file = CreateFileA(final_path.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        // Try in temp directory as fallback
        wchar_t temp_path[MAX_PATH] = {};
        if (GetTempPathW(MAX_PATH, temp_path)) {
            char filename[MAX_PATH] = {};
            const char* basename = strrchr(final_path.c_str(), '\\');
            if (basename) {
                strcpy_s(filename, basename + 1);
            } else {
                strcpy_s(filename, final_path.c_str());
            }

            char fallback_path[MAX_PATH] = {};
            sprintf_s(fallback_path, "%lsnexoria_crashes\\%s", temp_path, filename);
            final_path = fallback_path;

            // Ensure fallback directory exists
            std::wstring fallback_dir = std::wstring{temp_path} + L"nexoria_crashes";
            create_directory_recursive(fallback_dir);

            file = CreateFileA(fallback_path, GENERIC_WRITE, FILE_SHARE_READ,
                nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        }
    }

    if (file == INVALID_HANDLE_VALUE) {
        // Last resort - write to current directory
        const char* name = strrchr(final_path.c_str(), '\\');
        if (name) name++;
        else name = final_path.c_str();

        file = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ,
            nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    }

    if (file == INVALID_HANDLE_VALUE) {
        // Can't write file - try to at least show debug output
        OutputDebugStringA("[CRASH_REPORT] Failed to write crash report file!\n");
        OutputDebugStringA(content.c_str());
        return false;
    }

    DWORD written = 0;
    WriteFile(file, content.c_str(), (DWORD)content.size(), &written, nullptr);
    FlushFileBuffers(file);
    CloseHandle(file);

    // Update path in data for the report
    const_cast<crash_report_data&>(data).report_path = final_path;

    return true;
}

// Write minidump
inline bool write_minidump(const char* path, EXCEPTION_POINTERS* info) {
    // Ensure directory exists first
    std::wstring dir_path = get_crash_directory();

    HANDLE file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        // Try fallback path
        wchar_t temp_path[MAX_PATH] = {};
        if (GetTempPathW(MAX_PATH, temp_path)) {
            char filename[MAX_PATH] = {};
            const char* basename = strrchr(path, '\\');
            if (basename) {
                strcpy_s(filename, basename + 1);
            } else {
                strcpy_s(filename, path);
            }

            char fallback_path[MAX_PATH] = {};
            sprintf_s(fallback_path, "%lsnexoria_crashes\\%s", temp_path, filename);

            std::wstring fallback_dir = std::wstring{temp_path} + L"nexoria_crashes";
            create_directory_recursive(fallback_dir);

            file = CreateFileA(fallback_path, GENERIC_WRITE, FILE_SHARE_READ,
                nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (file != INVALID_HANDLE_VALUE) {
                // Update path in info (will need to be returned separately)
                OutputDebugStringA("[CRASH_REPORT] Minidump written to fallback path\n");
            }
        }
    }

    if (file == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("[CRASH_REPORT] Failed to create minidump file\n");
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION ex = {};
    ex.ThreadId = GetCurrentThreadId();
    ex.ExceptionPointers = info;
    ex.ClientPointers = FALSE;

    BOOL res = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
        (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithThreadInfo),
        &ex, nullptr, nullptr);

    CloseHandle(file);
    return res != FALSE;
}

// Create and save crash report
inline void handle_crash(EXCEPTION_POINTERS* info, const char* stage,
                        HMODULE module, const char* version = "1.0.0",
                        const char* config = "Release", const char* game = "CS2") {
    OutputDebugStringA("[CRASH_REPORT] handle_crash called\n");

    if (!info || !info->ExceptionRecord) {
        OutputDebugStringA("[CRASH_REPORT] ERROR: Invalid exception pointers\n");
        return;
    }

    crash_report_data r = {};

    // Timestamp
    time_t now = time(nullptr);
    char time_str[128] = {};
    struct tm tm_info = {};
    if (localtime_s(&tm_info, &now) == 0) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);
    }
    r.crash_time_str = time_str;

    // DLL info
    r.dll_base = (std::uintptr_t)module;
    wchar_t dll_path[MAX_PATH] = {};
    GetModuleFileNameW(module, dll_path, MAX_PATH);
    r.dll_path = to_string(dll_path);

    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)module;
    if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
        IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((char*)module + dos->e_lfanew);
        if (nt->Signature == IMAGE_NT_SIGNATURE) {
            r.dll_size = nt->OptionalHeader.SizeOfImage;
        }
    }

    r.build_version = version;
    r.build_config = config;
    r.game_version = game;

    // Exception info
    r.exception.exception_code = info->ExceptionRecord->ExceptionCode;
    r.exception.exception_address = (std::uintptr_t)info->ExceptionRecord->ExceptionAddress;
    r.exception.thread_id = GetCurrentThreadId();
    r.exception.process_id = GetCurrentProcessId();

    if (info->ExceptionRecord->NumberParameters >= 2) {
        r.exception.access_type = (DWORD)info->ExceptionRecord->ExceptionInformation[0];
        r.exception.accessed_address = (std::uintptr_t)info->ExceptionRecord->ExceptionInformation[1];
    }

    r.exception.category = categorize_exception(r.exception.exception_code);
    r.exception.exception_name = exception_code_to_name(r.exception.exception_code);
    r.exception.category_name = category_to_string(r.exception.category);

    r.crash_severity = (r.exception.category == crash_category::stack_overflow) ?
        severity::critical : severity::high;

    // Registers
    if (info->ContextRecord) {
        collect_registers(r.registers, info->ContextRecord);
    }

    // Check fault location
    r.is_in_nexoria_module = is_nexoria_address(r.exception.exception_address, r.dll_base, r.dll_size);

    HMODULE fault_mod = nullptr;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                          (LPCSTR)r.exception.exception_address, &fault_mod)) {
        char name[MAX_PATH] = {};
        GetModuleFileNameA(fault_mod, name, MAX_PATH);
        char* n = strrchr(name, '\\');
        r.faulting_module = n ? n + 1 : name;
    }

    // Stack trace
    if (info->ContextRecord) {
        capture_stack_trace(r.stack_trace, info->ContextRecord);
    }

    // System info
    collect_system_info(r.system);

    // Capture enabled settings
    settings_snapshot snapshot = get_current_settings();
    r.enabled_settings = snapshot.enabled_features;

    // Fix suggestions
    r.suggestions = generate_fixes(r.exception.category, r.exception);

    // Get crash directory (fixed location)
    std::wstring crash_dir = get_crash_directory();
    OutputDebugStringA("[CRASH_REPORT] Crash directory: ");
    OutputDebugStringW((std::wstring(crash_dir) + L"\n").c_str());

    bool dir_ok = create_directory_recursive(crash_dir);
    if (!dir_ok) {
        OutputDebugStringA("[CRASH_REPORT] WARNING: Failed to create directory, using fallback\n");
    }

    // File paths with timestamp
    wchar_t report_path[MAX_PATH] = {};
    swprintf_s(report_path, MAX_PATH, L"%s\\crash_report_%04d%02d%02d_%02d%02d%02d.txt",
        crash_dir.c_str(), tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
        tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
    r.report_path = to_string(report_path);

    wchar_t dump_path[MAX_PATH] = {};
    swprintf_s(dump_path, MAX_PATH, L"%s\\crash_%04d%02d%02d_%02d%02d%02d.dmp",
        crash_dir.c_str(), tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
        tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
    r.minidump_path = to_string(dump_path);

    OutputDebugStringA("[CRASH_REPORT] Writing report...\n");

    // Write files
    bool report_ok = write_report(r);
    bool dump_ok = write_minidump(r.minidump_path.c_str(), info);

    if (report_ok) {
        OutputDebugStringA("[CRASH_REPORT] Report written successfully\n");
    } else {
        OutputDebugStringA("[CRASH_REPORT] ERROR: Failed to write report\n");
    }

    if (dump_ok) {
        OutputDebugStringA("[CRASH_REPORT] Minidump written successfully\n");
    } else {
        OutputDebugStringA("[CRASH_REPORT] ERROR: Failed to write minidump\n");
    }
}

} // namespace crash_report
