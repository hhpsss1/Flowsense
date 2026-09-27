#pragma once

#include <vector>
#include <string>

struct CModelChanger final
{
    struct ModelEntry_t {
        std::string m_Name;
        std::string m_Path;
    };

    auto OnInit() -> void;
    auto OnFrameStageNotify(int FrameStage) -> void;
    auto ScanModels() -> void;

    inline auto GetModels() -> std::vector<ModelEntry_t>& { return m_Models; }
    inline auto& GetSelectedIdx() { return m_SelectedIdx; }
    inline auto& NeedSetModel() { return m_NeedSetModel; }

private:
    auto ChangeModelNow() -> void;
    auto PrecacheResource(const std::string& path) -> void;

    std::vector<ModelEntry_t> m_Models;
    int m_SelectedIdx{ 0 };
    bool m_NeedSetModel{ false };

    void* (*m_fnPrecache)(void*, void*, const char*){ nullptr };
    const char* (__fastcall* m_fnInsert)(void*, int, const char*, int, bool){ nullptr };
    void* m_IRS{ nullptr };
    bool m_bInitialized{ false };
};

auto GetModelChanger() -> CModelChanger*;
