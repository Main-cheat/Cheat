#include "antiaim.h"

void c_antiaim::on_pre_prediction( ) { 
	m_data.m_max_choke = 14;
	if ( ( *interfaces::m_game_rules )->get_freeze_period( ) )
		m_data.m_max_choke = 1;
	else if ( ( *interfaces::m_game_rules )->is_valve_ds( ) )
		m_data.m_max_choke = 6;

	/// fakelag
	globals::m_packet = interfaces::m_client_state->m_choked_commands >= m_data.m_max_choke;

	if ( globals::m_packet )
		m_data.m_switch_jitter = !m_data.m_switch_jitter;
}

void c_antiaim::on_create_move( ) {
	if ( globals::m_weapon.is_shooting( ) 
		 && ( globals::m_weapon.get( )->get_item_definition_index( ) != WEAPON_C4
		 && globals::m_weapon.get( )->get_item_definition_index( ) != WEAPON_MEDISHOT ) )
		return;

	if ( globals::m_cur_cmd->m_buttons.has( IN_USE ) )
		return;

	if ( ( *interfaces::m_game_rules )->get_freeze_period( ) )
		return;

	globals::m_cur_cmd->m_view_angles = qangle_t( get_pitch( ), get_yaw( ) + get_fake_yaw( ), get_roll( ) );
}

float c_antiaim::get_pitch( ) {
	return 90.f;
}

float c_antiaim::get_yaw( ) {
	return globals::m_cur_cmd->m_view_angles.y + 180.f;
}

float c_antiaim::get_fake_yaw( ) {
	if ( globals::m_packet )
		return 0.f;

	return 60.f;
}

float c_antiaim::get_roll( ) {
	if ( globals::m_local->get_velocity( ).length( ) > 5.f || globals::m_local->get_velocity( ).z != 0.f )
		return 0.f;

	return 0.f;
}