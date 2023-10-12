#pragma once
#include "common_includes.h"
#include "sdk/interfaces.h"
#include "singleton.h"
#include "features/listener/listener.h"
#include "menu/menu.h"

namespace globals {
	namespace angles {
		extern qangle_t		m_view;
		extern qangle_t		m_anim;
		extern qangle_t     m_real;
		extern qangle_t     m_fake;
	}

	extern HMODULE			m_module;
	extern c_local_player	m_local;
	extern c_local_weapon	m_weapon;
	extern c_user_cmd*		m_cur_cmd;
	extern bool				m_packet;
	extern bool				m_allow_anim_update;
	extern bool				m_allow_setup_bones;

	__forceinline bool& allow_anim_update( ) { return m_allow_anim_update; }
	__forceinline bool& allow_setup_bones( ) { return m_allow_setup_bones; }
}