#pragma once

#include "SettingManager.h"

namespace POS
{
	class Papyrus
	{
	
		static void _OnConfigClose(RE::TESQuest*)
		{
			SettingManager::ReadSettings();
		}

		static bool _Register(RE::BSScript::IVirtualMachine* a_vm)
		{
			a_vm->RegisterFunction("OnConfigClose", "PoisonAid_MCM", _OnConfigClose);

			logger::info("Registered PoisonAid_MCM class");
			return true;
		}
	public:
		static void Register()
		{
			auto papyrus = SKSE::GetPapyrusInterface();
			papyrus->Register(_Register);
			logger::info("Registered papyrus functions");
		}
	};


	
}