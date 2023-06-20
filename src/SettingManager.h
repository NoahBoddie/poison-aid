#pragma once

#include "SimpleIni.h"
#include "StringSetting.h"
#include "FloatSetting.h"
namespace POS
{
	enum struct SettingType
	{
		RightEquipType,
		LeftEquipType,
		ItemSelectType,
		RightFailType,
		LeftFailType,
		SelectFailType,

		CleanPrompt,
		DrinkPrompt,
		ReplacePrompt,
		FortifyPrompt,
		MessageType,
		QuickReapply,
		QuickExtraApply,

	};

	struct SettingManager
	{
		static void ReadSettings()
		{
			constexpr auto defaultSettingsPath = L"Data/MCM/Config/PoisonAid/settings.ini";
			constexpr auto mcmPath = L"Data/MCM/Settings/PoisonAid.ini";

			const auto readMCM = [&](std::filesystem::path path) {
				CSimpleIniA mcm;
				mcm.SetUnicode();

				mcm.LoadFile(path.string().c_str());


				ReadIntegerSetting(mcm, "Primary", "bRightEquip", rightEquipType);
				ReadIntegerSetting(mcm, "Primary", "bLeftEquip", leftEquipType);
				ReadIntegerSetting(mcm, "Primary", "bItemSelect", itemSelectType);

				ReadIntegerSetting(mcm, "Primary", "bRightFail", rightFailType);
				ReadIntegerSetting(mcm, "Primary", "bLeftFail", leftFailType);
				ReadIntegerSetting(mcm, "Primary", "bSelectFail", selectFailType);


				ReadIntegerSetting(mcm, "Secondary", "bCleanPoison", cleanPrompt);
				ReadIntegerSetting(mcm, "Secondary", "bDrinkPoison", drinkPrompt);
				ReadIntegerSetting(mcm, "Secondary", "bReplacePoison", replacePrompt);
				ReadIntegerSetting(mcm, "Secondary", "bFortifyPoison", fortifyPrompt);

				ReadIntegerSetting(mcm, "Misc", "bMessageType", promptType);
				ReadFloatingSetting(mcm, "Misc", "fQuickReapplyWindow", quickReapply);
				ReadFloatingSetting(mcm, "Misc", "fQuickExtraApplyWindow", quickExtraApply);
			};

			logger::info("Reading MCM .ini...");
			
			readMCM(defaultSettingsPath);  // read the default ini first
			readMCM(mcmPath);

			logger::info("...success");
		}

		template <std::floating_point T>
		static void ReadFloatingSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, T& a_setting)
		{
			const char* bFound = nullptr;
			bFound = a_ini.GetValue(a_sectionName, a_settingName);
			if (bFound) {
				a_setting = static_cast<T>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
			}
		}

		template <std::integral T>
		static void ReadIntegerSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, T& a_setting)
		{
			const char* bFound = nullptr;
			bFound = a_ini.GetValue(a_sectionName, a_settingName);
			if (bFound) {
				a_setting = static_cast<T>(a_ini.GetLongValue(a_sectionName, a_settingName));
			}
		}
		//Make these their own types I can expand on.


		inline static RE::StringSetting confirmPoisonSelf{ "sConfirmPoisonSelf", "Are you sure you want to poison yourself with {}?" };
		inline static RE::StringSetting confirmPoisonReplace{ "sConfirmPoisonReplace", "Are you sure you want to replace {} with {} on {}?" };
		inline static RE::StringSetting confirmPoisonFortify{ "sConfirmPoisonFortify", "Are you sure you want to fortify {} on {}?" };

		inline static RE::StringSetting poisonSelf{ "sPoisonSelf", "Do you want to poison yourself with {}?" };
		inline static RE::StringSetting poisonHand{ "sPoisonHand", "Do you want to coat {} with {}?" };
		inline static RE::StringSetting poisonBothHands{ "sPoisonBothHands", "Do you want to coat {} or {} with {}?" };
		inline static RE::StringSetting poisonSelfOrHand{ "sPoisonSelfOrHand", "Do you want to poison yourself, or coat {} with {}?" };
		inline static RE::StringSetting poisonSelfOrBothHands{ "sPoisonSelfOrBothHands", "Do you want to poison yourself, coat {}, or coat {} with {}?" };
		inline static RE::StringSetting dropOrCleanPoison{ "sDropOrCleanPoison", "Drop item or Clean and remove {}?" };
		inline static RE::StringSetting cleanPoison{ "sCleanPoison", "Clean and remove {}?" };
		inline static RE::StringSetting poisonUnableToPoisonSelf{ "sPoisonUnableToPoisonSelf", "You cannot poison yourself with {}." };
		
		
		inline static RE::StringSetting poisonRunOutMessage{ "sPoisonRunOutMessage", "You have run out of {} doses." };
		inline static RE::StringSetting poisonReapplyMessage{ "sPoisonReapplyMessage", "{} reapplied to {}" };
		inline static RE::StringSetting poisonRecoverMessage{ "sPoisonRecoverMessage", "You recovered some doses of {} ({})." };//The second is plural
		inline static RE::StringSetting poisonRemoveMessage{ "sPoisonRemovedMessage", "Poison removed." };


		inline static RE::StringSetting poisonDrink{ "sPoisonDrink", "Drink Poison" };
		inline static RE::StringSetting poisonRight{ "sPoisonRight", "Right Item" };
		inline static RE::StringSetting poisonLeft{ "sPoisonLeft", "Left Item" };
		inline static RE::StringSetting clean{ "sClean", "Clean" };
		inline static RE::StringSetting drop{ "sDrop", "Drop" };

		inline static RE::StringSetting& poisonUnableToPoison = reinterpret_cast<RE::StringSetting&>(*RE::GameSettingCollection::GetSingleton()->GetSetting("sPoisonUnableToPoison"));
		inline static RE::StringSetting& poisonNoWeaponMessage = reinterpret_cast<RE::StringSetting&>(*RE::GameSettingCollection::GetSingleton()->GetSetting("sPoisonNoWeaponMessage"));
		inline static RE::StringSetting& Yes = reinterpret_cast<RE::StringSetting&>(*RE::GameSettingCollection::GetSingleton()->GetSetting("sYes"));
		inline static RE::StringSetting& Cancel = reinterpret_cast<RE::StringSetting&>(*RE::GameSettingCollection::GetSingleton()->GetSetting("sCancel"));

		inline static RE::FloatSetting reapplyDoseMult{ "fReapplyDoseMult", 0.5f };
		inline static RE::FloatSetting poisonRemoveChance{ "fPoisonRemoveChance", 33.0f };
		inline static RE::FloatSetting poisonRemoveMult{ "fPoisonRemoveMult", 0.80f };
		inline static RE::FloatSetting drinkPoisonValue = { "fDrinkPoisonValue", 0.f };

		static void Install()
		{
			RE::GameSettingCollection* collection = RE::GameSettingCollection::GetSingleton();

			collection->InsertSetting(confirmPoisonSelf);
			collection->InsertSetting(confirmPoisonReplace);
			collection->InsertSetting(confirmPoisonFortify);
			collection->InsertSetting(poisonSelf);
			collection->InsertSetting(poisonHand);
			collection->InsertSetting(poisonBothHands);
			collection->InsertSetting(poisonSelfOrHand);
			collection->InsertSetting(poisonSelfOrBothHands);
			collection->InsertSetting(poisonDrink);
			collection->InsertSetting(poisonRight);
			collection->InsertSetting(poisonLeft);
			collection->InsertSetting(reapplyDoseMult);
			collection->InsertSetting(poisonRemoveMult);
			collection->InsertSetting(poisonRemoveChance);
			//collection->InsertSetting(drinkPoisonValue);//Gonna make a boolean later.
			collection->InsertSetting(dropOrCleanPoison);
			collection->InsertSetting(cleanPoison);
			collection->InsertSetting(clean);
			collection->InsertSetting(drop);
			collection->InsertSetting(poisonReapplyMessage);
			collection->InsertSetting(poisonUnableToPoisonSelf);
			collection->InsertSetting(poisonRunOutMessage);
			collection->InsertSetting(poisonRecoverMessage);
			collection->InsertSetting(poisonRemoveMessage);
			ReadSettings();
		}


		inline static uint8_t rightEquipType = 0;
		inline static uint8_t leftEquipType = 0;
		inline static uint8_t itemSelectType = 1;
		//An option at a later point may be to mirror vanilla behaviour if it cannot apply it. Will expand later, so uint8_t for now.
		inline static uint8_t rightFailType = 0;
		inline static uint8_t leftFailType = 0;
		inline static uint8_t selectFailType = 1;

		inline static bool cleanPrompt = 0;
		inline static bool drinkPrompt = 0;
		inline static bool replacePrompt = 1;
		inline static bool fortifyPrompt = 1;

		inline static bool replaceIsRemove = false;
		inline static bool promptType = false;
		inline static float quickReapply = 0.75f;
		inline static float quickExtraApply = 0.25f;
	};


	
}