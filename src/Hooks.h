
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

					
					if (result == 2)
						return;

					logger::info("test {}?", result);

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
						UnkB unk_b{};
						UnkA unk_a{ inventory_menu.get(), unk_b };
						
						func(unk_a);
					}
				};

				RE::InventoryMenu* menu = *reinterpret_cast<RE::InventoryMenu**>(unk1 + 24);
				double count = *reinterpret_cast<double*>(*reinterpret_cast<uint64_t*>(unk1 + 40) + 16);

				if (count != 1.f)//There shouldn't be multiples of this to drop. Just an extra check.
					return func(unk1);

				auto* item = menu->GetRuntimeData().itemList->GetSelectedItem();

				RE::InventoryEntryData* obj = item->data.objDesc;

				if (!obj)
					return;
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
						mov(rdx, rsi);
						mov(rax, ret_addr);
						jmp(rax);
					}
				} static code{ (uintptr_t)thunk<0> };


				//SE: 0x7220B0+0x169, AE: 0x75F320+0x174, VR: ???
				//NEW: SE: 0x7220B0+0x11A, AE: 0x75F320+0x11F, VR: ???
				REL::Relocation<uintptr_t> hook{ REL::RelocationID(41778, 42859), REL::VariantOffset(0x11A, 0x11F, 0) };


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

				//SE: 0x6310A0+0x188, AE: 0x6691F0+0x18B, VR: ???
				//NEW!: SE: 0x6310A0+0x148, AE: 0x6691F0+0x153, VR: ???
				REL::Relocation<uintptr_t> hook{ REL::RelocationID(37799, 38748), REL::VariantOffset(0x148, 0x153, 0) };


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

	public:
		static void Install()
		{
			SKSE::AllocTrampoline(70);
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
