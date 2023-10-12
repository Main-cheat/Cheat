#include "../hooks.h"

qangle_t* __fastcall hooks::player::eye_angles::fn(c_cs_player* ecx, void* edx) {
	if (ecx != globals::m_local)
		return original(ecx, edx);

	static const auto return_to_anim_state_yaw = SIG("client.dll", "F3 0F 10 55 ? 51 8B 8E ? ? ? ?");
	static const auto return_to_anim_state_pitch = SIG("client.dll", "8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55 ?");
	static const auto return_to_anim_state_roll = SIG( "client.dll", "8B 55 0C 8B C8 E8 ? ? ? ? 83 C4 08 5E 8B E5" );

	const auto ret = memory::stack_t().ret();
	if (ret == return_to_anim_state_yaw
		|| ret == return_to_anim_state_pitch
		|| ret == return_to_anim_state_roll )
		return &globals::angles::m_fake;

	return original(ecx, edx);
}

void __fastcall hooks::player::update_client_side_animation::fn( c_cs_player* ecx, void* edx ) {
	if ( !ecx || !ecx->is_alive( ) || ecx->is_dormant( ) || ecx->is_friendly( globals::m_local ) )
		return original( ecx, edx );

	if ( !globals::allow_anim_update( ) ) {
		if ( ecx == globals::m_local )
			local_animations->on_animation_update( );

		return;
	}

	return original( ecx, edx );
}

void* __fastcall hooks::player::get_client_renderable::fn( c_cs_player*, void* ) {
	return nullptr;
}

void __fastcall hooks::player::do_extra_bones_processing::fn( c_cs_player* ecx, void* edx, int a2, int a3, int a4, int a5, int a6, int a7 ) {
	return;
}

void __fastcall hooks::player::build_transformations::fn( c_cs_player* ecx, void* edx, c_studio_hdr* hdr, vec3_t* pos, void* q, matrix3x4_t& camera_transform, int bone_mask, void* bone_computed ) {
	if ( !ecx || !ecx->is_alive( ) || ecx->is_dormant( ) )
		return original( ecx, edx, hdr, pos, q, camera_transform, bone_mask, bone_computed );

	const auto backup_jiggle_bones = ecx->is_jiggle_bones( );

	ecx->is_jiggle_bones( ) = false;

	original( ecx, edx, hdr, pos, q, camera_transform, bone_mask, bone_computed );

	ecx->is_jiggle_bones( ) = backup_jiggle_bones;
}

void __fastcall hooks::player::standard_blending_rules::fn( c_cs_player* ecx, void* edx, void* hdr, vec3_t* pos, void* q, float curtime, int mask ) {
	if ( !ecx || !ecx->is_player( ) || ecx->is_dormant( ) )
		return original( ecx, edx, hdr, pos, q, curtime, mask );

	ecx->get_effects( ).add( EF_NO_INTERP );

	original( ecx, edx, hdr, pos, q, curtime, mask );

	ecx->get_effects( ).remove( EF_NO_INTERP );
}

bool __fastcall hooks::player::should_skip_animation_frame::fn( c_base_animating* ecx, void* edx ) {
	return false;
}

void __fastcall hooks::player::clamp_bones_in_bbox::fn( c_cs_player* ecx, void* edx, void* mat, int mask ) {
	if ( !ecx || !ecx->is_alive( ) || !ecx->is_player( ) )
		return original( ecx, edx, mat, mask );

	const auto collideable = ecx->get_collideable( );
	if ( !collideable )
		return original( ecx, edx, mat, mask );

	if ( !globals::allow_setup_bones( ) )
		return original( ecx, edx, mat, mask );

	const auto backup_curtime = interfaces::m_global_vars->m_cur_time;

	if ( ecx == globals::m_local ) {
		interfaces::m_global_vars->m_cur_time = TICKS_TO_TIME( globals::m_local->get_tick_base( ) );

		original( ecx, edx, mat, mask ); 

		interfaces::m_global_vars->m_cur_time = backup_curtime;
	}
	else {
		interfaces::m_global_vars->m_cur_time = ecx->get_sim_time( );

		original( ecx, edx, mat, mask );

		interfaces::m_global_vars->m_cur_time = backup_curtime;
	}
}