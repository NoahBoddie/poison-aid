#pragma once 

#include "SettingManager.h"
#include "Utility.h"

namespace POS
{
    /*
    DirectApplySpellOnTarget_14054CBE0(
        ActorMagicCaster *a1,
        MagicItem *rdx0,
        Actor *target,
        TESForm *weapon,
        char a5)
    //*/

	//using MsgCallback = void(int, bool);
	//sub_1406A1E30 apply poison callback. Should follow the above, and should also be loaded else where.
    //6A1E30
    //Give these a home plz
    void ApplyPoisonCallback(int a1, bool a2)
    {
        //The original only took an integer, the bool is forced.
        //SE: 0x6a1e30, AE: 0x6DC830
        using func_t = decltype(&ApplyPoisonCallback);
        REL::Relocation<func_t> func{ RELOCATION_ID(39407, 40482) };
        return func(a1, a2);
    }

    //This likely is as described, second item is a form pointer, use this instead of inventory list and see how mileage varies.
    void UpdateRefInventory_Query(RE::TESObjectREFR* a_this, uint64_t a2)
    {
        //SE: 0x8D5710, AE: 0x9180D0
        using func_t = decltype(&UpdateRefInventory_Query);
        REL::Relocation<func_t> func{ RELOCATION_ID(51911, 52849) };
        return func(a_this, a2);
    }

    void DecrementCreatedPoisonRef(RE::BGSCreatedObjectManager* a_this, RE::AlchemyItem* a2)
    {
        //SE: 0x59F6E0, AE: 0x5D3220
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&DecrementCreatedPoisonRef);
        REL::Relocation<func_t> func{ RELOCATION_ID(35269, 36171) };
        return func(a_this, a2);
    }


    void IncrementCreatedPoisonRef(RE::BGSCreatedObjectManager* a_this, RE::AlchemyItem* a2)
    {
        //SE: 0x59F480, AE: 0x5D3110
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&IncrementCreatedPoisonRef);
        REL::Relocation<func_t> func{ RELOCATION_ID(35268, 36170) };
        return func(a_this, a2);
    }



    //ExtraDataList
    bool GetUniqueIDs(RE::ExtraDataList* a_this, uint16_t& special_id, RE::FormID& form_id)
    {
        //SE: 0x125270, AE: 0x12E020
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&GetUniqueIDs);
        REL::Relocation<func_t> func{ RELOCATION_ID(11891, 12030) };
        return func(a_this, special_id, form_id);
    } 
    
    //ExtraDataList
    bool GetReferenceHandle(RE::ExtraDataList* a_this, RE::ObjectRefHandle& handle)
    {
        //SE: 0x113F00, AE: 0x120E10
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&GetReferenceHandle);
        REL::Relocation<func_t> func{ RELOCATION_ID(11573, 11719) };
        return func(a_this, handle);
    }



    //ExtraDataList
    template<class T>
    void SendEquipEvent(RE::ScriptEventSourceHolder* a_this, T& unk, RE::TESBoundObject* item, uint16_t unique_id, RE::FormID ref_id, bool equipped)
    {
        //SE: 0x61E420, AE: 0x656280
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&SendEquipEvent<T>);
        REL::Relocation<func_t> func{ RELOCATION_ID(37438, 38386) };
        return func(a_this, unk, item, unique_id, ref_id, equipped);
    }
    //-First: source holder, 2nd: objectReference*, 3rd: BoundObj form, 4th: uniqueID, 5th: the FormID of the equipped object, 6th: True, has been equipped.

    //TESBoundObject
    void SendActorItemEquippedEvent(RE::TESBoundObject* a_this, RE::Actor* actor)
    {
        //SE: 0x610110, AE: 0x64A880
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&SendActorItemEquippedEvent);
        REL::Relocation<func_t> func{ RELOCATION_ID(37038, 38066) };
        return func(a_this, actor);
    }

    void ClearInterfaceEffects_query(RE::ProcessLists* a_this)
    {
        //Seems to target interface effects and calls free?

        //SE: 0x6DD850, AE: 0x718600
        using func_t = decltype(&ClearInterfaceEffects_query);
        REL::Relocation<func_t> func{ RELOCATION_ID(40380, 41394) };
        return func(a_this);
    }






    struct UnkType {};

    UnkType* GetUnk()
    {
        //SE: 0x6C7F40, AE: 0x7024B0
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&GetUnk);
        REL::Relocation<func_t> func{ RELOCATION_ID(40058, 202120) };
        return func();
    }

    void UseUnk(UnkType* a_this, RE::TESForm** a2)
    {
        //SE: 0x6C52D0, AE: 0x6FFD20
        //The original only took an integer, the bool is forced.
        using func_t = decltype(&UseUnk);
        REL::Relocation<func_t> func{ RELOCATION_ID(40033, 41044) };
        return func(a_this, a2);
    }


    RE::ExtraPoison* GetExtraPoison(RE::InventoryEntryData* item)
    {
        auto lists = item->extraLists;

        if (!lists || lists->empty())
            return nullptr;

        auto front = lists->front();

        if (!front)
            return nullptr;

        RE::ExtraPoison* extra = front->GetByType<RE::ExtraPoison>();

        return extra;
    }

    bool HasEntry(RE::Character* target, RE::TESBoundObject* item, RE::ExtraDataList* list)
    {
        if (!target)
            return false;

        auto inventory = target->GetInventory();

        auto& entry = inventory[item].second;

        if (!entry)
            return false;

        if (!list)
            return true;

        if (!entry->extraLists)
            return false;

        for (auto& extra_list : *entry->extraLists)
        {
            if (extra_list == list)
                return true;
        }

        return false;
    }


    int GetWorn(RE::InventoryEntryData* item)
    {
        auto lists = item->extraLists;

        if (!lists || lists->empty())
            return 0;

        auto front = lists->front();

        if (!front)
            return 0;

        if (front->HasType<RE::ExtraWorn>() == true)
            return 1;


        if (front->HasType<RE::ExtraWornLeft>() == true)
            return -1;


        return 0;
    }


	//For the message choices, instead of relying on the first index stuff, I'll be making a function
	// that handles all of those in a constexpr function that translates the choices.
	// It will do something like <Choice, choice, choice>, and it will make those choices into an array
	// it will then send to the true handle function.
    namespace detail
    {
        struct PrimaryType__Type
        {
            enum Type
            {
                Cancel      = 0 << 0,
                PoisonRight = 1 << 0,
                PoisonLeft  = 1 << 1,
                PoisonSelf  = 1 << 2
            };
        };
    }
    using PrimaryType = detail::PrimaryType__Type::Type;

    namespace detail
    {
        struct SecondaryType__Type
        {
            enum Type
            {
                Cancel,
                PoisonSelf,
                PoisonReplace,
                PoisonClean,
                PoisonFortify,
                IsRight = 1 << 7,

                ReplaceRight = PoisonReplace | IsRight,
                CleanRight = PoisonClean | IsRight,
                FortifyRight = PoisonFortify | IsRight,
            };
        };
    }
    using SecondaryType = detail::SecondaryType__Type::Type;

    //I might want more results. Mainly for things like knowing not to reapply a menu update every time I spam poison my weapons.
    enum struct PoisonResult
    {
        Success,
        NoWeapon,
        InvalidWeapon,
        InvalidSelf,
        InvalidAction,  //Used mainly to display that an auto equip didn't fail, just wasn't allowed.
    };

    template <class T>requires(std::is_enum_v<T>)
        inline constexpr T& operator|=(T& a, T b)
    {
        return (T&)((int&)a |= (int)b);
    }

    using MsgCallback = void(int);


    template <std::same_as<const char*>... T>
    void CreateMessage(const char* a_message, MsgCallback* a_callback, std::uint32_t a_arg3, std::uint32_t a_arg4, std::uint32_t a_arg5, T... argz)
    {
        //Args seems to be a va_list of strings ended with a null string (I think?), 
        // arg3 seems to be the value of the first index, 
        // callback is actually a void function that takes an int as a parameter
        // arg 4 and arg 5 are still unknown

        using func_t = decltype(&CreateMessage<T...>);
        REL::Relocation<func_t> func{ RELOCATION_ID(51420, 52269) };
        return func(a_message, a_callback, a_arg3, a_arg4, a_arg5, argz...);
    }

    template <std::convertible_to<const char*>... T>
    void TEST_CreateMessage(const char* a_message, MsgCallback* a_callback, std::uint32_t a_arg3, std::uint32_t a_arg4, std::uint32_t a_arg5, T... argz)
    {
        return CreateMessage(a_message, a_callback, a_arg3, a_arg4, a_arg5, static_cast<const char*>(argz)...);
    }



    bool temp_poisonSelf = true;

    int temp_autoPoisonHand = 0;
    //bool temp_poisonFists = false;
    bool temp_autoIncreaseDose = false;
    bool temp_autoReplaceDose = false;

    //This is to be noted that it's SPECIFICALLY for inventory. For the process of overdosing, I'll need something else.
    //This should be event registered.
	struct PoisonHandler
	{
		//sPoisonNoWeaponMessage
		//inline static RE::Setting* g_noWeaponMessage = nullptr;
		//inline static RE::Setting* g_noWeaponMessage = nullptr;
		//Instead of directly using this, I think I can probably just override the function that calls this,
		// straight up.
		

        static void _HandleCallback(PrimaryType type)
        {
            //ApplyPoisonCallback


        }
        

        template <void(*Callback)(PrimaryType), PrimaryType... Choices>
        static void _CallbackWrapper(int i)
        {
            std::array<PrimaryType, sizeof...(Choices)> choiceArray{ Choices... };

            Callback(choiceArray[i]);
        }


        template <class Enum, void(*Callback)(Enum), Enum... Choices>
        static void _CallbackWrapper2(int i)
        {
            std::array<Enum, sizeof...(Choices)> choiceArray{ Choices... };

            Callback(choiceArray[i]);
        }

        
        //The only reason why these should fail is if they have no viable target
		

        static PoisonResult temp_Drink(RE::PlayerCharacter* player)
        {
            //hand->GetPoison
            //At this point, only choice impacts
            RE::AlchemyItem*& pending_poison = player->GetInfoRuntimeData().pendingPoison;

            //TODO: invalid weapon isn't applicable here. The end result is the targeted consumable doesn't exist anymore.
            if (HasEntry(player, pending_poison, equippedList) == false)
                return PoisonResult::InvalidWeapon;
            logger::debug("a");

            uint16_t special_id = 0;
            RE::FormID form_id = 0;
            RE::FormID ref_id = 0;

            RE::ObjectRefHandle ref_handle{};
            
            if (equippedList)
            {
                GetUniqueIDs(equippedList, special_id, form_id);
                GetReferenceHandle(equippedList, ref_handle);
            }
            
            auto poison_ref = ref_handle.get();
            
            if (poison_ref) {
                ref_id = poison_ref->formID;
            }
            
            //RE::ActorHandle player_handle = player->CreateRefHandle();
            //auto player_nipointer = player_handle.get();

            RE::TESObjectREFR* ref = player;

            auto player_handle = ref->CreateRefHandle();

            
            auto player_nipointer = player_handle.get();

            
            auto source_holder = RE::ScriptEventSourceHolder::GetSingleton();
            
            
            SendEquipEvent(source_holder, player_nipointer, pending_poison, special_id, ref_id, true);
            
            if (player->DrinkPotion(pending_poison, equippedList) == true) {
                RE::DebugNotification("You've poisoned yourself you dingus.");
                //Play poisoned sound?
                player->PlayPickUpSound(pending_poison, true, true);
            
                SendActorItemEquippedEvent(pending_poison, player);
            

            }

            ClearPoison(player);

            return PoisonResult::Success;
        }

        //I think I can make this a lambda
        static void _SecondaryCallback(SecondaryType type)
        {
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

            //ApplyPoisonCallback(2, is_left);//This isn't handled here

            bool is_left = (type & SecondaryType::IsRight) == false;

            type = (SecondaryType)(type & ~SecondaryType::IsRight);

            if (type != SecondaryType::Cancel)
            {

                //
                RE::AlchemyItem*& pending_poison = player->GetInfoRuntimeData().pendingPoison;

                RE::InventoryEntryData* hand = type != SecondaryType::PoisonSelf ? player->GetEquippedEntryData(is_left) : nullptr;

                RE::ExtraPoison* extra_poison = hand ? GetExtraPoison(hand) : nullptr;

                //All the shit below needs to be a function that should be called.
                switch (type)
                {
                case SecondaryType::PoisonClean:
                {
                    _RemovePoison(hand);
                }

                case SecondaryType::PoisonReplace:
                {
                    if (auto* list = hand->extraLists->front(); list->Remove(RE::ExtraDataType::kPoison, extra_poison) == true)
                        ApplyPoisonCallback(2, is_left);

                    break;
                }

                case SecondaryType::PoisonFortify:
                {

                    if (!extra_poison)
                        break;

                    float out = 1;
                    //Should need both entry points.
                    //TODO:FIX THE FUCKING STACK OVERFLOW WITH THE FUCKING ENTRY POINTS ALREADY YOU DUMB SHIT CUNT BITCH BEFORE I MURDER YOU
                    //RE::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, out, "POISONING", hand->object, pending_poison);
                    RE::BGSEntryPoint::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, hand->object, pending_poison, &out);


                    uint32_t dosage = fmax(1, out);

                    uint32_t res = dosage;

                    if (dosage != 1)
                    {
                        //second perk entry use here.

                        RE::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, out, "PAID", hand->object, pending_poison);

                        float perk_mult = fmax(1, out);



                        float dose_num = fmax(extra_poison->count / dosage, 1);

                        float reapply_mult = dosage > 1 ? std::pow(SettingManager::reapplyDoseMult, dose_num) : SettingManager::reapplyDoseMult;

                        res = fmax(dosage * perk_mult * reapply_mult, 1);
;
                        //Increase dose
                    }

                    extra_poison->count += res;
                    
                    //This retreads ground, but it doesn't actually increment anything if extra poison is present. So I'm gonna just let this do
                    // whatever the fuck needs to be done.
                    ApplyPoisonCallback(2, is_left);

                }

                case SecondaryType::PoisonSelf:
                    temp_Drink(player);
                    break;
                }
            }

            ClearPoison();
        }



        static PoisonResult _HandleSecondary_REDONE(RE::PlayerCharacter* player, RE::InventoryEntryData* hand)
        {
            //HandleSecondary needs a handle final, something other functions can use too

            //hand->GetPoison
            //At this point, only choice impacts

            bool requires_callback = false;

            SecondaryType type{};

            PoisonResult result = PoisonResult::Success;

            RE::AlchemyItem*& pending_poison = player->GetInfoRuntimeData().pendingPoison;

            RE::ExtraPoison* extra_poison = nullptr;

            if (!hand)
            {
                if (SettingManager::drinkPrompt)
                    requires_callback = true;

                type = SecondaryType::PoisonSelf;
            }
            else if (int worn = GetWorn(hand); worn)
            {
                bool is_left = worn == -1;

                extra_poison = GetExtraPoison(hand);

                if (!extra_poison)
                {
                    ApplyPoisonCallback(2, is_left);
                }
                else
                {
                    //Asks questions possibly. For now, it assumes.
                    if (extra_poison->poison == pending_poison)
                    {
                        if (SettingManager::fortifyPrompt)
                            requires_callback = true;

                        type = SecondaryType::PoisonFortify;
                    }
                    else
                    {

                        if (SettingManager::replacePrompt)
                            requires_callback = true;

                        type = SecondaryType::PoisonReplace;
                    }
                
                    if (!is_left)
                        type = (SecondaryType)(type | SecondaryType::IsRight);
                }


            }


            if (type == SecondaryType::Cancel)
                ClearPoison(player);
            else
            {
                if (!requires_callback)
                {
                    _SecondaryCallback(type);
                }
                else
                {
                    std::string message{};

                    void(*callback)(int) = nullptr;
                    auto d = _CallbackWrapper2<SecondaryType, nullptr, SecondaryType::PoisonSelf, SecondaryType::Cancel>;

                    RE::Setting* sYes = RE::GameSettingCollection::GetSingleton()->GetSetting("sYes");
                    RE::Setting* sCancel = RE::GameSettingCollection::GetSingleton()->GetSetting("sCancel");

                    constexpr auto kFortAndRight = SecondaryType::PoisonFortify | SecondaryType::IsRight;
                    constexpr auto kReplaceAndRight = SecondaryType::PoisonReplace | SecondaryType::IsRight;

                    switch (type)
                    {
                    case SecondaryType::PoisonSelf:
                        message = std::vformat(SettingManager::confirmPoisonSelf, std::make_format_args(pending_poison->GetName()));
                        callback = _CallbackWrapper2<SecondaryType, _SecondaryCallback, SecondaryType::PoisonSelf, SecondaryType::Cancel>;
                        TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::Yes, SettingManager::Cancel, nullptr);
                        break;


                    case SecondaryType::PoisonFortify:
                        message = std::vformat(SettingManager::confirmPoisonFortify, std::make_format_args(pending_poison->GetName(), hand->GetDisplayName()));
                        callback = _CallbackWrapper2<SecondaryType, _SecondaryCallback, SecondaryType::PoisonFortify, SecondaryType::Cancel>;
                        TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::Yes, SettingManager::Cancel, nullptr);
                        break;
                    case SecondaryType::PoisonReplace:
                        message = std::vformat(SettingManager::confirmPoisonReplace, std::make_format_args(extra_poison->poison->GetName(), pending_poison->GetName(), hand->GetDisplayName()));
                        callback = _CallbackWrapper2<SecondaryType, _SecondaryCallback, SecondaryType::PoisonReplace, SecondaryType::Cancel>;
                        TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::Yes, SettingManager::Cancel, nullptr);
                        break;
                    case SecondaryType::PoisonFortify | SecondaryType::IsRight:
                        message = std::vformat(SettingManager::confirmPoisonFortify, std::make_format_args(pending_poison->GetName(), hand->GetDisplayName()));
                        callback = _CallbackWrapper2<SecondaryType, _SecondaryCallback, SecondaryType::FortifyRight, SecondaryType::Cancel>;
                        TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::Yes, SettingManager::Cancel, nullptr);
                        break;
                    case SecondaryType::PoisonReplace | SecondaryType::IsRight:
                        message = std::vformat(SettingManager::confirmPoisonReplace, std::make_format_args(extra_poison->poison->GetName(), pending_poison->GetName(), hand->GetDisplayName()));
                        callback = _CallbackWrapper2<SecondaryType, _SecondaryCallback, SecondaryType::ReplaceRight, SecondaryType::Cancel>;
                        TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::Yes, SettingManager::Cancel, nullptr);
                        break;

                    }
                }
            }


            return result;
        }

        
        static PoisonResult _HandleSecondary(RE::PlayerCharacter* player, RE::InventoryEntryData* hand, bool is_left_ = false)
        {
            //HandleSecondary needs a handle final, something other functions can use too

            //hand->GetPoison
            //At this point, only choice impacts

            PoisonResult result = PoisonResult::Success;

            if (!hand)
            {
                result = temp_Drink(player);
            }
            else if (int worn = GetWorn(hand); worn)
            {
                bool is_left = worn == -1;

                RE::AlchemyItem*& pending_poison = player->GetInfoRuntimeData().pendingPoison;

                RE::ExtraPoison* extra_poison = GetExtraPoison(hand);

                if (!extra_poison)
                {
                    ApplyPoisonCallback(2, is_left);
                }
                else
                {

                    //Asks questions possibly. For now, it assumes.
                    if (extra_poison->poison == pending_poison)
                    {
                        float out = 1;

                        RE::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, out, "PAID", hand->object, pending_poison);

                        //RE::BGSEntryPoint::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, hand->object, pending_poison, &out);

                        uint32_t dosage = fmin(1, out);

                        //if dosage is 1, none of this is required.

                        //Increase dose
                        extra_poison->count += dosage;
                        //This retreads ground, but it doesn't actually increment anything if extra poison is present. So I'm gonna just let this do
                        // whatever the fuck needs to be done.
                        ApplyPoisonCallback(2, is_left);
                        /*
                        RE::BGSCreatedObjectManager* obj_manager = RE::BGSCreatedObjectManager::GetSingleton();

                        //RE::RefHandle

                        //This removes when called too early, but not when called when it's supposed to be. WHY?
                        player->RemoveItem(pending_poison, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr, nullptr);

                        UpdateRefInventory_Query(player, 0);

                        UnkType* unk = GetUnk();
                        UseUnk(unk, pending_poison)

                        if (pending_poison->formID >= 0xFF000000)
                            DecrementPersistentRef(obj_manager, pending_poison);
                        //*/
                    }
                    else
                    {
                        if (auto* list = hand->extraLists->front(); list->Remove(RE::ExtraDataType::kPoison, extra_poison) == true)
                        {
                            ApplyPoisonCallback(2, is_left);
                        }
                    }
                }
            }

            ClearPoison(player);


            return result;
        }


        
        static PoisonResult _CheckHand(RE::PlayerCharacter* player, RE::InventoryEntryData* hand)
        {
            if (!hand)
            {
                //Is supposed run a perk entry point on the target.
                float out = SettingManager::drinkPoisonValue;
                
                
                //std::string tag = "PAID";
                if (auto poison = player->GetInfoRuntimeData().pendingPoison; poison)
                    //This always crashes. I'm not completely sure why, but if I could hazard a guess, maybe it's a threading thing?
                    RE::HandleEntryPoint(RE::PerkEntryPoint::kCanDualCastSpell, player, out, "PAID", poison);
                    //RE::HandleEntryPoint(RE::PerkEntryPoint::kModArmorRating, player, &out, tag, 255, { poison });
                    //RE::HandleEntryPoint(RE::PerkEntryPoint::kModArmorRating, player, out, "PAID", poison);
                
                if (out <= 0)
                    return PoisonResult::InvalidSelf;
            }
            else
            {
                if (!hand || !hand->object || hand->object->formType != RE::FormType::Weapon)
                    return PoisonResult::NoWeapon;

                RE::TESObjectWEAP* weapon = hand->object->As<RE::TESObjectWEAP>();

                if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee)
                    return PoisonResult::InvalidWeapon;
            }

            return  PoisonResult::Success; 
        }

        //Used by core handle, not primary.
        static PoisonResult _HandleHand(RE::PlayerCharacter* player, PrimaryType type)
        {
            //Should include a third option, perhaps nullable?

            RE::InventoryEntryData* hand = nullptr;

            bool allow_primary = false;

            

            switch (type)
            {
            case PrimaryType::PoisonSelf:
                //Remains none.
                if (SettingManager::selectFailType == 1)
                    allow_primary = true;
                break;

            case PrimaryType::PoisonRight:
                if (type == PrimaryType::PoisonRight && SettingManager::rightFailType == 1)
                    allow_primary = true;


            case PrimaryType::PoisonLeft:
                if (type == PrimaryType::PoisonLeft && SettingManager::rightFailType == 1)
                    allow_primary = true;

                hand = player->GetEquippedEntryData(type == PrimaryType::PoisonLeft);
                
                if (!hand)
                    return allow_primary ? PoisonResult::InvalidAction : PoisonResult::NoWeapon;

                break;

            default:
                return PoisonResult::InvalidWeapon;
            }

            //RE::InventoryEntryData* hand = player->GetEquippedEntryData(is_left);

            if (auto result = _CheckHand(player, hand); result != PoisonResult::Success)
                return allow_primary ? PoisonResult::InvalidAction : result;

            //This can just return success I think
            return _HandleSecondary_REDONE(player, hand);
        }



        static void _PrimaryCallback(PrimaryType type)
        {
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

            switch (type)
            {
            case PrimaryType::PoisonSelf:
                //Primary callback uses this directly because it doesn't seek to hit the callback for self poisoning;
                temp_Drink(player);

                break;

            case PrimaryType::PoisonRight:
                //ApplyPoisonCallback(2, false);
                _HandleSecondary_REDONE(player, player->GetEquippedEntryData(false));
                break;


            case PrimaryType::PoisonLeft:
                //ApplyPoisonCallback(2, true);
                _HandleSecondary_REDONE(player, player->GetEquippedEntryData(true));
                break;


                //Clearly, nothing for cancel.
            }
        }


        static PoisonResult _HandleAuto(RE::PlayerCharacter* player, const RE::BGSEquipSlot* slot)
        {
            PoisonResult result = PoisonResult::InvalidAction;

            if (slot)
            {
                if (slot->formID == 0x00013F42)//RightHand
                {
                    if (!SettingManager::rightEquipType)
                        return PoisonResult::InvalidAction;


                    result = _HandleHand(player, PrimaryType::PoisonRight);
                }
                else if (slot->formID == 0x00013F43)//LeftHand
                {
                    if (!SettingManager::rightEquipType)
                        return PoisonResult::InvalidAction;

                    result = _HandleHand(player, PrimaryType::PoisonLeft);
                }
                //This shouldn't happen, but i guess no weapon.
            }
            //Confirm button

            //Instead of this, I can check the amount of item menus currently open right now.
            
            //else if (auto* ui = RE::UI::GetSingleton(); ui->IsMenuOpen("FavoritesMenu") || ui->IsMenuOpen("InventoryMenu") || ui->IsMenuOpen("ContainerMenu"))
            else if (auto* ui = RE::UI::GetSingleton(); ui->IsMenuOpen("FavoritesMenu")|| ui->IsItemMenuOpen() == true)
            {
                //see if favorites menu can be in there.
                if (!SettingManager::itemSelectType)
                    return PoisonResult::InvalidAction;

                result = _HandleHand(player, PrimaryType::PoisonSelf);
            }

            return result;
        }

        static PoisonResult _HandlePrimary(RE::PlayerCharacter* player)
        {
            RE::InventoryEntryData* right_hand = player->GetEquippedEntryData(false);
            RE::InventoryEntryData* left_hand = player->GetEquippedEntryData(true);

            PrimaryType options = PrimaryType::Cancel;

            PoisonResult result = PoisonResult::Success;

            if (auto temp = _CheckHand(player, nullptr); temp != PoisonResult::Success) {
                result = temp;
            }
            else {
                options |= PrimaryType::PoisonSelf;
            }

            if (right_hand)
                if (auto temp = _CheckHand(player, right_hand);  temp != PoisonResult::Success) {
                    result = temp;
                }
                else {
                    options |= PrimaryType::PoisonRight;
                }

            if (left_hand)
                if (auto temp = _CheckHand(player, left_hand); temp != PoisonResult::Success) {
                    if (result == PoisonResult::Success)//This might function weird
                        result = temp;
                }
                else {
                    options |= PrimaryType::PoisonLeft;
                }
                


            if (options == PrimaryType::Cancel)
                return result;

            std::string message;

            //RE::Setting* g_sPoisonConfirmMessage = nullptr;
            //RE::Setting* g_sOr = nullptr;
            //RE::Setting* g_sOr = nullptr;

            void(*callback)(int)  = nullptr;
            

            //Cancel exists.

            RE::AlchemyItem* poison = player->GetInfoRuntimeData().pendingPoison;

            //MAYBE, should check for std::format_error just in case someone used a bad option.

            //I'd rather have a single function doing the formatting here, this function has become long enough.
            switch (options)
            {
            case PrimaryType::PoisonSelf:
                message = std::vformat(SettingManager::poisonSelf, std::make_format_args( poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonSelf, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonDrink, SettingManager::Cancel, nullptr);
                break;


            case PrimaryType::PoisonSelf | PrimaryType::PoisonRight:
                message = std::vformat(SettingManager::poisonSelfOrHand, std::make_format_args(right_hand->GetDisplayName(), poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonSelf, PrimaryType::PoisonRight, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonDrink, SettingManager::poisonRight, SettingManager::Cancel, nullptr);
                break;


            case PrimaryType::PoisonSelf | PrimaryType::PoisonLeft:
                message = std::vformat(SettingManager::poisonSelfOrHand, std::make_format_args(left_hand->GetDisplayName(), poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonSelf, PrimaryType::PoisonLeft, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonDrink, SettingManager::poisonLeft, SettingManager::Cancel, nullptr);
                break;


            case PrimaryType::PoisonSelf | PrimaryType::PoisonRight | PrimaryType::PoisonLeft:
                message = std::vformat(SettingManager::poisonSelfOrBothHands, std::make_format_args(right_hand->GetDisplayName(), left_hand->GetDisplayName(), poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonSelf, PrimaryType::PoisonRight, PrimaryType::PoisonLeft, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonDrink, SettingManager::poisonRight, SettingManager::poisonLeft, SettingManager::Cancel, nullptr);
                break;


            case PrimaryType::PoisonRight:
                message = std::vformat(SettingManager::poisonHand, std::make_format_args(right_hand->GetDisplayName(), poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonRight, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonRight, SettingManager::Cancel, nullptr);
                break;


            case PrimaryType::PoisonLeft:
                message = std::vformat(SettingManager::poisonHand, std::make_format_args(left_hand->GetDisplayName(), poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonLeft, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonLeft, SettingManager::Cancel, nullptr);
                break;


            case PrimaryType::PoisonRight | PrimaryType::PoisonLeft:
                message = std::vformat(SettingManager::poisonBothHands, std::make_format_args(right_hand->GetDisplayName(), left_hand->GetDisplayName(), poison->GetName()));
                callback = _CallbackWrapper<_PrimaryCallback, PrimaryType::PoisonRight, PrimaryType::PoisonLeft, PrimaryType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, SettingManager::poisonRight, SettingManager::poisonLeft, SettingManager::Cancel, nullptr);
                break;


            default: 
                //Should return whatever left or right returns
                return PoisonResult::NoWeapon;
            }

            return PoisonResult::Success;
        }

        //This has to be validated when used checking for equips.
        inline static RE::ExtraDataList* equippedList = nullptr;

        static void ClearPoison(RE::PlayerCharacter* player = nullptr)
        {
            if (!player)
                player = RE::PlayerCharacter::GetSingleton();


            player->GetInfoRuntimeData().pendingPoison = nullptr;
            equippedList = nullptr;

            loaded = false;
        }

        inline static bool loaded = false;

        static void SetPoison(RE::AlchemyItem* poison, RE::ExtraDataList* list, RE::PlayerCharacter* player = nullptr)
        {
            if (!player)
                player = RE::PlayerCharacter::GetSingleton();

            loaded = true;

            player->GetInfoRuntimeData().pendingPoison = poison;
            equippedList = list;
        }

    

		//I'm willing to have this function be a straight up rewrite.
		static void Handle(RE::PlayerCharacter* player, RE::AlchemyItem* poison, RE::ObjectEquipParams* equip_params)
		{
            logger::info("{:X} ?", (uintptr_t)equip_params->unk18);

            if (!poison)
                return;

            

            //player->GetInfoRuntimeData().pendingPoison = poison;
            SetPoison(poison, equip_params->extraDataList, player);
            
            auto result = _HandleAuto(player, equip_params->equipSlot);
            
            if (result == PoisonResult::Success)
                return;
            else if (result == PoisonResult::InvalidAction)
                result = _HandlePrimary(player);
            
            //This need change to account for how I deal with the auto now.

            switch (result)
            {

            case PoisonResult::Success:
                return;

            case PoisonResult::InvalidSelf:
            {
                std::string message = std::vformat(SettingManager::poisonUnableToPoisonSelf, std::make_format_args(poison->GetName()));
                SendPrompt(message.c_str());
                break;
            }


            case PoisonResult::NoWeapon:
                SendPrompt(SettingManager::poisonNoWeaponMessage);
                break;
            
            case PoisonResult::InvalidWeapon:
                SendPrompt(SettingManager::poisonUnableToPoison);
                break;
            
            case PoisonResult::InvalidAction:
                logger::info("Invalid action detected where it shouldn't be");
                break;
            }

            ClearPoison(player);

            return;

            
		}

        using VoidFunc = void();


        static bool _RemovePoison(RE::InventoryEntryData* item)
        {

            if (!item || !item->IsPoisoned() || !item->IsWorn())
                return false;

            auto* list = item->extraLists->front();

            if (!list)
                return false;

            //All this needs to be a function.

            RE::ExtraPoison* extra_poison = list->GetByType<RE::ExtraPoison>();

            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

            //This part should actually require some sort of effect in order to handle the in
            RE::AlchemyItem* poison = extra_poison->poison;

            //player->AddObjectToContainer(poison, nullptr, 1, nullptr);



            RE::BGSCreatedObjectManager* obj_manager = RE::BGSCreatedObjectManager::GetSingleton();


            std::random_device                  rand_dev;
            std::mt19937                        gen(rand_dev());
            std::uniform_int_distribution       dist(0, 99);


            uint32_t count = 0;


            float lose_chance = SettingManager::poisonRemoveChance;

            float dosage = 1;
            
            RE::BGSEntryPoint::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, item->object, poison, &dosage);

            dosage = extra_poison->count / dosage;


            RE::HandleEntryPoint(RE::PerkEntryPoint::kModRecoverArrowChance, player, lose_chance, "PAID");
           



            //This needs to be divided by the dose count.
            //*
            for (auto i = 0; i < dosage; i++)
            {
                float rand = dist(gen);

                logger::debug("{} < {}", rand, lose_chance);

                if (rand < lose_chance)
                {
                    count++;


                    if (poison->formID >= 0xFF000000)
                        IncrementCreatedPoisonRef(obj_manager, poison);

                    lose_chance *= SettingManager::poisonRemoveMult;
                }
            }
            //*/
            //if (poison->formID >= 0xFF000000)
            //    IncrementCreatedPoisonRef(obj_manager, poison);

            if (list->Remove(RE::ExtraDataType::kPoison, extra_poison) == true) {
                if (count) {
                    player->AddObjectToContainer(poison, nullptr, count, nullptr);

                    std::string message = std::vformat(SettingManager::poisonRecoverMessage, std::make_format_args(poison->GetName(), count));
                    RE::DebugNotification(message.c_str());

                }
                else {
                    RE::DebugNotification(SettingManager::poisonRemoveMessage);
                }
            }
            //The above should never not happen, but if it does, perhaps an exception is in order.
            return true;
        }

        static bool _Drop(RE::PlayerCharacter* player, RE::InventoryEntryData* item)
        {
            player->GetCurrent3D();
            return true;

            logger::info("{} thread", std::hash<std::thread::id>{}(std::this_thread::get_id()));


            RE::ExtraDataList* list = nullptr;
            
            
            player->DropObject(item->object, list, 1, nullptr, nullptr);
            //(vftable_PlayerCharacter_0->DropObject_658)(v7, &v11, v6, v10, v8, 0i64, 0i64);
            //sub_140718600(gProcessLists_141F587D0);
            //ItemList::Update_140895C60(*(v1 + 72), gPlayerCharacter_142FC1B78);
            ClearInterfaceEffects_query(RE::ProcessLists::GetSingleton());

            auto inventory_menu = RE::UI::GetSingleton()->GetMenu<RE::InventoryMenu>();
            
            if (inventory_menu)
                inventory_menu->GetRuntimeData().itemList->Update(player);


            return true;
        }

        static bool _AdjustSelectedItem(RE::InventoryEntryData* hand, int worn)
        {
            auto inv_menu = RE::UI::GetSingleton()->GetMenu<RE::InventoryMenu>();

            if (!inv_menu)
                return false;

            RE::ItemList* item_list = inv_menu->GetRuntimeData().itemList;
            

            if (RE::ItemList::Item* item = item_list->GetSelectedItem(); item && GetWorn(item->data.objDesc) == worn)
                return true;

            logger::debug("trigger");

            for (size_t i = 0; i < item_list->items.size(); i++)
            {
                auto& item = item_list->items[i];

                if (item->data.objDesc->object != hand->object)
                    continue;

                if (GetWorn(item->data.objDesc) == worn)
                {
                    RE::GFxValue selectedIndex;

                    selectedIndex.SetNumber(static_cast<double>(i));

                    if (item_list->root.SetMember("selectedIndex", selectedIndex) == true) {
                        return true;
                    }
                    else
                    {
                        logger::info("Fuck");
                        return false;
                    }
                }
            }

            return false;
        }

        //Seperate this into it's own handler section, and account for the fact there may be 2 entries if someone is dual wielding
        // Actually, likely impossible. They can only share a space if their only differences are worn, not if poison is involved
        static int HandleRemovePoison(RE::InventoryEntryData* item, MsgCallback* func)
        {
            static MsgCallback* _func = func;
            
            static int worn = 0;

            MsgCallback* _callback = [](int result)
            {
                //A note on this, as long as remove poison is a feature on something to be called and menu is enabled, always bring up the menu, then
                // make cancel an option.
                if (result != 2) {
                    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

                    RE::InventoryEntryData* hand = player->GetEquippedEntryData(worn == -1);


                    if (result == 1) {
                        _RemovePoison(hand);
                    }
                    else if (result == 0){
                        //This is just in case the selected item shifts it's stupid fucking face.
                        if (_AdjustSelectedItem(hand, worn) == false)
                            return;
                    }
                
                    return _func(result);
                }
                
            };


            worn = GetWorn(item);


            if (!item->IsPoisoned() || !worn)
                return 0;

            //Needs to be option later.
            if (SettingManager::cleanPrompt == 1 && item->IsQuestObject() == false) {
                RE::ExtraPoison* ep = item->extraLists->front()->GetByType<RE::ExtraPoison>();
                if (item->IsQuestObject() == false)
                {
                    std::string message = std::vformat(SettingManager::dropOrCleanPoison, std::make_format_args(ep->poison->GetName()));
                    TEST_CreateMessage(message.c_str(), _callback, 0, 25, 4, SettingManager::drop, SettingManager::clean, SettingManager::Cancel, nullptr);
                }
                else
                {
                    std::string message = std::vformat(SettingManager::cleanPoison, std::make_format_args(ep->poison->GetName()));
                    TEST_CreateMessage(message.c_str(), _callback, 1, 25, 4, SettingManager::clean, SettingManager::Cancel, nullptr);
                }
                
            }
            else
                return _RemovePoison(item);

            return 2;
        }


        
        /*
        //RE::AlchemyItem* 
        struct ReapplyData
        {
            RE::FormID poisonID = 0;
            float timeStamp = -1.f;//Not used for right now.

            
            operator bool() { return poisonID; }

            void Clear()
            {
                poisonID = 0; 
                timeStamp = -1.f;
            }

            ReapplyData() = default;
            ReapplyData(RE::AlchemyItem* p) 
            { 
                poisonID = p ? p->formID : 0;
            }
        };
        inline static ReapplyData reapplyData[2];


        //This gets it's own manager too, needs to store data.
        static void HandleOutOfPoison(RE::PlayerCharacter* player, RE::InventoryEntryData* item)
        {

            RE::ExtraPoison* extra_poison = GetExtraPoison(item);

            if (!extra_poison || extra_poison->count > 1)
                return;
            
            

            RE::AlchemyItem* poison = extra_poison->poison;

            RE::DebugNotification("Your poison has run out.");
            
            RE::PlaySound("ITMPoisonUse");
            
            
            int is_worn = GetWorn(item);

            if (!is_worn)
                return;

            //The below should happen additionally when activate is pressed, plus right or left hand attack.


            //For the amount of time it's here, I'll increment it, then decrement it. Before saving, it will be decremented. Or simply opening a menu will
            // cause it to decrement.
            //return;
            RE::InventoryChanges* changes = player->GetInventoryChanges();
            
            if (changes->GetItemCount(poison) > 0)
            {
                //Should play a notification sound here to note that the poison you've run out of is reapplicable
                reapplyData[is_worn == -1] = ReapplyData(poison);

                //Create reapply data. For these purposes, only the formID is kept. Reservation is then left upto the inventory. I think this saves better,
                // preventing a situation where my mod will permanently increment the reference count
            }
        }

        //Should register for item unequips so that if I do widgets, I can make the disappear when the poison is gone.
        static bool HandleReapply(RE::PlayerCharacter* player, bool is_left)
        {

            if (!player)
                player = RE::PlayerCharacter::GetSingleton();


            //boolean reports if it should prevent the press event or not

            ReapplyData& data = reapplyData[is_left];

            if (!data)
                return false;

            RE::AlchemyItem* poison = RE::TESForm::LookupByID<RE::AlchemyItem>(reapplyData[is_left].poisonID);

            if (!poison) 
            {
                //The poisong running out is cause for blocking, but also cause for notification.
                return true;
            }

            RE::InventoryChanges* changes = player->GetInventoryChanges();

            //RE::ExtraDataList* safe_list = nullptr;

            if (changes->GetItemCount(poison) <= 0)
            {
                //Send fail sound
                return true;
            }
            /*
          

            RE::InventoryEntryData* hand_item = player->GetEquippedEntryData(is_left);

            if (!hand_item) {
                return true;
            }
            SetPoison(poison, nullptr, player);
            //Temp placement, should be handle final
            //TODO: Reapplying should be using the secondary callback, as it has no reason not to.
            if (_HandleSecondary(player, hand_item, is_left) == PoisonResult::Success) {
                //Should have custom sound, and shouldn't apply if a different poison has been applied to your weapons.
                data = ReapplyData();
                RE::DebugNotification("Poison reapplied to something i guess who cares.");
            }
            return true;
        }
        //*/

        friend class ReapplyHandler;
	};

}



//Some notes
/*
remove ITMPoisonUse play sound call. I know what the fuck I'm about to do and that I can do it. So, why not do it myself? That function only exists for the needed
calls to go down, nothing else will be using it.

//*/