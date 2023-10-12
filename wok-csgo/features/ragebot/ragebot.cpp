#include "ragebot.h"
#include "../animations/animations.h"
#include "../autowall/autowall.h"

struct backup_data_t {
	vec3_t m_origin {}, m_abs_origin {};
	bone_matrix_t m_matrix {};
	anim_layers_t m_layers {};
	int m_tick { -1 };
};
std::array< backup_data_t, 65 > m_backup_data;

void setup_record( c_cs_player* player, c_lag_record* record ) {
	m_backup_data.at( player->get_index( ) ).m_tick = globals::m_cur_cmd->m_tick_count;
	m_backup_data.at( player->get_index( ) ).m_origin = player->get_origin( );
	m_backup_data.at( player->get_index( ) ).m_abs_origin = player->get_abs_origin( );

	std::memcpy( m_backup_data.at( player->get_index( ) ).m_layers.data( ), player->get_anim_layers( ).data( ), sizeof( anim_layers_t ) );
	std::memcpy( m_backup_data.at( player->get_index( ) ).m_matrix.data( ), player->get_bone_cache( )->m_cached_bones, sizeof( bone_matrix_t ) );

	player->get_origin( ) = record->m_origin;
	player->set_abs_origin( record->m_origin );
	std::memcpy( player->get_anim_layers( ).data( ), record->m_layers.data( ), sizeof( anim_layers_t ) );
	std::memcpy( player->get_bone_cache( )->m_cached_bones, record->m_matrix.data( ), sizeof( bone_matrix_t ) );
}

void reset_record( c_cs_player* player ) {
	if ( m_backup_data.at( player->get_index( ) ).m_tick != globals::m_cur_cmd->m_tick_count )
		return;

	player->get_origin( ) = m_backup_data.at( player->get_index( ) ).m_origin;
	player->set_abs_origin( m_backup_data.at( player->get_index( ) ).m_abs_origin );

	std::memcpy( player->get_anim_layers( ).data( ), m_backup_data.at( player->get_index( ) ).m_layers.data( ), sizeof( anim_layers_t ) );
	std::memcpy( player->get_bone_cache( )->m_cached_bones, m_backup_data.at( player->get_index( ) ).m_matrix.data( ), sizeof( bone_matrix_t ) );

	m_backup_data.at( player->get_index( ) ).m_tick = -1;
}

void c_ragebot::on_create_move( ) {
	if ( !globals::m_local->is_alive( ) || !globals::m_weapon.get( ) )
		return;

	target_data_t target;
	for ( auto& player : listener->get_players( ) ) {
		if ( !player || !player->is_alive( ) || player->is_immune( ) || !player->is_enemy( globals::m_local ) )
			continue;

		c_lag_record* record = &animations->get_first_available( player );
		c_lag_record* bt_record = &animations->get_last_available( player );

		c_lag_record backup( player );

		target.m_player = player;
		if ( animations->valid_time( record ) ) {
			setup_record( player, record );
			scan_player( target, record );
			if ( target.m_scan_data.m_damage >= 85.f ) {
				target.m_record = record;
				reset_record( player );
				break;
			}
			reset_record( player );
		}

		if ( !animations->valid_time( bt_record ) )
			continue;

		setup_record( player, record );
		scan_player( target, bt_record );
		if ( target.m_scan_data.m_damage < 85.f ) {
			reset_record( player );
			continue;
		}

		target.m_record = record;
		reset_record( player );
		break;
	}

	if ( !target.m_player || !animations->valid_time( target.m_record ) || target.m_scan_data.m_damage < 85.f )
		return;

	setup_record( target.m_player, target.m_record );
	if ( globals::m_weapon.can_fire( ) ) {
		if ( !can_hit_point( globals::m_local->get_eye_pos( ), target.m_scan_data.m_point ) )
			return reset_record( target.m_player );

		globals::m_cur_cmd->m_tick_count = TIME_TO_TICKS( target.m_record->m_sim_time + animations->lerp_time( ) );
		globals::m_cur_cmd->m_view_angles = ( target.m_scan_data.m_point - globals::m_local->get_eye_pos( ) ).angle( ).sanitize( );
		globals::m_cur_cmd->m_buttons = IN_ATTACK;
	}
	reset_record( target.m_player );
}

bool c_ragebot::can_hit_point( vec3_t start, vec3_t point ) {
	return true;
}

void c_ragebot::scan_player( target_data_t& data, c_lag_record* record ) {
	if ( !data.m_player || !record )
		return;

	std::vector< std::tuple< vec3_t, float > > scan_data;
	for ( auto& hitbox : { HITBOX_HEAD, HITBOX_CHEST, HITBOX_PELVIS, HITBOX_STOMACH } ) {
		const vec3_t point = data.m_player->get_hitbox_pos( hitbox, record->m_matrix.data( ) );
		const float damage_to_point = autowall->get_damage( globals::m_local, point );
		if ( damage_to_point < 85.f )
			continue;

		scan_data.push_back( std::make_tuple( point, damage_to_point ) );
	}

	std::sort( scan_data.begin( ), scan_data.end( ), [ & ]( std::tuple< vec3_t, float >& one, std::tuple< vec3_t, float >& two ) {
		return std::get< float >( one ) < std::get< float >( two );
	} );

	if ( scan_data.empty( ) )
		return;

	data.m_scan_data.m_point = std::get< vec3_t >( scan_data.back( ) );
	data.m_scan_data.m_damage = std::get< float >( scan_data.back( ) );
}