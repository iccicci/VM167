#pragma once

namespace logic
{
	public struct TickInfo {
		int         button;
		const char* log;
	};

	public ref class LogicInterface
	{
	private:

		class Logic*     logic;

	public:

		LogicInterface();
		~LogicInterface();

		void             clearScreen(int buttonId);
		System::Boolean  connect();
		float            getFontSize();
		System::Boolean  init();
		struct TickInfo* tick();
	};
}
