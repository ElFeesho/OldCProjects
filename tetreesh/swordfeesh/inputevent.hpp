/*
	InputEvent will store key press data, pow
	InputEvent will also house joystick and wiimotes, wiiboard data!
	Mainly wiiboards right now, but can be extended later!

	class Wiiboards will be a wrapper for all wiiboard connectivity and
	InputEvents will be passed to entities which will be able to manipulate
	it to get wiiboard data they require!
*/

#ifndef __INPUT_EVENT_HPP__
#define __INPUT_EVENT_HPP__

namespace SwordFeesh
{
	class InputEvent
	{
	public:
		InputEvent();
		~InputEvent();
		static void init_wiiboards();
	private:
	
	};
}

#endif
