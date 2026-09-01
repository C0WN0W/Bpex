#pragma once

typedef long long uint64_t;

const uint64_t OFF_ENTITYLIST = 0x5d5b618;
const uint64_t OFF_LOCALPLAYER = 0x26781f8;
const uint64_t OFF_VIEW_MATRIX = 0x11a390;
const uint64_t OFF_VIEW_RENDER = 0x44580a0;
const uint64_t OFF_LEVELNAME = 0x1e23ba4;
const uint64_t OFF_LOCAL_ENTITY_HANDLE = 0x2505fe0;
const uint64_t OFF_NAMELIST = 0x87dd320;

const uint64_t OFF_VecAbsOrigin = 0x16C;
const uint64_t OFF_iTeamNum = 0x334;
const uint64_t OFF_iHealth = 0x324;
const uint64_t OFF_iMaxHealth = 0x470;
const uint64_t OFF_iShield = 0x190;
const uint64_t OFF_iMaxShield = 0x194;
const uint64_t OFF_Armor = 0x48d4;
const uint64_t OFF_scriptNameIndex = 0x588;
const uint64_t OFF_fFlags = 0xC8;
const uint64_t OFF_LifeState = 0x698;
const uint64_t OFF_bleedoutState = 0x27d0;
const uint64_t OFF_Bone = 0xdd0 + 0x48;	// m_nForceBone + 0x48 ?
const uint64_t OFF_StudioHdr = 0x1000;
const uint64_t OFF_SignifierName = 0x478;

namespace Global {
	inline int pid;
	inline uint64_t GameBase;
	inline uint64_t ViewRender;
	// inline Matrix m;
}