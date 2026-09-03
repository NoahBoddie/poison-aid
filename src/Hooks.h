
#include "PoisonHandler.h"
#include "ReapplyHandler.h"
#include "xbyak/xbyak.h"
namespace POS
{
	struct Hooks
	{
	//private:
		static int IsCallOrJump(uintptr_t addr)
		{
			//0x15 0xE8//These are calls, represented by negative numbers
			//0x25 0xE9//These are jumps, represented by positive numbers.
			//And zero represent it being neither.

			if (addr)
			{
				auto first_byte = reinterpret_cast<uint8_t*>(addr);

				switch (*first_byte)
				{
				case 0x15:
				case 0xE8:
					return -1;

				case 0x25:
				case 0xE9:
					return 1;

				}
			}

			return 0;
		}
		
		struct ProloguePatch : Xbyak::CodeGenerator
		{
			explicit ProloguePatch(uintptr_t address, uintptr_t length)
			{
				// Hook returns here. Execute the restored bytes and jump back to the original function.
				for (size_t i = 0; i < length; i++)
					db(*reinterpret_cast<uint8_t*>(address + i));

				jmp(ptr[rip]);
				dq(address + length);
			}
		};


		struct PlayerCharacter__PendPoison
		{
			static void Patch()
			{
				//SE: 0x6A1CC0, AE: 0x6DC6C0, VR: ???
				auto hook_addr = REL::RelocationID(39406, 40481).address();

				//auto return_addr = hook_addr + 0x5;
				
				auto& trampoline = SKSE::GetTrampoline();

				struct Code : Xbyak::CodeGenerator
				{
					Code(uintptr_t ret_addr)
					{
						//push(r8);
						mov(r8, rsi);
						mov(rax, ret_addr);
						jmp(rax);
						//pop(r8);
						ret();
					}
				} static code{ (uintptr_t)thunk };

				//auto placed_call = IsCallOrJump(hook_addr) > 0;

				//auto place_query = trampoline.write_branch<5>(hook_addr, (uintptr_t)thunk);
				auto place_query = trampoline.write_branch<5>(hook_addr, (uintptr_t)code.getCode());

				//if (!placed_call)
				//	func = (uintptr_t)code.getCode();
				//else
				//	func = place_query;


				logger::info("PlayerCharacter__PendPoison complete...");
				//*/
			}


			static void thunk(RE::PlayerCharacter* a_this, RE::AlchemyItem* poison, RE::ObjectEquipParams* equip_params)
			{
				return PoisonHandler::Handle(a_this, poison, equip_params);
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};



		struct MenuCallback__ApplyDosage
		{
			static void Install()
			{
				//SE: 6A1E30, AE: 6DC830, VR:???
				auto hook_addr = REL::RelocationID(39407, 40482).address() + 0xB0;

				auto& trampoline = SKSE::GetTrampoline();

				func = trampoline.write_call<5>(hook_addr, thunk);


				logger::info("MenuCallback__ApplyDosage complete...");
			}


			static void thunk(RE::PerkEntryPoint a1, RE::PlayerCharacter* a2, RE::TESObjectWEAP* a3, RE::AlchemyItem* a4, float& out)
			{
				func(a1, a2, a3, a4, out);
				
				if (auto list = PoisonHandler::equippedList) {
					if (auto poison_health = list->GetByType<RE::ExtraHealth>()) {
						out = std::max(1.0f, out * poison_health->health);
					}
				}
			}


			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct MenuCallback__RemoveItemCall
		{
			static void Install()
			{
				//SE: 6A1E30, AE: 6DC830, VR:???
				auto hook_addr = REL::RelocationID(39407, 40482).address() + 0x13F;

				//auto return_addr = hook_addr + 0x5;

				auto& trampoline = SKSE::GetTrampoline();

				trampoline.write_call<6>(hook_addr, thunk);

				//if (!placed_call)
				//	func = (uintptr_t)code.getCode();
				//else
				//	func = place_query;


				logger::info("PlayerCharacter__PendPoison complete...");
				//*/
			}


			static RE::ObjectRefHandle* thunk(RE::PlayerCharacter* a_this,
				RE::ObjectRefHandle* hidden,
				RE::TESBoundObject* a1,
				std::int32_t a2,
				RE::ITEM_REMOVE_REASON a3,
				RE::ExtraDataList* a4,
				RE::TESObjectREFR* a5,
				const RE::NiPoint3* a6,
				const RE::NiPoint3* a7)
			{
				*hidden = a_this->RemoveItem(a1, a2, a3, PoisonHandler::equippedList, a5, a6, a7);
				return hidden;
				//return ;
			}
		};

		struct StandardItemData__DisplayName
		{
			static void Install()
			{
				//889C90+4
				//SE: 889C90, AE: xxxxxx, VR:???
				auto hook_addr = REL::RelocationID(50926, 000000).address() + 0x4;

				//auto return_addr = hook_addr + 0x5;

				auto& trampoline = SKSE::GetTrampoline();

				func = trampoline.write_branch<5>(hook_addr, thunk);

				//if (!placed_call)
				//	func = (uintptr_t)code.getCode();
				//else
				//	func = place_query;


				logger::info("PlayerCharacter__PendPoison complete...");
				//*/
			}

			//This expects a string managed by someone else.
			static const char* thunk(RE::InventoryEntryData* a_this)
			{

				RE::StandardItemData;
				RE::ExtraDataList* list = nullptr;
				RE::ExtraTextDisplayData* display = nullptr;
				float temper = 0;
				float percent = 0;
				
				if (auto object = a_this->GetObject())
				{
					if (auto alch = object->As<RE::AlchemyItem>(); alch && alch->IsPoison())
					{
						if (a_this->extraLists->empty() == false)
						{
							if (list = a_this->extraLists->front())
							{
								if (auto poisonHealth = list->GetByType<RE::ExtraHealth>())
								{
									if (display = list->GetByType<RE::ExtraTextDisplayData>()){
										temper = display->temperFactor;
									}

									percent = poisonHealth->health;
								}
							}
						}
					}
				}

				auto result = func(a_this);


				//This needs to be update after, but this also creates the extra text. So pickle. And annoying.
				//*

				if (percent && list) {
					bool existed = display;
					if (!display) {
						display = list->GetByType<RE::ExtraTextDisplayData>();
					}

					if (display && (!existed || temper != percent)) {

						char buffer[288];
						std::sprintf(buffer, "%s %i%%", result, (int)std::floor(percent * 100));//Make this a setting
						display->displayName = buffer;
						result = display->displayName.c_str();
					}
				}

				//*/


				return result;
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};


		struct ItemDrop_Hook
		{
			//
			//Note, hook is actually for SetBaseActorValue

			static void Patch()
			{
				//SE: 0x88E280, AE: 0x8CFDC0, VR: ???
				auto hook_addr = REL::RelocationID(50978, 51857).address();

				auto return_addr = hook_addr + RELOCATION_OFFSET(0x5, 0x7);
				//*
				struct Code : Xbyak::CodeGenerator
				{
					Code(uintptr_t ret_addr)
					{
						push(rbx);
						push(rbp);

						if (REL::Module::GetRuntime() == REL::Module::Runtime::AE)
						{
							sub(rsp, 0x48);
						}
						else
						{
							push(r14);
						}

						mov(rax, ret_addr);
						jmp(rax);
					}
				} static code{ return_addr };

				auto& trampoline = SKSE::GetTrampoline();


				auto placed_call = IsCallOrJump(hook_addr) > 0;

				auto place_query = trampoline.write_branch<5>(hook_addr, (uintptr_t)thunk);

				if (!placed_call)
					func = (uintptr_t)code.getCode();
				else
					func = place_query;


				logger::info("TESConditionItem__IsTrue complete...");
				//*/
			}
			
			
			static void thunk(uint64_t unk1)
			{
				//The menu callback will require this
				static void(*callback)(int) = [](int result)
				{
					//These structs are used in the game delegate callbacks, I just have no fucking idea what's in them, nor do I care.
					// This is all the function I'm hooking uses, so that's all I'll be providing.
					struct UnkB
					{
						//The use of the double and this coming straight from game delegate leads me to believe that this is 
						// a GFxValue.
						uint64_t paddingA[2]{};				//000
						double count = 1.f;					//016
					};
					static_assert(offsetof(UnkB, count) == 16);

					struct UnkA
					{
						uint64_t paddingA[3]{};					//000
						RE::InventoryMenu* menu = nullptr;		//024
						uint64_t paddingB{};					//032
						UnkB* unk = nullptr;					//040	

						operator uint64_t() { return reinterpret_cast<uintptr_t>(this); }
						UnkA(RE::InventoryMenu* m, UnkB& b) : menu{ m }, unk{ &b } {}
					};
					static_assert(offsetof(UnkA, menu) == 24);
					static_assert(offsetof(UnkA, unk) == 40);

					//This is actually "FxDelegateArgs"
					
					if (result == 2)
						return;

					

					//RE::InventoryMenu* menu = *reinterpret_cast<RE::InventoryMenu**>(g_unkPtr + 24);
					auto inventory_menu = RE::UI::GetSingleton()->GetMenu<RE::InventoryMenu>();

					if (!inventory_menu)
						return;
						//	inventory_menu->GetRuntimeData().itemList->Update(player);

					if (result == 1) {
						if (auto* item_list = inventory_menu->GetRuntimeData().itemList; item_list)
							item_list->Update(RE::PlayerCharacter::GetSingleton());
					}
					else 
					{
						constexpr bool use_real_type = true;


						if (use_real_type)
						{

							RE::GFxValue gfx_count{ 1.0 };

							RE::FxDelegateArgs args{ RE::GFxValue{}, inventory_menu.get(), nullptr, &gfx_count, 1 };

							func(reinterpret_cast<uintptr_t>(&args));
						}
						else
						{
							UnkB unk_b{};
							UnkA unk_a{ inventory_menu.get(), unk_b };

							func(unk_a);
						}
						

						
					}
				};

				RE::InventoryMenu* menu = *reinterpret_cast<RE::InventoryMenu**>(unk1 + 24);
				double count = *reinterpret_cast<double*>(*reinterpret_cast<uint64_t*>(unk1 + 40) + 16);

				if (count != 1.f)//There shouldn't be multiples of this to drop. Just an extra check.
					return func(unk1);

				auto* item = menu->GetRuntimeData().itemList->GetSelectedItem();

				if (!item)
					return func(unk1);

				RE::InventoryEntryData* obj = item->data.objDesc;

				//Used to not call the original.
				if (!obj)
					return func(unk1);

				auto result = PoisonHandler::HandleRemovePoison(obj, callback);

				if (result == 1) {
					menu->GetRuntimeData().itemList->Update(RE::PlayerCharacter::GetSingleton());
					return;
				}
				else if (result == 0) {
					return func(unk1);
				}
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};


		struct DecrementPoison_Hook
		{
			//
			//Note, hook is actually for SetBaseActorValue
			
			//Do the hook a little higher, on get poison item. the second variable will be for the actor.
			//The 2 calls will be pretty similarly incompatible but similar. Unfortunately, with this there's no way for compatibility stuff like 
			// infinite poison, checking a few instructions after if the poison is still had.

			//The apply will need the rbx register, while the other will need the rsi register.


			static void Patch1()
			{
				//Arrow patch
				
				struct Code : Xbyak::CodeGenerator
				{
					Code(uintptr_t ret_addr)
					{
						if (REL::Module::IsAE() == true)
							mov(rdx, rbp);
						else
							mov(rdx, rsi);

						mov(rax, ret_addr);
						jmp(rax);
					}
				} static code{ (uintptr_t)thunk<0> };


				//SE: 0x7220B0+0x11A, AE: 0x75F320+0x11F, VR: ???
				REL::Relocation<uintptr_t> hook{ REL::RelocationID(41778, 42859), REL::VariantOffset(0x11A, 0x11F, 0x11A) };


				auto& trampoline = SKSE::GetTrampoline();


				func[0] = trampoline.write_call<5>(hook.address(), (uintptr_t)code.getCode());
			}

			static void Patch2()
			{
				//Melee Patch
				
				struct Code : Xbyak::CodeGenerator
				{
					Code(uintptr_t ret_addr)
					{
						mov(rdx, rbx);
						mov(rax, ret_addr);
						jmp(rax);
					}
				} static code{ (uintptr_t)thunk<1> };

				//SE: 0x6310A0+0x148, AE: 0x6691F0+0x153, VR: ???
				REL::Relocation<uintptr_t> hook{ REL::RelocationID(37799, 38748), REL::VariantOffset(0x148, 0x153, 0x148) };


				auto& trampoline = SKSE::GetTrampoline();


				func[1] = trampoline.write_call<5>(hook.address(), (uintptr_t)code.getCode());
			}


			static void Patch()
			{
				Patch1();
				Patch2();

				logger::info("TESConditionItem__IsTrue complete...");
				//*/
			}


			template <int I = 0>
			static RE::AlchemyItem* thunk(RE::InventoryEntryData* item, RE::Character* aggressor)
			{
				RE::AlchemyItem* poison = func[I](item);

				//logger::info("{}/{} player, {} poison", aggressor->IsPlayerRef(), aggressor != nullptr, extra_poison != nullptr);
				if (aggressor && aggressor->IsPlayerRef() && poison)
				{
					
					ReapplyHandler::HandleOutOfPoison(skyrim_cast<RE::PlayerCharacter*>(aggressor), item);
				}

				return poison;
			}

			static inline REL::Relocation<RE::AlchemyItem*(RE::InventoryEntryData*)> func[2];
		};


		struct AttackBlockHandler_Hook
		{
			//
			//Note, hook is actually for SetBaseActorValue

			static void Patch()
			{
				REL::Relocation<uintptr_t> vtable{ RE::VTABLE_AttackBlockHandler[0] };

				func = vtable.write_vfunc(0x4, thunk);
			}
			
			static void thunk(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
			{
				if (auto activate_handle = RE::PlayerControls::GetSingleton()->GetActivateHandler();a_event->IsDown() == true && activate_handle->heldStateActive)
				{
					bool is_left = a_event->QUserEvent() == RE::UserEvents::GetSingleton()->leftAttack;

					if (ReapplyHandler::HandleReapply(nullptr, is_left) == true) {
						return;
					}
				}

				return func(a_this, a_event, a_data);
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};

		class MainUpdateHook
		{
		public:
			static void Patch()
			{
				//Your hook. . . I'll take it. Flagrantly taken from Ersh
				auto& trampoline = SKSE::GetTrampoline();
				REL::Relocation<uintptr_t> hook{ RELOCATION_ID(35565, 36564) };  // 5B2FF0, 5D9F50, main update

				func = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x748, 0xC26), thunk);  // 5B3738, 5DAB76
			}

		private:
			static void thunk()
			{
				func();

				//static float globalTime = 0;

				//float app_time = RE::GetDurationOfApplicationRunTime() * 0.001f;

				//float update_time = app_time - globalTime;
				
				//globalTime = app_time;

				float update_time = RE::GetSecondsSinceLastFrame();

				ReapplyHandler::Update(update_time);
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};

		//Wanna hook a null sub on the main update, 1405B2FF0+748 on 1.5.97. This way I can check real time difference and disregard it if
		// the game is currently paused or something.

		struct PoisonBlameHook
		{
			static void Install()
			{
				//Needs a beginning hook
				//SE: 567A80, AE: xxx, VR: ???
				auto hook = REL::RelocationID(34286, 0).address();
				uintptr_t offset = 0x9;


				struct Patch : Xbyak::CodeGenerator
				{
					explicit Patch(uintptr_t address, uintptr_t length)
					{
						// Hook returns here. Execute the restored bytes and jump back to the original function.
						for (size_t i = 0; i < length; i++)
							db(*reinterpret_cast<uint8_t*>(address + i));

						jmp(ptr[rip]);
						dq(address + length);
					}
				} static code{ hook, offset };



				auto& trampoline = SKSE::GetTrampoline();

				auto placed_call = IsCallOrJump(hook) > 0;

				auto place_query = trampoline.write_branch<5>(hook, (uintptr_t)thunk);

				if (!placed_call)
					func = (uintptr_t)code.getCode();
				else
					func = place_query;

			}


			static void thunk(RE::ValueModifierEffect* a_this, RE::Actor* target, float value, RE::ActorValue av)
			{
				bool should = false;
				bool disqualify = false;
				//So this will need a value to say

				auto middle = target->GetMiddleHighProcess();

				//So it will be viable if 

				auto group = target->GetCombatGroup();

				RE::Actor* caster = nullptr;

				

				//target->IsInCombat();combat group is enough
				target->GetCombatGroup()->searchState;
				//Run this on
				target->GetActorRuntimeData().boolFlags.any(RE::Actor::BOOL_FLAGS::kAngryWithPlayer);
				middle->pickPocketed;//those who've witnessed people being pick pocketed will suspect you, so if they've come to suspect you, don't pick pocket
				//around them.

				float val;

				if (av == RE::ActorValue::kHealth || av == RE::ActorValue::kNone && a_this->actorValue == RE::ActorValue::kHealth)
				{
					if (!target->IsDead() && a_this->spell->IsPoison())
					{
						//I think what I'll do is that I'll represent the time of death with a positive number, priming with 1, and invalid with 0.
						target->GetMiddleHighProcess()->lastHitData;

						auto caster = a_this->GetCasterActor();
						if (caster && caster->IsPlayerRef() == true) {
							val = target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLastFlattered);
							
							should = true;
							
							disqualify = (group && group->searchState && target->IsHostileToActor(caster.get())) ||
								target->GetActorRuntimeData().boolFlags.any(RE::Actor::BOOL_FLAGS::kAngryWithPlayer) ||
								(middle && middle->pickPocketed);

							if (std::abs(val) == 2) {
								target->AsActorValueOwner()->SetActorValue(RE::ActorValue::kLastFlattered, 0);
								val = 0;
							}

						}
						//a_cause && a_cause->IsPlayerRef() == true
					}
				}


				func(a_this, target, value, av);

				//if it's value is 0, it moves onto stage 1, waiting to actually be used. if it's value is stage 2, it's been used, and it's our hob to put it back


				//This needs a second hook to work, the murder alarm takes place elsewhere
				
				if (should)
				{
					if (middle && middle->killQueued && val >= 0) {
						target->AsActorValueOwner()->SetActorValue(RE::ActorValue::kLastFlattered, !disqualify ? 1.f : -1.f);
					}
				}
				


				/*
				//Old
				if (middle && middle->killQueued && should){
					target->AsActorValueOwner()->SetActorValue(RE::ActorValue::kLastFlattered, 1);
					//(uint8_t&)middle->killQueued |= (1 << 7);

					//target->GetActorRuntimeData().boolBits &= (RE::Actor::BOOL_BITS)~std::to_underlying(RE::Actor::BOOL_BITS::kMurderAlarm);
				}
				else if (target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLastFlattered) == 2) {
					target->AsActorValueOwner()->SetActorValue(RE::ActorValue::kLastFlattered, 0);
				}
				//*/
			}


			inline static REL::Relocation<decltype(thunk)> func;
		};



		struct PoisonForgive1Hook
		{
			static void Install()
			{
				//SE: 5DEF20, AE: xxx, VR: ???
				auto hook = REL::RelocationID(36431, 0).address() + 0x1E7;


				auto& trampoline = SKSE::GetTrampoline();

				func = trampoline.write_call<5>(hook, thunk);
			}


			static RE::TESFaction* thunk(RE::Character* target)
			{
				auto value = target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLastFlattered);

				if (value == 1.f) {
					target->AsActorValueOwner()->SetActorValue(RE::ActorValue::kLastFlattered, 2);
					return nullptr;
				}
				
				if (value == -1.f) {
					target->AsActorValueOwner()->SetActorValue(RE::ActorValue::kLastFlattered, -2.f);
				}

				return func(target);
			}


			inline static REL::Relocation<decltype(thunk)> func;
		};


		struct PoisonForgive2Hook
		{
			static void Install()
			{
				//SE: 5DEF20, AE: xxx, VR: ???
				auto hook = REL::RelocationID(36431, 0).address() + 0x3E4;


				struct Patch : Xbyak::CodeGenerator
				{
					explicit Patch(uintptr_t address, uintptr_t func)
					{
						mov(rdx, ptr[rbp + 0x57 + 0x10]);
						mov(rax, func);
						call(rax);
						ret();

					}
				} static code{ hook, (uintptr_t)thunk };



				auto& trampoline = SKSE::GetTrampoline();

				func = trampoline.write_call<5>(hook, (uintptr_t)code.getCode());
			}

			
			static RE::TESFaction* thunk(RE::Character* witness, RE::Character* target)
			{
				static RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();


				auto middle = witness->GetMiddleHighProcess();

				//So it will be viable if 

				auto group = witness->GetCombatGroup();

				//*
				bool disqualify = (group && group->searchState && witness->IsHostileToActor(player)) ||
					witness->GetActorRuntimeData().boolFlags.any(RE::Actor::BOOL_FLAGS::kAngryWithPlayer) ||
					(middle && middle->pickPocketed);


				if (!disqualify && target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLastFlattered) >= 1)
					return nullptr;

				return func(witness);
			}


			inline static REL::Relocation<RE::TESFaction*(RE::Character*)> func;
		};


	public:
		static void Install()
		{
			SKSE::AllocTrampoline(156);
			//TestPatch::Patch();

			//6A1E30+2B
			static constexpr std::uint8_t NoOperation3[0x3]{ 0x0F, 0x1F, 0x00 };
			static_assert(sizeof(NoOperation3) == 0x3);


			static constexpr std::uint8_t NoOperation2[0x2]{ 0x66, 0x90 };
			static_assert(sizeof(NoOperation2) == 0x2);

			REL::RelocationID comp{ 39407, 40482 };//SE: 0x6A1E30, AE: 0x6DC830, VR:???


			//bool is_ae = REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_5_97) == std::strong_ordering::greater;

			uintptr_t offset = 0x2B;
			
			//const uint8_t* op_addr = !IsAE() ? &Utility::NoOperation3[0] : &Utility::NoOperationA[0];
			const uint8_t* op_addr = &NoOperation2[0];//&NoOperation3[0];
			//size_t op_size = !IsAE() ? 0x3 : 0xA;
			size_t op_size = 0x2;//0x3;

			//REL::Module::get().version().compare(v)
			REL::safe_write(comp.address() + offset, op_addr, op_size);
			MenuCallback__RemoveItemCall::Install();
			MenuCallback__ApplyDosage::Install();
			PoisonBlameHook::Install();
			PoisonForgive1Hook::Install();
			PoisonForgive2Hook::Install();
			StandardItemData__DisplayName::Install();
			AttackBlockHandler_Hook::Patch();
			ItemDrop_Hook::Patch();
			DecrementPoison_Hook::Patch();
			MainUpdateHook::Patch();
			PlayerCharacter__PendPoison::Patch();

			
		}
	};


	/*
		//A note, these all get a ton easier to design if you aren't trying to submit a fuck ton of calls and branches at the same time. Allow them to be, 
		// one at a time.

		//Singleton id is a thing that's basically used to organize which singleton object should be accessed.
	template <class T>//requres that it derives from some specialization of hookstruct.
	struct SingletonID
	{
		using Type = T;
		inline static size_t value = std::numeric_limits<size_t>::max();

		operator size_t() { return value; }
	};

	struct HookBase
	{

		static std::mutex g_initLock;
		//These are random ideas for profiling data. I could make it so such a thing is enabled if is in debug, or in pseudo debug release
		//inline static size_t classCount = 0;
		//inline static size_t hookCount = 0;
	};

	template<size_t N, class R, class... Args>
	struct HookStruct : public HookBase
	{
		static constexpr size_t Size = N;

		using Return = R;

		using FuncType = Return(Args...);
		using Relocation = REL::Relocation<FuncType>;

		using Self = HookStruct<N, R, Args...>;
		using hook_type = HookStruct<N, R, Args...>;




		//To this, there's a problem of uniqueness. Sharing parameters and a size will result in a fighting for space. So here's how it might go
		// we have a vector of singletons, and each time a new class is found, we attach another singleton. Then we find someway to back the search
		// into the static thunk and that might do it?

		//I have an idea


		//Hook
		Self* _singleton = nullptr;
		inline static std::vector<Self*> _singletonList;

		//When a hook function is used, this is where it will go to. Then, the utility functions will set up the proper place

		//Might send an index with this too
		//This finds a way to create itself some how and then uses the virtual function for processing.
		virtual R thunk(Args... args, Relocation& func) = 0;//At the end it should have a function that these objects can use.


		R _safethunk(Args... args, Relocation& func)
		{
			if (!this) {
				//Log issue here.
				return func(args...);
			}
			else
				return thunk(args, func);
		}


		virtual void Init()
		{
			//A function used to install any necessary changes that the function might additionally need to to through.
			// called when singleton is first launched.
		}


		virtual void GenerateBranchAssembly(Xbyak::CodeGenerator& code)
		{


			//A function used to install any necessary changes that the function might additionally need to to through.
			// called when singleton is first launched.
		}


		template<class T>
		static void Install()
		{
			constexpr bool is_type_void = std::is_same_v<T, void>;

			if constexpr (is_type_void) {
				assert(is_type_void);
				return;
			}

			//Make a lock right here. put it within a base class, so a fuck ton of mutex locks are produced.

			//if (_singleton)
			if (SingletonID<T>::value != SingletonID<void>::value)
				return;

			std::lock_guard<std::mutex> guard{ g_initLock };

			SingletonID<T>::value = _singletonList.size();

			T* hook_class = new T();

			_singletonList.push_back(hook_class);

			hook_class->Init();
		}

		//usingSelf 
		template<size_t I = 0, class T>
		static R thunk(Args... args)
		{
			//return _singleton->_safethunk(args..., _func[I]);
			return _singletonList[SingletonID<T>::value]->_safethunk(args..., _func[I]);
		}
		//This can actually be a tuple instead, and if it is, the thunk can sort it out.
		//It being a tuple would solve the issue where the types may not have coresponding types.
		static inline std::array<Relocation, N> _func;
	};

	struct Test : public HookStruct<1, void, int, bool>
	{

	};

	template <class T>
	static void Example()
	{
		using HookStructID = SingletonID<T>;

		T::Install<T>();
	}

	namespace stl
	{



		using namespace SKSE::stl;


		namespace detail
		{
			template <class T, size_t I, size_t S>
			void write_thunk_call(SKSE::Trampoline& trampoline, std::array<uintptr_t, S>& a_src)
			{
				T::func[I] = trampoline.write_call<5>(a_src[I], T::thunk<I, T>);

				if constexpr (I != 0)
					return write_thunk_call(trampoline, a_src);
			}
		}
		template <class T, std::convertible_to<uintptr_t>... Is> requires (sizeof...(Is) == T::Size)
		void write_thunk_call(Is... a_srcs)
		{
			T::Install<T>();

			constexpr size_t Size = T::Size;

			std::array<uintptr_t, Size> sourceArray{ a_srcs... };

			SKSE::AllocTrampoline(14 * Size);

			auto& trampoline = SKSE::GetTrampoline();

			detailed::write_thunk_call<T, Size, Size>(trampoline, sourceArray);
		}

		template <class T, std::convertible_to<uintptr_t>... Is> requires (sizeof...(Is) == T::Size)
			void write_thunk_call(Is... a_srcs)
		{
			T::Install<T>();

			constexpr size_t Size = T::Size;

			std::array<uintptr_t, Size> sourceArray{ a_srcs... };

			SKSE::AllocTrampoline(14 * Size);

			auto& trampoline = SKSE::GetTrampoline();

			detailed::write_thunk_call<T, Size, Size>(trampoline, sourceArray);
		}


		template <class F, std::size_t idx, class T>
		void write_vfunc()
		{
			REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
			T::func = vtbl.write_vfunc(idx, T::thunk);
		}

		template <std::size_t idx, class T>
		void write_vfunc(REL::VariantID id)
		{
			REL::Relocation<std::uintptr_t> vtbl{ id };
			T::func = vtbl.write_vfunc(idx, T::thunk);
		}
	}
	//*/
}
