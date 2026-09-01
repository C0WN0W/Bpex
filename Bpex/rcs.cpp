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