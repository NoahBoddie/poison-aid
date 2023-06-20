#pragma once

namespace RE
{
	class FloatSetting
	{
	private:
		uintptr_t _vtable = RE::VTABLE_Setting[0].address(); //0x00

	public:
		float			currValue;							// 0x08
		mutable float	prevValue;							// 0x0C
		const char* name;									// 0x10

		float GetValue() const { return currValue; }

		void Update() const { prevValue = currValue; }

		FloatSetting(const char* a_name, float a_value)
		{
			name = a_name;
			currValue = a_value;
			prevValue = a_value;
		}

		operator float() { return currValue; }
		operator RE::Setting* () { return reinterpret_cast<RE::Setting*>(this); }
	};
	static_assert(sizeof(FloatSetting) == 0x18);
}