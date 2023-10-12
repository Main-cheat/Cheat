#include "../globals.h"

c_lag_record::c_lag_record( c_cs_player* player ) {
	m_player = player;
	if ( !player || !player->is_alive( ) )
		return;

	m_origin = player->get_origin( );
	m_abs_origin = player->get_abs_origin( );
	m_velocity = player->get_velocity( );
	m_abs_velocity = player->get_abs_velocity( );

	if ( player->get_collideable( ) ) {
		m_mins = player->get_collideable( )->obb_mins( );
		m_maxs = player->get_collideable( )->obb_maxs( );
	}

	m_angles = player->get_abs_angles( );
	m_eye_angles = player->get_eye_angles( );

	m_flags = player->get_flags( );
	m_eflags = player->get_eflags( );

	m_sim_time = player->get_sim_time( );
	m_old_sim_time = player->get_old_sim_time( );

	m_sim_ticks = TIME_TO_TICKS( m_sim_time - m_old_sim_time );

	m_duck_amount = player->get_duck_amount( );

	m_jumping = m_flags.has( FL_ONGROUND );
	m_landing = false;
	m_dormant = player->is_dormant( );

	std::memcpy( m_layers.data( ), player->get_anim_layers( ).data( ), sizeof( anim_layers_t ) );
	std::memcpy( m_matrix.data( ), player->get_bone_cache( )->m_cached_bones, sizeof( bone_matrix_t ) );

	for ( int i = 0; i < 3; i++ ) {
		std::memcpy( m_anim_data.m_anim_layers.at( i ).data( ), m_layers.data( ), sizeof( anim_layers_t ) );
		std::memcpy( m_anim_data.m_bone_matrix.at( i ).data( ), m_matrix.data( ), sizeof( bone_matrix_t ) );
	}

	m_valid = m_sim_ticks >= 0 && m_sim_ticks <= 16;
	m_breaking_lc = m_sim_ticks < 0;

	m_sim_ticks = std::max( 1, m_sim_ticks );
}

bool c_local_weapon::can_fire( int extra_ticks ) {
	if ( !m_data || !m_weapon || !globals::m_local )
		return false;

	if ( globals::m_local->get_flags( ).has( FL_FROZEN ) )
		return false;

	if ( globals::m_local->is_defusing( ) )
		return false;

	const auto layer = globals::m_local->get_anim_layers( ).at( 1u );
	if ( layer.m_owner == globals::m_local ) {
		if ( globals::m_local->get_sequence_activity( layer.m_sequence ) == 967 && layer.m_weight != 0.f )
			return false;
	}

	if ( m_data->m_weapon_type >= WEAPON_TYPE_PISTOL && m_data->m_weapon_type <= WEAPON_TYPE_MACHINE_GUN && m_weapon->get_ammo( ) <= 0 )
		return false;

	const float current_time = TICKS_TO_TIME( globals::m_local->get_tick_base( ) - extra_ticks );
	if ( current_time < globals::m_local->get_next_attack( ) )
		return false;

	if ( m_weapon->has_burst_mode( ) && m_weapon->burst_shots_remain( ) > 0 )
		if ( current_time >= m_weapon->get_next_burst_shot( ) )
			return true;

	if ( current_time < m_weapon->get_next_primary_attack( ) )
		return false;

	if ( m_weapon->get_item_definition_index( ) != WEAPON_R8_REVOLVER )
		return true;

	return current_time >= m_weapon->get_post_pone_fire_ready_time( );
}

bool c_local_weapon::is_shooting( ) {
	if ( !m_data || !m_weapon || !globals::m_local )
		return false;

	if ( globals::m_local->get_flags( ).has( FL_FROZEN ) )
		return false;

	if ( m_data->m_weapon_type == WEAPON_TYPE_GRENADE )
		return !m_weapon->get_pin_pulled( ) && m_weapon->get_throw_time( ) > 0.f && m_weapon->get_throw_time( ) < TICKS_TO_TIME( globals::m_local->get_tick_base( ) );

	bool double_attack = globals::m_cur_cmd->m_buttons & IN_ATTACK || globals::m_cur_cmd->m_buttons & IN_ATTACK2;
	if ( m_data->m_weapon_type == WEAPON_TYPE_KNIFE || m_weapon->get_item_definition_index( ) == WEAPON_R8_REVOLVER )
		return double_attack && this->can_fire( );

	return globals::m_cur_cmd->m_buttons & IN_ATTACK && this->can_fire( );
}

std::wstring c_base_combat_weapon::get_name() {
	const auto weapon_data = get_cs_weapon_data();
	if (!weapon_data)
		return L"";

	return interfaces::m_localize->find_safe(weapon_data->m_hud_name);
}

c_cs_weapon_data* c_base_combat_weapon::get_cs_weapon_data() { return interfaces::m_weapon_system->get_cs_weapon_data(get_item_definition_index()); }

player_info_t c_cs_player::get_info() {
	auto ret = player_info_t();

	interfaces::m_engine->get_player_info(get_index(), &ret);

	return ret;
}

bool c_cs_player::is_enemy(c_cs_player* from) {
	if (this == from)
		return false;

	if (interfaces::m_game_types->get_cur_game_type() == GAME_TYPE_FREEFORALL)
		return get_survival_team() != from->get_survival_team();

	static const auto mp_teammates_are_enemies = interfaces::m_cvar_system->find_var(FNV1A("mp_teammates_are_enemies"));
	if (mp_teammates_are_enemies->get_bool())
		return true;

	return get_team() != from->get_team();
}

bool c_cs_player::is_friendly( c_cs_player* from ) {
	if ( this == from )
		return false;

	if ( is_enemy( from ) )
		return false;

	return true;
}

int c_base_animating::get_sequence_activity(int sequence) {
	const auto model = get_model();
	if (!model)
		return -1;

	const auto hdr = interfaces::m_model_info->get_studio_model(model);
	if (!hdr)
		return -1;

	static const auto get_sequence_activity_fn = SIG("client.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83").cast<int(__fastcall*)(void*, studiohdr_t*, int)>();

	return get_sequence_activity_fn(this, hdr, sequence);
}

c_base_combat_weapon* c_base_combat_character::get_active_weapon() {
	const auto handle = get_active_weapon_handle();
	if (!handle.is_valid())
		return nullptr;

	return reinterpret_cast<c_base_combat_weapon*>(handle.get());
}

vec3_t c_cs_player::get_hitbox_pos( int hitbox_id ) {
	auto model_ptr = get_studio_hdr( );
	if ( !model_ptr )
		return { 0, 0, 0 };

	auto studio_hdr = model_ptr->m_studio_hdr;
	if ( !studio_hdr )
		return { 0, 0, 0 };

	auto hitbox = studio_hdr->get_hitbox_set( 0 )->get_hitbox( hitbox_id );
	if ( !hitbox )
		return { 0, 0, 0 };

	matrix3x4_t bone_matrix[ 256 ];
	if ( !setup_bones( bone_matrix, 256, BONE_FLAG_USED_BY_HITBOX, 0.0f ) )
		return { 0, 0, 0 };

	vec3_t min = hitbox->m_obb_min.transform( bone_matrix[ hitbox->m_bone ] );
	vec3_t max = hitbox->m_obb_max.transform( bone_matrix[ hitbox->m_bone ] );

	return ( min + max ) / 2;
}

vec3_t c_cs_player::get_hitbox_pos( int hitbox_id, matrix3x4_t* best_matrix ) {
	auto model_ptr = get_studio_hdr( );
	if ( !model_ptr )
		return { 0, 0, 0 };

	auto studio_hdr = model_ptr->m_studio_hdr;
	if ( !studio_hdr )
		return { 0, 0, 0 };

	auto hitbox = studio_hdr->get_hitbox_set( 0 )->get_hitbox( hitbox_id );
	if ( !hitbox )
		return { 0, 0, 0 };

	if ( !best_matrix )
		if ( !setup_bones( best_matrix, 256, BONE_FLAG_USED_BY_HITBOX, 0.0f ) )
			return { 0, 0, 0 };

	vec3_t min = hitbox->m_obb_min.transform( best_matrix[ hitbox->m_bone ] );
	vec3_t max = hitbox->m_obb_max.transform( best_matrix[ hitbox->m_bone ] );

	return ( min + max ) / 2;
}