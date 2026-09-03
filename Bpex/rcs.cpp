#include "rcs.hpp"
#include "aimbot.hpp"

Vector3 RecoilControl::GetViewAngle(uint64_t LocalPlayerPtr)
{
	return drv.RPM<Vector3>(LocalPlayerPtr + ViewAngle);
}

Vector2 RecoilControl::GetPunchAngle(uint64_t LocalPlayerPtr)
{
	return drv.RPM<Vector2>(LocalPlayerPtr + PunchAngle);
}

Vector3 RecoilControl::NormalizeAngle(Vector3 angle)
{
	while (angle.x > 89.f) angle.x -= 180.f;
	while (angle.x < -89.f) angle.x += 180.f;
	while (angle.y > 180.f) angle.y -= 360.f;
	while (angle.y < -180.f) angle.y += 360.f;
	return angle;
}

void RecoilControl::run(uint64_t LocalPlayerPtr)
{
	static Vector2 OldPunch = { 0,0 };

	if (!cfg::rcs) return;

	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000))
	{ 
		Vector3 angle = GetViewAngle(LocalPlayerPtr);
		Vector2 viewAngle = { angle.x, angle.y };
		Vector2 punchAngle = GetPunchAngle(LocalPlayerPtr);
		Vector2 delta = viewAngle - (viewAngle + (OldPunch - (punchAngle * 2.0f)));

		int MouseX = (int)(delta.y / (3.f * 0.011f) * 2.f);
		int MouseY = (int)(delta.x / (3.f * 0.011f) * 2.f);

		Aimbot::MoveMouse(MouseX, -MouseY);

		OldPunch = punchAngle * 2.0f;
	}
	else
	{
		OldPunch = { 0,0 };
	}
}

void RecoilControl::run_mem(uint64_t LocalPlayerPtr)
{
	static Vector2 OldPunch = { 0,0 };

	if (!cfg::rcs) return;

	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000))
	{
		Vector3 viewAngle = GetViewAngle(LocalPlayerPtr);
		Vector2 punchAngle = GetPunchAngle(LocalPlayerPtr);

		Vector2 delta = punchAngle - OldPunch;
		if (fabs(delta.x) > 5.0f || fabs(delta.y) > 5.0f) {
			OldPunch = punchAngle;
			return; // Filter out sudden large changes
		}

		if (punchAngle.x >= 0) return;

		float pitch = viewAngle.x + (OldPunch.x - punchAngle.x);
		float yaw = viewAngle.y + (OldPunch.y - punchAngle.y);

		Vector3 newAngle = { pitch, yaw, 0 };

		newAngle = NormalizeAngle(newAngle);

		drv.WPM<Vector3>(LocalPlayerPtr + ViewAngle, newAngle);
		OldPunch = punchAngle;
	}
	else
	{
		OldPunch = { 0,0 };
	}
}