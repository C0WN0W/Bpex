#include "entity.hpp"

// 读地图名称
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

// 取实体指针
uint64_t GetEntityPtr(int i)
{
	return drv.RPM<uint64_t>(Global::GameBase + ENTITYLIST + ((uint64_t)i << 5));
}

// 取本地玩家指针
uint64_t GetLocalPlayerPtr()
{
	return drv.RPM<uint64_t>(Global::GameBase + LOCALPLAYER);
}

// 判断实体地址是否正确
bool Isvalid(uint64_t Addr)
{
	if (Addr == 0)
		return false;

	if (get_script_name(Addr) < -1000 || get_script_name(Addr) > 100000)
		return false;

	return true;
}

// 取坐标
Vector3 GetPosition(uint64_t Addr)
{
	return drv.RPM<Vector3>(Addr + VecAbsOrigin);
}

// 取倒地状态
int GetKnocked(uint64_t Addr)
{
	return drv.RPM<int>(Addr + bleedoutState);
}

// 取存活状态
int GetLifeState(uint64_t Addr)
{
	return drv.RPM<int>(Addr + LifeState);
}

// 取生命值
int GetHealth(uint64_t Addr)
{
	return drv.RPM<int>(Addr + iHealth);
}

// 取最大生命值。貌似没有意义
int GetMaxHealth(uint64_t Addr)
{
	return drv.RPM<int>(Addr + iMaxHealth);
}

// 取队伍ID
int GetTeamID(uint64_t Addr)
{
	return drv.RPM<int>(Addr + iTeamNum);
}

// 取距离（游戏引擎中的相对距离，转换为游戏中的米需要另外计算）
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

// 判断是否可见
bool GetVisible(uint64_t LocalPlayerPtr, uint64_t EntityPtr)
{
	float last_visible_time = drv.RPM<float>(EntityPtr + LastVisibleTime);
	float current_time = drv.RPM<float>(LocalPlayerPtr + TIMEBASE);
	bool visible = last_visible_time > current_time - 0.19f;	// 0.18极限 建议0.25
	return visible;
}

// 计算等级
int getLevel(int m_xp) {
	if (m_xp < 0) return 0;
	if (m_xp >= 714800) {
		return floor((m_xp - 714800 + 1) / 18000) + 57;
	}
	// 前56级需要的经验值，后面都一样
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

// 传入骨骼ID获取对应骨骼坐标
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