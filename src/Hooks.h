
#include "PoisonHandler.h"
#include "xbyak/xbyak.h"
namespace POS
{
	struct Hooks
	{
	private:
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
		
		struct PlayerCharacter__PendPoison
		{
			static void Patch()
			{
				//SE: 0x6A1CC0, AE: 0x???, VR: ???
				auto hook_addr = REL::RelocationID(39406,0000).address();

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
				logger::info("count {}, id {:08X}", equip_params->count, equip_params->equipSlot ? equip_params->equipSlot->formID : 0);
				return PoisonHandler::Handle(a_this, poison, equip_params->equipSlot);
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
				static uint64_t g_unkPtr = unk1;
				
				//The menu callback will require this
				void(*callback)(int) = [](int result)
				{
					func(g_unkPtr);
				};


				g_unkPtr = unk1;

				RE::InventoryMenu* menu = *reinterpret_cast<RE::InventoryMenu**>(unk1 + 24);
				auto* item = menu->GetRuntimeData().itemList->GetSelectedItem();

				RE::InventoryEntryData* obj = item->data.objDesc;

				if (obj && PoisonHandler::HandleRemovePoison(obj) == true) {
					menu->GetRuntimeData().itemList->Update(RE::PlayerCharacter::GetSingleton());
					
					return;
				
				}
				return func(unk1);


				//So the idea here is we want to see if this is poisoned, and equipped (as equipped is the safest and laziest way to do this)
				//We also want to see if it's a quest item so we know if we just want the remove poison dialogue.

			}

			static inline REL::Relocation<decltype(thunk)> func;
		};


		struct DecrementPoison_Hook
		{
			//
			//Note, hook is actually for SetBaseActorValue

			static void Patch1()
			{
				//Arrow patch
				
				struct Code : Xbyak::CodeGenerator
				{
					Code(uintptr_t ret_addr)
					{
						mov(rdx, rsi);
						mov(rax, ret_addr);
						jmp(rax);
					}
				} static code{ (uintptr_t)thunk<0> };


				//SE: 0x7220B0+0x169, AE: 0x75F320+0x174, VR: ???
				REL::Relocation<uintptr_t> hook{ REL::RelocationID(41778, 42859), REL::VariantOffset(0x169, 0x174, 0) };


				auto& trampoline = SKSE::GetTrampoline();


				func[1] = trampoline.write_call<5>(hook.address(), (uintptr_t)code.getCode());
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

				//SE: 0x6310A0+0x188, AE: 0x6691F0+0x18B, VR: ???
				REL::Relocation<uintptr_t> hook{ REL::RelocationID(37799, 38748),REL::VariantOffset(0x188, 0x18B, 0) };


				auto& trampoline = SKSE::GetTrampoline();


				func[1] = trampoline.write_call<5>(hook.address(), (uintptr_t)thunk<1>);
			}


			static void Patch()
			{
				Patch1();
				Patch2();

				logger::info("TESConditionItem__IsTrue complete...");
				//*/
			}


			static void thunk_(RE::InventoryEntryData* item) {}

			template <int I = 0>
			static void thunk(RE::InventoryEntryData* item)// , RE::Character* forced_a2)
			{
				//if (forced_a2->IsPlayerRef() == true)
				//	PoisonHandler::HandleOutOfPoison(forced_a2->As<RE::PlayerCharacter>(), item);


				return func[I](item);

			}

			static inline REL::Relocation<void(RE::InventoryEntryData*)> func[2];
		};

		template<size_t N, class R, class... Args>
		struct HookStruct
		{
			//Hook

			//This finds a way to create itself some how and then uses the virtual function for processing.
			virtual R thunk(Args... args)//At the end it should have a function that these objects can use.
			{

			}


			//usingSelf 
			template<size_t I = 0>
			static R thunk(Args... args)
			{
				
			}

			static inline REL::Relocation<decltype(thunk<>)> func[N];
		};

	public:
		static void Install()
		{
			SKSE::AllocTrampoline(56);
			//TestPatch::Patch();

			//6A1E30+2B
			static constexpr std::uint8_t NoOperation3[0x3]{ 0x0F, 0x1F, 0x00 };
			static_assert(sizeof(NoOperation3) == 0x3);

			REL::RelocationID comp{ 39407, 00000 };//SE: 0xA1E30, AE: 0x???, VR:???


			//bool is_ae = REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_5_97) == std::strong_ordering::greater;

			uintptr_t offset = RELOCATION_OFFSET(0x2B, 0x00);
			
			//const uint8_t* op_addr = !IsAE() ? &Utility::NoOperation3[0] : &Utility::NoOperationA[0];
			const uint8_t* op_addr = &NoOperation3[0];
			//size_t op_size = !IsAE() ? 0x3 : 0xA;
			size_t op_size = 0x3;

			//REL::Module::get().version().compare(v)
			REL::safe_write(comp.address() + offset, op_addr, op_size);
			ItemDrop_Hook::Patch();
			DecrementPoison_Hook::Patch();
			PlayerCharacter__PendPoison::Patch();
		}
	};
}
