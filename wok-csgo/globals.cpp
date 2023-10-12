#include "globals.h"

namespace globals {
	namespace angles {
		qangle_t    m_view = {};
		qangle_t    m_anim = {};
		qangle_t    m_real = {};
		qangle_t    m_fake = {};
	}

	HMODULE			m_module = nullptr;
	c_local_player	m_local = {};
	c_local_weapon	m_weapon = {};
	c_user_cmd*		m_cur_cmd = nullptr;
	bool			m_packet = true;
	bool			m_allow_anim_update = true;
	bool			m_allow_setup_bones = true;
}