#include "entity.hpp"

std::string GetLevelName()
{
	char tmpstr[64] = { 0 };
	for (int i = 0; i < 64; i++)
	{
		tmpstr[i] = drv.RPM<char>(Global::GameBase + LEVELNAME + i);
		if (tmpstr[i] == '\0') break;
	}
	return std::string(tmpstr);
}

uint64_t GetEntityPtr(int i)
{
	return drv.RPM<uint64_t>(Global::GameBase + ENTITYLIST + ((uint64_t)i << 5));
}

uint64_t GetLocalPlayerPtr()
{
	return drv.RPM<uint64_t>(Global::GameBase + LOCALPLAYER);
}

bool Isvalid(uint64_t Addr)
{
	if (Addr == 0)
		return false;

	if (get_script_name(Addr) < -1000 || get_script_name(Addr) > 100000)
		return false;

	return true;
}

Vector3 GetPosition(uint64_t Addr)
{
	return drv.RPM<Vector3>(Addr + VecAbsOrigin);
}

int GetKnocked(uint64_t Addr)
{
	return drv.RPM<int>(Addr + bleedoutState);
}

int GetLifeState(uint64_t Addr)
{
	return drv.RPM<int>(Addr + LifeState);
}

int GetHealth(uint64_t Addr)
{
	return drv.RPM<int>(Addr + iHealth);
}

int GetMaxHealth(uint64_t Addr)
{
	return drv.RPM<int>(Addr + iMaxHealth);
}

int GetTeamID(uint64_t Addr)
{
	return drv.RPM<int>(Addr + iTeamNum);
}

float GetDistance(Vector3 LocalPlayer, Vector3 Entity)
{
	Vector3 tmp = { 0,0,0 };
	tmp.x = LocalPlayer.x - Entity.x;
	tmp.y = LocalPlayer.y - Entity.y;
	tmp.z = LocalPlayer.z - Entity.z;

	return (float)sqrt((double)tmp.x * tmp.x + (double)tmp.y * tmp.y + (double)tmp.z * tmp.z);
}

std::string GetSignifier(uint64_t Addr)
{
	uint64_t SignifierPtr = drv.RPM<uint64_t>(Addr + SignifierName);
	return ReadStr32(SignifierPtr);
}

int get_script_name(uint64_t EntityAddr)
{
	return drv.RPM<int>(EntityAddr + scriptNameIndex);
}

bool GetVisible(uint64_t LocalPlayerPtr, uint64_t EntityPtr)
{
	float last_visible_time = drv.RPM<float>(EntityPtr + LastVisibleTime);
	float current_time = drv.RPM<float>(LocalPlayerPtr + TIMEBASE);
	// std::cout << "Local: " << LocalPlayerTime << " Entity: " << EntityTime << std::endl;
	bool visible = last_visible_time > current_time - 0.19f;
	return visible;
}

int getLevel(int m_xp) {
	if (m_xp < 0) return 0;
	if (m_xp >= 714800) {
		return floor((m_xp - 714800 + 1) / 18000) + 57;
	}
	static const int levels[] = {
		100, 2750, 6650, 11400, 17000, 23350, 30450, 38300, 46450, 55050,
		64100, 73600, 83550, 93950, 104800, 116100, 127850, 140050, 152400, 164900,
		177550, 190350, 203300, 216400, 229650, 243050, 256600, 270300, 284150, 298150,
		312300, 326600, 341050, 355650, 370400, 385300, 400350, 415550, 430900, 446400,
		462050, 477850, 493800, 509900, 526150, 542550, 559100, 575800, 592650, 609650,
		626800, 644100, 661550, 679150, 696900, 714800
	};

	int left = 0;
	int right = sizeof(levels) / sizeof(levels[0]);

	while (left < right) {
		int mid = left + (right - left) / 2;
		if (levels[mid] <= m_xp) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}
	return left + 1; // Levels are 1-indexed
}

bool IsInCrossHair(uint64_t LocalPlayerPtr, uint64_t EntityPtr)
{
	float LastVisibleTime;
	float WorldTime;
	WorldTime = drv.RPM<float>(LocalPlayerPtr + TIMEBASE);
	LastVisibleTime = drv.RPM<float>(EntityPtr + LastCrosshairTime);

	return (LastVisibleTime + 0.19f) >= WorldTime;
}

bool IsOnGround(uint64_t Addr)
{
	uint32_t flag = drv.RPM<uint32_t>(Addr + fFlags);
	return (flag & 0x1) != 0;
}

Vector3 GetViewOffset(uint64_t Addr)
{
	return drv.RPM<Vector3>(Addr + ViewOffset);
}

Vector3 NewHitbox(uintptr_t ent, int HitBox) {
	DWORD64 Bones = drv.RPM<DWORD64>(ent + mBone);
	if (!Bones) return Vector3();
	uintptr_t Model = drv.RPM<uintptr_t>(ent + StudioHdr);

	DWORD64 StudioHdr = drv.RPM<DWORD64>(Model + 8);
	if (!StudioHdr) return Vector3();

	uint16_t HitboxCache = drv.RPM<uint16_t>(StudioHdr + 0x34);
	uint64_t HitboxArray = StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));

	int HitboxNum = HitBox;

	uint16_t IndexCache = drv.RPM<uint16_t>(HitboxArray + 4);
	int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
	uint16_t Bone = drv.RPM<uint16_t>(HitboxIndex + HitboxArray + (HitboxNum * 0x20));

	Vector3 BoneOff;

	Matrix3x4 BoneMatrix = drv.RPM<Matrix3x4>(Bones + (Bone * sizeof(Matrix3x4)));
	BoneOff = { BoneMatrix._14, BoneMatrix._24, BoneMatrix._34 };

	Vector3 Pos = GetPosition(ent) + BoneOff;
	return Pos;
}