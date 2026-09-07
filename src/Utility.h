#pragma once

#include "SettingManager.h"

namespace POS
{
    void SendPrompt(const char* prompt, const char* sound = nullptr)
    {
        if (SettingManager::promptType == 1) {
            //May do play sound here
            RE::DebugMessageBox(prompt);
        }
        else {
            RE::SendHUDMessage::ShowHUDMessage(prompt, sound);
        }
    }

    template<typename T>
    const T& unmove(T&& x)
    {
        return x;
    }

    namespace util
    {
        template <class Context = std::format_context, class... Args>
        auto make_format_args(Args&&... vals)
        {
            return std::make_format_args(vals...);
        }
    }
    

    RE::ExtraPoison* GetExtraPoison(RE::InventoryEntryData* item)
    {
        if (!item)
            return nullptr;

        auto lists = item->extraLists;

        if (!lists || lists->empty())
            return nullptr;

        auto front = lists->front();

        if (!front)
            return nullptr;

        RE::ExtraPoison* extra = front->GetByType<RE::ExtraPoison>();

        return extra;
    }

    inline RE::ExtraDataList* CreateExtraDataList()
    {
        static bool is_se = !REL::Module::IsAE();


        auto a_this = (RE::ExtraDataList*)RE::malloc(is_se ? 0x18 : 0x20);

        using func_t = RE::ExtraDataList*(RE::ExtraDataList*);

        //SE: 107690, AE: 1135B0
        REL::Relocation<func_t> func{ REL::RelocationID{ 11437, 11583} };

        return func(a_this);
    }


    struct PoisonedState
    {
        enum
        {
            //This looks weird but I'm sure this will work fine.
            Compromised = -4,
            Seen = -3,
            None = -2,
            Unseen = 3,
            Escape = 4,
        };
    };
}