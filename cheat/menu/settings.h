#pragma once

namespace settings
{
	namespace aimbot
	{
		inline bool enable = true;
		inline bool kernelAim = false;
		inline bool silentAim = false;
		inline bool drawFov = false;
		inline bool crosshair = false;

		inline float fovSize = 0;
		inline float smoothness = 1;

		inline const char* aimkey[] = { "Left Mouse Button", "Right Mouse Button", "Mouse Button 4", "Mouse Button 5", "Shift", "Ctrl", "ALT", "Caps Lock" };
		inline int current_aimkey = 1; // default to Right Mouse Button
		inline int current_key;
	}

	namespace visuals
	{
		inline bool enable = true;
		inline bool drawBox = false;
		inline bool drawSkeleton = false;
		inline bool drawSnaplines = false;
		inline bool drawDistance = false;
		inline bool drawName = false;
	}

	namespace exploits
	{
		inline bool enable = true;
		inline bool mantleLunge = false;
		inline bool noSpread = false;
		inline bool noRecoil = false;
		inline bool noSway = false;
	}

	namespace misc
	{
		inline bool show_console = true;
		inline bool streamproof = true;
	}
}