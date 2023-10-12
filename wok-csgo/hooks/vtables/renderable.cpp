#include "../hooks.h"

bool __fastcall hooks::renderable::setup_bones::fn(i_client_renderable* ecx, void* edx, matrix3x4_t* bones, int max_bones, int mask, float time) {
	const auto player = reinterpret_cast< c_cs_player* >( ( uintptr_t ) ecx - 4u );
	if ( !player || !player->is_alive( ) || player->is_dormant( ) || player->is_friendly( globals::m_local ) )
		return original( ecx, edx, bones, max_bones, mask, time );

	if ( !globals::allow_setup_bones( ) ) {
		if ( !bones || max_bones == -1 )
			return true;

		if ( player == globals::m_local )
			return local_animations->on_bone_setup( bones, max_bones );
		else {
			std::memcpy( bones, player->get_bone_cache( )->m_cached_bones, sizeof( matrix3x4_t ) * max_bones );
			return true;
		}
	}

	return original(ecx, edx, bones, max_bones, mask, time);
}