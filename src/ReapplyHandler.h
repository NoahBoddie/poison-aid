#pragma once

#include "Utility.h"

namespace POS
{

    struct ReapplyHandler : RE::BSTEventSink<RE::TESEquipEvent>
    {
        static ReapplyHandler* GetSingleton()
        {
            static ReapplyHandler singleton{};
            return std::addressof(singleton);
        }

        //I will not register this right now. I'm just gonna clear this when one reloads a save or some shit
        RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
        {
            //equip or not, if it's a weapon, spell
            return RE::BSEventNotifyControl::kContinue;
        }


        inline static float timePassed = 0;


        inline static std::mutex reapplyLock;

        struct ReapplyData
        {
            RE::FormID poisonID = 0;
            float timeStamp = 0.f;//Not used for right now.

            float GetTime()
            {
                return abs(timeStamp);
            }

            bool HasApplied()
            {
                return timeStamp < 0;
            }

            operator bool() { return poisonID; }

            void Clear()
            {
                poisonID = 0;
                timeStamp = 0.f;
            }

            ReapplyData() = default;
            ReapplyData(RE::AlchemyItem* p, bool re)
            {
                poisonID = p ? p->formID : 0;
                timeStamp = timePassed * (re ? -1 : 1);
            }
        };
        inline static ReapplyData reapplyData[2];

        static void Clear()
        {
            timePassed = 0;
            reapplyData[false].Clear();
            reapplyData[true].Clear();
        }

        static void _UpdateHand(bool hand)
        {
            auto& data = reapplyData[hand];

            if (!data)
                return;

            float comp_time = data.HasApplied() ? SettingManager::quickExtraApply : SettingManager::quickReapply;

            if (timePassed >= data.GetTime() + comp_time) {
                RE::PlaySound("UIAlchemyFail");
                data.Clear();
            }
        }

        static void Update(float update)
        {
            RE::UI* ui = RE::UI::GetSingleton();
            
            if (ui->numPausesGame)
                return;

            timePassed += update;

            _UpdateHand(false);
            _UpdateHand(true);
            
        }

        //This gets it's own manager too, needs to store data.
        static void HandleOutOfPoison(RE::PlayerCharacter* player, RE::InventoryEntryData* item)
        {

            RE::ExtraPoison* extra_poison = GetExtraPoison(item);

            if (!extra_poison || extra_poison->count > 1)
                return;



            RE::AlchemyItem* poison = extra_poison->poison;

            int is_worn = GetWorn(item);

            if (!is_worn)
                return;

            
            RE::PlaySound("ITMPoisonUse");



            //The below should happen additionally when activate is pressed, plus right or left hand attack.


            //For the amount of time it's here, I'll increment it, then decrement it. Before saving, it will be decremented. Or simply opening a menu will
            // cause it to decrement.
            //return;
            RE::InventoryChanges* changes = player->GetInventoryChanges();

            if (changes->GetItemCount(poison) > 0)
            {
                //Here to tell you if you have no doses instead of just some.
                std::string message = std::vformat(SettingManager::poisonRunOutMessage, std::make_format_args(poison->GetName()));
                RE::DebugNotification(message.c_str());

                //Should play a notification sound here to note that the poison you've run out of is reapplicable
                reapplyData[is_worn == -1] = ReapplyData(poison, false);

                //Create reapply data. For these purposes, only the formID is kept. Reservation is then left upto the inventory. I think this saves better,
                // preventing a situation where my mod will permanently increment the reference count
            }
        }

        //Should register for item unequips so that if I do widgets, I can make the disappear when the poison is gone.
        static bool HandleReapply(RE::PlayerCharacter* player, bool is_left)
        {

            if (!player)
                player = RE::PlayerCharacter::GetSingleton();

            //std::lock_guard<std::mutex> lock{ reapplyLock };

            //boolean reports if it should prevent the press event or not

            ReapplyData& data = reapplyData[is_left];

            if (!data)
                return false;

            RE::AlchemyItem* poison = RE::TESForm::LookupByID<RE::AlchemyItem>(reapplyData[is_left].poisonID);

            if (!poison)
            {
                //The poisong running out is cause for blocking, but also cause for notification.
                RE::PlaySound("MAGFailSD");
                return true;
            }

            RE::InventoryChanges* changes = player->GetInventoryChanges();

            //RE::ExtraDataList* safe_list = nullptr;

            if (changes->GetItemCount(poison) <= 0)
            {
                RE::PlaySound("MAGFailSD");
                return true;
            }
            /*
            else if (changes->entryList)
            {//I'm going to not use this, but it's to avoid quest items. Gonna hold off on it for now though.
                for (auto& entry : *changes->entryList)
                {
                    if (!entry->extraLists)
                        continue;

                    for (auto& entry : *changes->entryList)
                    {
                        for
                    }

                    if (!entry->extraLists)
                        continue;
                }
            }
            //*/

            RE::InventoryEntryData* hand_item = player->GetEquippedEntryData(is_left);

            if (!hand_item) {
                RE::PlaySound("MAGFailSD");
                return true;
            }
            PoisonHandler::SetPoison(poison, nullptr, player);
            //Temp placement, should be handle final
            //TODO: Reapplying should be using the secondary callback, as it has no reason not to.
            if (PoisonHandler::_HandleSecondary(player, hand_item, is_left) == PoisonResult::Success) {
                //Should have custom sound, and shouldn't apply if a different poison has been applied to your weapons.
                
                if (data.HasApplied() == false) {
                    std::string message = std::vformat(SettingManager::poisonReapplyMessage, std::make_format_args(poison->GetName(), hand_item->GetDisplayName()));
                    RE::DebugNotification(message.c_str());
                }

                data = ReapplyData(poison, true);
            }
            return true;
        }

    };
}