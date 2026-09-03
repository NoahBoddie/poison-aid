#pragma once

#include "Utility.h"

namespace POS
{
	enum struct WidgetTarget
	{
		Right,
		Left,
		Pouch,
	};

	enum struct WidgetState : uint8_t
	{
		Active,
		Inactive
	};

	//update func
	//stl::write_thunk_call<MainUpdate_Nullsub>(REL::RelocationID(35565, 36564).address() + REL::Relocate(0x748, 0xC26));

	constexpr RE::NiColor poisonInUse{ 1, 256,1 };
	constexpr RE::NiColor poisonOutOfUse{ 256, 1,1 };

	constexpr RE::NiColor stateColors[]
	{
		RE::NiColor{ 1, 256,1 },
		RE::NiColor{ 256, 1, 1 },
	};

	

	//Make game setting or string or something
	constexpr const char* tmpNone = "None";
	std::string_view widgetID = "PAID";
	std::string_view widgetLoader = "_root.widgetLoaderContainer.widgetLoader";
	std::string_view widgetSource = "poisoningextended/poisoninfo.swf";//should be from poison aid
	std::string_view widgetRoot = "_root.widgetLoaderContainer.widgetLoader.PAID";

	struct WidgetManager : public RE::BSTEventSink<RE::MenuOpenCloseEvent>, public RE::BSTEventSink<RE::TESEquipEvent>
	{
		inline static RE::GFxMovieView* movie = nullptr;
		inline static std::string pouchText = tmpNone;
		inline static RE::AlchemyItem* pouchPoison = nullptr;
		inline static bool isVisible = false;//If false, things will progress toward invisible regardless.

		static bool IsViable()
		{
			return movie;
		}

		struct Widget
		{
			//std::string_view pouchPath;
			//std::string_view widgetPath;

			std::string text;
			RE::AlchemyItem* poison = nullptr;
			float alpha = 0.f;
			bool usesPouch = false;
			RE::NiColor oldColor;		
			WidgetState targetState = WidgetState::Active;
			uint32_t timestamp = -1;

			constexpr bool operator==(const Widget& other) const noexcept
			{
				//Basically, if they have the same text and the same conditions for the text they are equal
				return text == other.text &&
					//poison == other.poison &&
					usesPouch == other.usesPouch;
			}

			float GetTime()
			{
				if (timestamp == -1) {
					return -1;
				}

				auto ms = RE::GetDurationOfApplicationRunTime();

				return (ms - timestamp) / 1000.f;

			}

			void SetColor(const RE::NiColor& color)
			{	
				bool does;

				does = movie->SetVariableDouble("colorR", color.red);
				assert(does);
				does = movie->SetVariableDouble("colorB", color.blue);
				assert(does);
				does = movie->SetVariableDouble("colorG", color.green);
				assert(does);
			}

			void SetText(std::string_view newText, bool setText  = true)
			{
				if (setText)
					text = newText;

				movie->SetVariable("text", newText.data());
			}

			bool UsesPouch() const
			{
				return !poison && pouchPoison;
			}

			void RefreshColor()
			{
				SetColor(stateColors[(int)targetState]);
			}

			void SetState(WidgetState state, bool immediate = false)
			{
				if (targetState == state)
					return;

				if (alpha <= 0)
					immediate = true;
				
				targetState = state;

				if (immediate) {
					RefreshColor();
				}
				else{
					oldColor.red = movie->GetVariableDouble("colorR");
					oldColor.blue = movie->GetVariableDouble("colorB");
					oldColor.green = movie->GetVariableDouble("colorG");
					timestamp = RE::GetDurationOfApplicationRunTime();//this should maybe use seeking timestamps instead
				}
				

				
			}

			void UpdatePouch()
			{
				//Only does something if pouch poison is being used.
				if (!UsesPouch()) {
					return;
				}

				SetText(pouchText, false);
			}


			void UpdatePoison(RE::AlchemyItem* alch, uint32_t count)
			{
				if (!count) {
					SetState(WidgetState::Inactive);
				}
				else {
					SetState(WidgetState::Active);
				}
				std::string newText = std::format("{} ({})", alch->GetName(), count);
				
				SetText(newText);
			}

			void EmplacePoison(RE::ExtraPoison* ex_poison)
			{
				if (!ex_poison) {
					SetText(pouchText, false);
					if (UsesPouch()) {
						SetState(WidgetState::Active);
					}
				}
				else {
					UpdatePoison(ex_poison->poison, ex_poison->count);
				}
			}

			bool IsVisible() const
			{
				return (poison || pouchPoison) && isVisible;
			}

			void UpdateAlpha(float delta, bool fade)
			{
				constexpr float alphaConst = 256;
				constexpr float maxAlpha = 1.f;
				constexpr float timeToFadeOut = 0.5f;
				constexpr float timeToFadeIn = 0.75f;



				if (IsVisible() && !fade) {
					if (alpha >= alphaConst) {
						return;
					}

					if (alpha <= 0) {
						//TODO: mark as visible if need be
					}

					alpha += (alphaConst / timeToFadeIn) * delta;
				}
				else {
					if (alpha <= 0) {
						return;
					}

					alpha -= (alphaConst / timeToFadeOut) * delta;

					if (alpha > 0) {
						//TODO: Mark as invisible an don't set alpha
					}
				}



			}

			void UpdateColor()
			{
				if (alpha <= 0) {
					return;
				}

				float time = GetTime();

				if (time < 0)
					return;

				constexpr float time_constant = 0.4f;

				float value = time / time_constant;

				if (value > 1.f) {
					timestamp = -1;
					value = 1;
				}

				RE::NiColor current = oldColor + value * (oldColor - stateColors[(int)targetState]);
				//Do the thing.

				SetColor(current);
			}


			bool Update(float deltaTime, bool fade = false)
			{
				//update precedence is first color change then update visibility
				UpdateAlpha(deltaTime, fade);
				UpdateColor();
			}


		};


		inline static Widget lhsValue;
		inline static Widget rhsValue;


		//This will need to update when:
		// poison is equipped
		// when the hud first opens
		// when a decrement happens (I should have the hook for that

	
		
		static void Register()
		{
			static WidgetManager singleton;
			RE::UI* ui = RE::UI::GetSingleton();
			auto source = RE::ScriptEventSourceHolder::GetSingleton();
			ui->AddEventSink<RE::MenuOpenCloseEvent>(&singleton);
			source->AddEventSink<RE::TESEquipEvent>(&singleton);
			//TODO: Register for this mod event.
			//RegisterForModEvent("SKIWF_widgetManagerReady", "OnWidgetManagerReady")
		}


		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* src) override
		{
			if (event->opening && event->menuName == RE::HUDMenu::MENU_NAME) {
				Initialize();
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* event, RE::BSTEventSource<RE::TESEquipEvent>* src) override
		{


			if (event->equipped && event->actor->IsPlayerRef())
			{
				auto actor = event->actor->As<RE::Actor>();

				if (actor && actor->IsPlayerRef() == true) {
					auto process = actor->GetMiddleHighProcess();

					if (process) {
						lhsValue.EmplacePoison(GetExtraPoison(process->leftHand));
						rhsValue.EmplacePoison(GetExtraPoison(process->rightHand));

					}

				}
			}

			return RE::BSEventNotifyControl::kContinue;
		}


		void UpdateFrame(float deltaTime)
		{
			rhsValue.Update(deltaTime); 
			lhsValue.Update(deltaTime, rhsValue == lhsValue);
		}

		void UpdatePoison(WidgetTarget tar, RE::AlchemyItem* alch, uint32_t count)
		{
			switch (tar)
			{
			case WidgetTarget::Pouch:
				pouchText = alch ? std::format("{} ({})", alch->GetName(), count) : tmpNone;
				pouchPoison = alch;
				lhsValue.UpdatePouch();
				rhsValue.UpdatePouch();
				break;

			case WidgetTarget::Left:
				GetWidget(false).UpdatePoison(alch, count);
				break;

			case WidgetTarget::Right:
				GetWidget(true).UpdatePoison(alch, count);
				break;

			}
		}



		////////////////////////////////////////////////
		
		void Reset()
		{
			pouchPoison = nullptr;
			lhsValue = rhsValue = Widget{};
			movie = nullptr;
			//should initialize each time
		}

		void UpdateText()
		{
			static RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

			if (player) {
				auto process = player->GetMiddleHighProcess();

				if (process) {
					lhsValue.EmplacePoison(GetExtraPoison(process->leftHand));
					rhsValue.EmplacePoison(GetExtraPoison(process->rightHand));
				}

			}
		}

		void Initialize()
		{
			if (IsViable() == false) {
				RE::HUDMenu* hud = RE::UI::GetSingleton()->GetMenu<RE::HUDMenu>().get();

				if (!hud) {
					return;
				}

				movie = hud->uiMovie.get();
			}

			UpdateText();

			


		}


		Widget& GetWidget(bool is_right)
		{
			if (is_right) {
				return rhsValue;
			}
			else {
				return lhsValue;
			}

		}

	};
}