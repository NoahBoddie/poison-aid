#pragma once 

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
        using func_t = decltype(&ApplyPoisonCallback);
        REL::Relocation<func_t> func{ RELOCATION_ID(39407, 00000) };
        return func(a1, a2);
    }

    //Unsure if this is what it really is
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

        if (!lists)
            return nullptr;
        auto front = lists->front();

        if (!front)
            return nullptr;

        RE::ExtraPoison* extra = front->GetByType<RE::ExtraPoison>();

        return extra;
    }

    int GetWorn(RE::InventoryEntryData* item)
    {
        auto lists = item->extraLists;

        if (!lists)
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
        struct ChoiceType__Type
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
    using ChoiceType = detail::ChoiceType__Type::Type;

    enum struct PoisonResult
    {
        Success,
        NoWeapon,
        InvalidWeapon
    };

    template <class T>requires(std::is_enum_v<T>)
        inline constexpr T& operator|=(T& a, T b)
    {
        return (T&)((int&)a |= (int)b);
    }

    using MsgCallback = void(int);

    template <std::convertible_to<const char*>... T>
    void TEST_CreateMessage(const char* a_message, MsgCallback* a_callback, std::uint32_t a_arg3, std::uint32_t a_arg4, std::uint32_t a_arg5, T... argz)
    {
        //Args seems to be a va_list of strings ended with a null string (I think?), 
        // arg3 seems to be the value of the first index, 
        // callback is actually a void function that takes an int as a parameter
        // arg 4 and arg 5 are still unknown

        using func_t = decltype(&TEST_CreateMessage<T...>);
        REL::Relocation<func_t> func{ RELOCATION_ID(51420, 52269) };
        return func(a_message, a_callback, a_arg3, a_arg4, a_arg5, argz...);
    }



    bool temp_poisonSelf = true;

    int temp_autoPoisonHand = 0;
    //bool temp_poisonFists = false;
    bool temp_autoIncreaseDose = false;
    bool temp_autoReplaceDose = false;

    //This is to be noted that it's SPECIFICALLY for inventory. For the process of overdosing, I'll need something else.
	struct PoisonHandler
	{
		//sPoisonNoWeaponMessage
		//inline static RE::Setting* g_noWeaponMessage = nullptr;
		//inline static RE::Setting* g_noWeaponMessage = nullptr;
		//Instead of directly using this, I think I can probably just override the function that calls this,
		// straight up.
		

        static void _HandleCallback(ChoiceType type)
        {
            //ApplyPoisonCallback


        }
        

        template <void(*Callback)(ChoiceType), ChoiceType... Choices>
        static void _CallbackWrapper(int i)
        {
            std::array<ChoiceType, sizeof...(Choices)> choiceArray{ Choices... };

            Callback(choiceArray[i]);
        }

        
        //The only reason why these should fail is if they have no viable target
		


        //I will forgo this for now. Because it requires things I kinda don't want to do right now.
        static PoisonResult _HandleSecondary(RE::PlayerCharacter* player, RE::InventoryEntryData* hand, bool is_left)
        {
            //hand->GetPoison
            //At this point, only choice impacts

            RE::ExtraPoison* extra_poison = GetExtraPoison(hand);

            RE::AlchemyItem*& pending_poison = player->GetInfoRuntimeData().pendingPoison;

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

                    RE::BGSEntryPoint::HandleEntryPoint(RE::PerkEntryPoint::kModPoisonDoseCount, player, hand->object, pending_poison, &out);

                    uint32_t dosage = fmin(1, out);


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
                        logger::info("dd");
                        ApplyPoisonCallback(2, is_left);
                    }                    
                }
            }

            pending_poison = nullptr;

            return PoisonResult::Success;
        }


        
        static PoisonResult _CheckHand(RE::PlayerCharacter* player, RE::InventoryEntryData* hand)
        {
            if (!hand || !hand->object || hand->object->formType != RE::FormType::Weapon)
                return PoisonResult::NoWeapon;

            RE::TESObjectWEAP* weapon = hand->object->As<RE::TESObjectWEAP>();

            if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee)
                return PoisonResult::InvalidWeapon;

            return  PoisonResult::Success; 
        }

        //Used by core handle, not primary.
        static PoisonResult _HandleHand(RE::PlayerCharacter* player, bool is_left)
        {
            //Should include a third option, perhaps nullable?

            RE::InventoryEntryData* hand = player->GetEquippedEntryData(is_left);

            if (auto result = _CheckHand(player, hand); result != PoisonResult::Success)
                return result;

            return _HandleSecondary(player, hand, is_left);
        }



        static void _PrimaryCallback(ChoiceType type)
        {
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

            switch (type)
            {
            case ChoiceType::PoisonSelf:
                //Simply cast direct spell apply on self. Pain to write out.
                //Handle secondary should do this too.
                if (player->DrinkPotion(player->GetInfoRuntimeData().pendingPoison, nullptr) == true)
                    RE::DebugNotification("You've poisoned yourself you fool.");

                player->GetInfoRuntimeData().pendingPoison = nullptr;

                break;

            case ChoiceType::PoisonRight:
                //ApplyPoisonCallback(2, false);
                _HandleSecondary(player, player->GetEquippedEntryData(false), false);
                break;


            case ChoiceType::PoisonLeft:
                //ApplyPoisonCallback(2, true);
                _HandleSecondary(player, player->GetEquippedEntryData(true), true);
                break;


                //Clearly, nothing for cancel.
            }
        }


        static PoisonResult _HandlePrimary(RE::PlayerCharacter* player)
        {
            RE::InventoryEntryData* right_hand = player->GetEquippedEntryData(false);
            RE::InventoryEntryData* left_hand = player->GetEquippedEntryData(true);

            ChoiceType options = ChoiceType::Cancel;

            if (temp_poisonSelf)
                options |= ChoiceType::PoisonSelf;

            PoisonResult result = PoisonResult::Success;


            if (auto temp = _CheckHand(player, right_hand); temp != PoisonResult::Success) {
                result = temp;
            }
            else {
                options |= ChoiceType::PoisonRight;
            }

            if (auto temp = _CheckHand(player, left_hand); temp != PoisonResult::Success) {
                if (result == PoisonResult::Success)//This might function weird
                    result = temp;
            }
            else {
                options |= ChoiceType::PoisonLeft;
            }
                


            if (options == ChoiceType::Cancel)
                return result;

            std::string message;

            //These can be constants (settings rather).
            constexpr const char* poison_self_text = "Drink Poison";
            constexpr const char* right_text = "Right Item";
            constexpr const char* left_text = "Left Item";

            constexpr const char* cancel_text = "Cancel";

            //RE::Setting* g_sPoisonConfirmMessage = nullptr;
            //RE::Setting* g_sOr = nullptr;
            //RE::Setting* g_sOr = nullptr;

            constexpr const char* temp_or = "or";
            constexpr const char* temp_the = "the";
            constexpr const char* temp_PrimaryHandChoice = "Do you want to poison the";
            constexpr const char* temp_PrimarySelfChoice = "Do you want to poison yourself";
            void(*callback)(int)  = nullptr;
            
            switch (options)
            {
            case ChoiceType::PoisonSelf:
                message = std::format("{}?", temp_PrimarySelfChoice);
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonSelf, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, poison_self_text, cancel_text, nullptr);
                break;


            case ChoiceType::PoisonSelf | ChoiceType::PoisonRight:
                message = std::format("{} {} {} {}?", temp_PrimarySelfChoice, temp_or, temp_the, right_hand->GetDisplayName());
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonSelf, ChoiceType::PoisonRight, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, poison_self_text, right_text, cancel_text, nullptr);
                break;


            case ChoiceType::PoisonSelf | ChoiceType::PoisonLeft:
                message = std::format("{} {} {} {}?", temp_PrimarySelfChoice, temp_or, temp_the, left_hand->GetDisplayName());
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonSelf, ChoiceType::PoisonLeft, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, poison_self_text, left_text, cancel_text, nullptr);
                break;


            case ChoiceType::PoisonSelf | ChoiceType::PoisonRight | ChoiceType::PoisonLeft:
                message = std::format("{}, {}, {} {} {}?", temp_PrimarySelfChoice, right_hand->GetDisplayName(), temp_or, temp_the, left_hand->GetDisplayName());
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonSelf, ChoiceType::PoisonRight, ChoiceType::PoisonLeft, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, poison_self_text, right_text, left_text, cancel_text, nullptr);
                break;


            case ChoiceType::PoisonRight:
                message = std::format("{} {}?", temp_PrimaryHandChoice, right_hand->GetDisplayName());
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonRight, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, right_text, cancel_text, nullptr);
                break;


            case ChoiceType::PoisonLeft:
                message = std::format("{} {}?", temp_PrimaryHandChoice, left_hand->GetDisplayName());
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonLeft, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, left_text, cancel_text, nullptr);
                break;


            case ChoiceType::PoisonRight | ChoiceType::PoisonLeft:
                message = std::format("{} {} {} {}?", temp_PrimaryHandChoice, temp_or, temp_the, left_hand->GetDisplayName());
                callback = _CallbackWrapper<_PrimaryCallback, ChoiceType::PoisonRight, ChoiceType::PoisonLeft, ChoiceType::Cancel>;
                TEST_CreateMessage(message.c_str(), callback, 0, 25, 4, right_text, left_text, cancel_text, nullptr);
                break;


            default: 
                //Should return whatever left or right returns
                return PoisonResult::NoWeapon;
            }

            return PoisonResult::Success;
        }

		//I'm willing to have this function be a straight up rewrite.
		static void Handle(RE::PlayerCharacter* player, RE::AlchemyItem* poison, const RE::BGSEquipSlot* slot)
		{
            if (!poison)
                return;

            player->GetInfoRuntimeData().pendingPoison = poison;
            //This is firing prematurely for some reason.
            //if (!temp_poisonSelf && temp_autoPoisonHand != 0 && _HandleHand(player, temp_autoPoisonHand <= -1) == PoisonResult::Success)
            //    return;

            switch (_HandlePrimary(player))
            {
            case PoisonResult::Success:
                return;
                

            case PoisonResult::NoWeapon:
                //report
                break;
            case PoisonResult::InvalidWeapon:
                //report
                break;
            }

            return;

            

            /*
            if (a1)
            {
                CurrentlyEquippedWeapon_14067A3E0 = ActorProcess::GetCurrentlyEquippedWeapon_14067A3E0(a2->Process_F0, 0);
                v6 = CurrentlyEquippedWeapon_14067A3E0;
                if (CurrentlyEquippedWeapon_14067A3E0
                    && (LODWORD(v7) = _RTDynamicCast_14134BDB0(
                        *CurrentlyEquippedWeapon_14067A3E0,
                        0i64,
                        &TESBoundObject `RTTI Type Descriptor',
                        & TESObjectWEAP `RTTI Type Descriptor'),
                        v7)
                        && (v8 = *(v7 + 413)) != 0)
                {
                    if (v8 == 8)
                    {
                        v9 = off_141DE8F80[0];
                    }
                    else
                    {
                        if (!ExtraContainerChanges__ItemEntry::GetPoisonItem_1401D69C0(v6))
                        {
                            a2->PendingPoison_968 = a1;
                            v10 = off_141DE5BC0;
                            sub_1401D73C0(v6, a3);
                            f_snprintf_1401423D0(Buffer, 256i64, "%s%s?", v10);
                            sub_1408AB180(Buffer, sub_1406A1E30, 2u, 25, 4, off_141DE5650, off_141DE5668, 0i64);
                            return;
                        }
                        v9 = off_141DE5BA8[0];
                    }
                }
                else
                {
                    v9 = off_141DE5B90;
                }
                sub_1408AB180(v9, sub_1406A1E30, 1u, 25, 4, off_141DE5638, 0i64);
            }
        }
        //*/
		}

        //Seperate this into it's own handler section, and account for the fact there may be 2 entries if someone is dual wielding
        // Actually, likely impossible. They can only share a space if their only differences are worn, not if poison is involved
        static bool HandleRemovePoison(RE::InventoryEntryData* item)
        {
            if (!item->IsPoisoned() || !item->IsWorn() || !item->extraLists)
                return false;

            auto* list = item->extraLists->front();

            if (!list)
                return false;

            RE::ExtraPoison* extra_poison = list->GetByType<RE::ExtraPoison>();

            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

            //This part should actually require some sort of effect in order to handle the in
            RE::AlchemyItem* poison = extra_poison->poison;

            player->AddObjectToContainer(poison, nullptr, 1, nullptr);

            RE::BGSCreatedObjectManager* obj_manager = RE::BGSCreatedObjectManager::GetSingleton();


            if (poison->formID >= 0xFF000000)
                IncrementCreatedPoisonRef(obj_manager, poison);

            if (list->Remove(RE::ExtraDataType::kPoison, extra_poison) == true) {
                RE::DebugNotification("Poison removed.");
            }

            return true;
        }

        //RE::AlchemyItem* 

        //This gets it's own manager too, needs to store data.
        static void HandleOutOfPoison(RE::PlayerCharacter* player, RE::InventoryEntryData* item)
        {
            RE::ExtraPoison* extra_poison = GetExtraPoison(item);

            if (!extra_poison || extra_poison->count > 1)
                return;

            RE::AlchemyItem* poison = extra_poison->poison;

            RE::DebugNotification("Your poison has run out.");
            RE::PlaySound("MAGFailSD");

            

            //For the amount of time it's here, I'll increment it, then decrement it. Before saving, it will be decremented. Or simply opening a menu will
            // cause it to decrement.

            RE::InventoryChanges* changes = player->GetInventoryChanges();

            if (changes->GetItemCount(poison) > 0)
            {
                //Create reapply data. For these purposes, only the formID is kept. Reservation is then left upto the inventory. I think this saves better,
                // preventing a situation where my mod will permanently increment the reference count
            }
        }
	};
}