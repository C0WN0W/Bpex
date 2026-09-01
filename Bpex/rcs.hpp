#pragma once
#include "entity.hpp"

namespace RecoilControl
{
	Vector3 GetViewAngle(uint64_t LocalPlayerPtr);
	Vector2 GetPunchAngle(uint64_t LocalPlayerPtr);

	Vector3 NormalizeAngle(Vector3 angle);

	void run(uint64_t LocalPlayerPtr);
}