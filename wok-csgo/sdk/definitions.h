#pragma once

#define IN_RANGE(a, b, c)		(a >= b && a <= c) 

#define TICK_INTERVAL			(interfaces::m_global_vars->m_interval_per_tick)

#define TIME_TO_TICKS(dt)		(static_cast<int>(0.5f + static_cast<float>(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t)		(TICK_INTERVAL * (t))
#define ROUND_TO_TICKS(t)		(TICK_INTERVAL * TIME_TO_TICKS(t))

#define NUM_ENT_ENTRY_BITS				(11 + 2)
#define NUM_ENT_ENTRIES					(1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX			0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS				16
#define NUM_SERIAL_NUM_SHIFT_BITS		(32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK					((1 << NUM_SERIAL_NUM_BITS) - 1)

#define DECL_ALIGN(x)			__declspec(align(x))

#define ALIGN4 DECL_ALIGN(4)
#define ALIGN8 DECL_ALIGN(8)
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN32 DECL_ALIGN(32)
#define ALIGN128 DECL_ALIGN(128)

#define MAX_COORD_FLOAT (16384.0f)
#define MIN_COORD_FLOAT (-MAX_COORD_FLOAT)

#define EVENT_DEBUG_ID_INIT 42
#define EVENT_DEBUG_ID_SHUTDOWN 13

#define MULTIPLAYER_BACKUP				150

#define DISPSURF_FLAG_SURFACE           (1 << 0)
#define DISPSURF_FLAG_WALKABLE          (1 << 1)
#define DISPSURF_FLAG_BUILDABLE         (1 << 2)
#define DISPSURF_FLAG_SURFPROP1         (1 << 3)
#define DISPSURF_FLAG_SURFPROP2         (1 << 4)

#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"
#define TEXTURE_GROUP_STATIC_PROP					"StaticProp textures"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

#define CHAR_TEX_CONCRETE	'C'	
#define CHAR_TEX_METAL		'M'
#define CHAR_TEX_DIRT		'D'
#define CHAR_TEX_VENT		'V'
#define CHAR_TEX_GRATE		'G'
#define CHAR_TEX_TILE		'T'
#define CHAR_TEX_SLOSH		'S'
#define CHAR_TEX_WOOD		'W'
#define CHAR_TEX_COMPUTER	'P'
#define CHAR_TEX_GLASS		'Y'
#define CHAR_TEX_FLESH		'F'
#define CHAR_TEX_SNOW		'N'
#define CHAR_TEX_PLASTIC	'L'
#define CHAR_TEX_CARDBOARD	'U'

#define	CONTENTS_EMPTY					0

#define	CONTENTS_SOLID					0x1
#define	CONTENTS_WINDOW					0x2
#define	CONTENTS_AUX					0x4
#define	CONTENTS_GRATE					0x8
#define	CONTENTS_SLIME					0x10
#define	CONTENTS_WATER					0x20
#define	CONTENTS_BLOCKLOS				0x40
#define CONTENTS_OPAQUE					0x80
#define	LAST_VISIBLE_CONTENTS			CONTENTS_OPAQUE

#define ALL_VISIBLE_CONTENTS			(LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS - 1))

#define CONTENTS_TESTFOGVOLUME			0x100
#define CONTENTS_UNUSED					0x200	

#define CONTENTS_BLOCKLIGHT				0x400

#define CONTENTS_TEAM1					0x800
#define CONTENTS_TEAM2					0x1000

#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

#define CONTENTS_MOVEABLE				0x4000

#define	CONTENTS_AREAPORTAL				0x8000

#define	CONTENTS_PLAYERCLIP				0x10000
#define	CONTENTS_MONSTERCLIP			0x20000

#define	CONTENTS_CURRENT_0				0x40000
#define	CONTENTS_CURRENT_90				0x80000
#define	CONTENTS_CURRENT_180			0x100000
#define	CONTENTS_CURRENT_270			0x200000
#define	CONTENTS_CURRENT_UP				0x400000
#define	CONTENTS_CURRENT_DOWN			0x800000

#define	CONTENTS_ORIGIN					0x1000000

#define	CONTENTS_MONSTER				0x2000000
#define	CONTENTS_DEBRIS					0x4000000
#define	CONTENTS_DETAIL					0x8000000
#define	CONTENTS_TRANSLUCENT			0x10000000
#define	CONTENTS_LADDER					0x20000000
#define CONTENTS_HITBOX					0x40000000

#define	MASK_ALL					(0xFFFFFFFF)
#define	MASK_SOLID					(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
#define	MASK_NPCSOLID				(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
#define	MASK_NPCFLUID				(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER)
#define	MASK_WATER					(CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME)
#define	MASK_OPAQUE					(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE)
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE | CONTENTS_MONSTER)
#define MASK_BLOCKLOS				(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS)
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS | CONTENTS_MONSTER)
#define	MASK_VISIBLE				(MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE)
#define	MASK_SHOT					(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX)
#define MASK_SHOT_BRUSHONLY			(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS)
#define MASK_SHOT_HULL				(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE)
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC_FLUID	(CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP)
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER | CONTENTS_SLIME)
#define MASK_CURRENT				(CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN)
#define	MASK_DEADSOLID				(CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE)
#define	MASK_SHOT_PLAYER			(MASK_SHOT_HULL | CONTENTS_HITBOX)

#define MAX_PENETRATION_DISTANCE	90.f

#define CAM_HULL_OFFSET		14.f
#define CAM_HULL_MIN		vec3_t(-CAM_HULL_OFFSET, -CAM_HULL_OFFSET, -CAM_HULL_OFFSET)
#define CAM_HULL_MAX		vec3_t(CAM_HULL_OFFSET, CAM_HULL_OFFSET, CAM_HULL_OFFSET)

struct anim_layer_t;
struct matrix3x4_t;

using pose_params_t = std::array<float, 24u>;
using anim_layers_t = std::array<anim_layer_t, 13u>;
using bone_matrix_t = std::array<matrix3x4_t, 256u>;

namespace math {
	double __forceinline __declspec (naked) __fastcall sqrt(double n) {
		_asm {
			fld qword ptr[esp + 4]
			fsqrt
			ret 8
		}
	}
}
