#pragma once

namespace logic
{
	public ref class LogicInterface
	{
	private:

		class Logic *logic;

	public:

		LogicInterface();
		~LogicInterface();

		System::Boolean connect();
		float           getFontSize();
		System::Boolean init();
		const char*     tick();
	};
}
