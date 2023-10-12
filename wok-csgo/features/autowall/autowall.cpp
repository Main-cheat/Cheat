#include "autowall.h"

float c_autowall::get_damage( c_cs_player* attacker, const vec3_t& point, fire_bullet_data_t* data_out ) {
	const vec3_t shoot_pos = attacker->get_eye_pos( );

	fire_bullet_data_t data = { };
	data.m_position = shoot_pos;
	data.m_direction = ( point - shoot_pos ).normalized( );

	if ( c_base_combat_weapon* weapon = attacker->get_active_weapon( ); weapon == nullptr || !simulate_fire_bullet( attacker, weapon, data ) )
		return -1.0f;

	if ( data_out != nullptr )
		*data_out = data;

	return data.m_current_damage;
}

void c_autowall::scale_damage( const int hit_group, c_cs_player* player, const float weapon_armor_ratio, const float weapon_headshot_multiplier, float* damage_to_scale ) {
	static const auto mp_damage_headshot_only = interfaces::m_cvar_system->find_var( FNV1A( "mp_damage_headshot_only" ) );
	
	if ( hit_group != HITGROUP_HEAD && mp_damage_headshot_only->get_bool( ) ) {
		*damage_to_scale = 0.0f;
		return;
	}

	static const auto mp_damage_scale_ct_head = interfaces::m_cvar_system->find_var( FNV1A( "mp_damage_scale_ct_head" ) );
	static const auto mp_damage_scale_t_head = interfaces::m_cvar_system->find_var( FNV1A( "mp_damage_scale_t_head" ) );
	static const auto mp_damage_scale_ct_body = interfaces::m_cvar_system->find_var( FNV1A( "mp_damage_scale_ct_body" ) );
	static const auto mp_damage_scale_t_body = interfaces::m_cvar_system->find_var( FNV1A( "mp_damage_scale_t_body" ) );

	const bool heavy_armor = player->has_heavy_armor( );

	float head_damage_scale = ( player->get_team( ) == 3 ? mp_damage_scale_ct_head->get_float( ) : mp_damage_scale_t_head->get_float( ) );
	const float body_damage_scale = ( player->get_team( ) == 3 ? mp_damage_scale_ct_body->get_float( ) : mp_damage_scale_t_body->get_float( ) );

	if ( heavy_armor )
		head_damage_scale *= 0.5f;

	switch ( hit_group ) {
		case HITGROUP_HEAD:
			*damage_to_scale *= weapon_headshot_multiplier * head_damage_scale;
			break;
		case HITGROUP_CHEST:
		case HITGROUP_LEFT_ARM:
		case HITGROUP_RIGHT_ARM:
		case HITGROUP_NECK:
			*damage_to_scale *= body_damage_scale;
			break;
		case HITGROUP_STOMACH:
			*damage_to_scale *= 1.25f * body_damage_scale;
			break;
		case HITGROUP_LEFT_LEG:
		case HITGROUP_RIGHT_LEG:
			*damage_to_scale *= 0.75f * body_damage_scale;
			break;
		default:
			break;
	}

	const auto is_armored = [ & ]( int box ) -> bool {
		bool armored = false;

		if ( player->get_armor_value( ) > 0 ) {
			switch ( box ) {
				case HITGROUP_GENERIC:
				case HITGROUP_CHEST:
				case HITGROUP_STOMACH:
				case HITGROUP_LEFT_ARM:
				case HITGROUP_RIGHT_ARM:
				case HITGROUP_NECK:
					armored = true;
					break;
				case HITGROUP_HEAD:
					if ( player->has_helmet( ) )
						armored = true;
					[[fallthrough]];
				case HITGROUP_LEFT_LEG:
				case HITGROUP_RIGHT_LEG:
					if ( player->has_heavy_armor( ) )
						armored = true;
					break;
				default:
					break;
			}
		}

		return armored;
	};

	if ( is_armored( hit_group ) ) {
		// @ida CCSPlayer::OnTakeDamage(): server.dll -> "80 BF ? ? ? ? ? F3 0F 10 5C 24 ? F3 0F 10 35"

		const int armor = player->get_armor_value( );
		float heavy_armor_bonus = 1.0f, armor_bonus = 0.5f, armor_ratio = weapon_armor_ratio * 0.5f;

		if ( heavy_armor ) {
			heavy_armor_bonus = 0.25f;
			armor_bonus = 0.33f;
			armor_ratio *= 0.20f;
		}

		float damage_to_health = *damage_to_scale * armor_ratio;
		if ( const float damage_to_armor = ( *damage_to_scale - damage_to_health ) * ( heavy_armor_bonus * armor_bonus ); damage_to_armor > static_cast< float >( armor ) )
			damage_to_health = *damage_to_scale - static_cast< float >( armor ) / armor_bonus;

		*damage_to_scale = damage_to_health;
	}
}

bool c_autowall::simulate_fire_bullet( c_cs_player* attacker, c_base_combat_weapon* weapon, fire_bullet_data_t& data ) {
	const c_cs_weapon_data* weapon_data = weapon->get_cs_weapon_data( );
	if ( weapon_data == nullptr )
		return false;

	float max_range = weapon_data->m_range;

	data.m_penetrate_count = 4;
	data.m_current_damage = static_cast< float >( weapon_data->m_damage );

	float trace_length = 0.0f;
	c_trace_filter_simple filter( attacker );

	while ( data.m_penetrate_count > 0 && data.m_current_damage >= 1.0f ) {
		// max bullet range
		max_range -= trace_length;

		// end position of bullet
		const vec3_t end = data.m_position + data.m_direction * max_range;

		ray_t ray( data.m_position, end );
		interfaces::m_trace_system->trace_ray( ray, MASK_SHOT_HULL | CONTENTS_HITBOX, filter.get( ), &data.m_enter_trace );

		// check for player hitboxes extending outside their collision bounds
		clip_trace_to_players( data.m_position, end + data.m_direction * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, filter.get( ), &data.m_enter_trace );

		const surfacedata_t* enter_surface_data = interfaces::m_surface_data->get_surface_data( data.m_enter_trace.m_surface.m_surface_props );
		const float enter_penetration_modifier = enter_surface_data->m_game.m_penetration_modifier;

		// we didn't hit anything, stop tracing shoot
		if ( data.m_enter_trace.m_fraction == 1.0f )
			break;

		// calculate the damage based on the distance the bullet traveled
		trace_length += data.m_enter_trace.m_fraction * max_range;
		data.m_current_damage *= std::powf( weapon_data->m_range_modifier, trace_length / 500.f );

		// check is actually can shoot through
		if ( trace_length > 3000.f || enter_penetration_modifier < 0.1f )
			break;

		// check did we hit the player
		// @todo: here shouldn't be enemy check?
		if ( data.m_enter_trace.m_hitgroup != HITGROUP_GENERIC && data.m_enter_trace.m_hitgroup != HITGROUP_GEAR && attacker->is_enemy( static_cast< c_cs_player* >( data.m_enter_trace.m_hit_entity ) ) ) {
			scale_damage( data.m_enter_trace.m_hitgroup, static_cast< c_cs_player* >( data.m_enter_trace.m_hit_entity ), weapon_data->m_armor_ratio, weapon_data->m_head_shot_multiplier, &data.m_current_damage );
			return true;
		}

		// check if the bullet can no longer continue penetrating materials
		if ( handle_bullet_penetration( attacker, weapon_data, enter_surface_data, data ) )
			break;
	}

	return false;
}

bool c_autowall::trace_to_exit( const c_game_trace& enter_trace, c_game_trace& exit_trace, const vec3_t& position, const vec3_t& direction, i_handle_entity* clip_player ) {
	static const auto sv_clip_penetration_traces_to_players = interfaces::m_cvar_system->find_var( FNV1A( "sv_clip_penetration_traces_to_players" ) );
	
	float distance = 0.0f;
	int start_contents = 0;

	while ( distance <= 90.0f ) {
		// add extra distance to our ray
		distance += 4.0f;

		// multiply the direction vector to the distance so we go outwards, add our position to it
		vec3_t end = position + direction * distance;

		if ( start_contents == 0 )
			start_contents = interfaces::m_trace_system->get_point_contents( end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );

		if ( const int current_contents = interfaces::m_trace_system->get_point_contents( end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );
			 !( current_contents & MASK_SHOT_HULL ) || ( ( current_contents & CONTENTS_HITBOX ) && current_contents != start_contents ) ) {
			// setup our end position by deducting the direction by the extra added distance
			const vec3_t start = end - ( direction * 4.0f );

			// trace ray to world
			ray_t ray_to_world( end, start );
			interfaces::m_trace_system->trace_ray( ray_to_world, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exit_trace );

			if ( sv_clip_penetration_traces_to_players->get_bool( ) ) // @note: convar check was added since 07.07.2021 (version 1.37.9.5, build 1304)
			{
				c_trace_filter_simple filter( clip_player );
				clip_trace_to_players( start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, filter.get( ), &exit_trace, -60.f );
			}

			// check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if ( exit_trace.m_start_solid && ( exit_trace.m_surface.m_flags & SURF_HITBOX ) ) {
				// trace ray to entity @note: added since 2021Q4-2022Q1 updates, also skip one more specific entity
				ray_t ray( end, position );
				c_trace_filter_skip_two_entities filter( exit_trace.m_hit_entity, clip_player );

				interfaces::m_trace_system->trace_ray( ray, MASK_SHOT_HULL, filter.get( ), &exit_trace );

				if ( exit_trace.did_hit( ) && !exit_trace.m_start_solid ) {
					end = exit_trace.m_end_pos;
					return true;
				}
			}
			else if ( !exit_trace.did_hit( ) || exit_trace.m_start_solid ) {
				// check did hit non world entity
				if ( exit_trace.m_hit_entity != nullptr && exit_trace.m_hit_entity->get_index( ) != 0 &&
					 ( ( c_cs_player* )exit_trace.m_hit_entity )->is_breakable( ) ) {
					exit_trace = enter_trace;
					exit_trace.m_end_pos = end + direction;
					return true;
				}
			}
			else {
				// check did hit breakable two-way entity (barrel, box, etc)
				if ( ( ( c_cs_player* ) enter_trace.m_hit_entity )->is_breakable( ) && ( ( c_cs_player* ) exit_trace.m_hit_entity )->is_breakable( ) )
					return true;

				if ( ( enter_trace.m_surface.m_flags & SURF_NODRAW ) || ( !( exit_trace.m_surface.m_flags & SURF_NODRAW ) && exit_trace.m_plane.m_normal.dot_product( direction ) <= 1.0f ) ) {
					end -= direction * ( exit_trace.m_fraction * 4.0f );
					return true;
				}
			}
		}
	}

	return false;
}

bool c_autowall::handle_bullet_penetration( c_cs_player* local, const c_cs_weapon_data* weapon_data, const surfacedata_t* enter_surface_data, fire_bullet_data_t& data ) {
	static const auto ff_damage_reduction_bullets = interfaces::m_cvar_system->find_var( FNV1A( "ff_damage_reduction_bullets" ) );
	static const auto mp_teammates_are_enemies = interfaces::m_cvar_system->find_var( FNV1A( "mp_teammates_are_enemies" ) );
	static const auto ff_damage_bullet_penetration = interfaces::m_cvar_system->find_var( FNV1A( "ff_damage_bullet_penetration" ) );
	const auto enter_material = enter_surface_data->m_game.m_material;

	// check is not penetrable material
	if ( data.m_penetrate_count == 0 && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS && !( data.m_enter_trace.m_surface.m_flags & SURF_NODRAW ) )
		return true;

	// check is weapon can't penetrate
	if ( weapon_data->m_penetration <= 0.0f || data.m_penetrate_count <= 0 )
		return true;

	c_game_trace exit_trace = { };
	if ( !trace_to_exit( data.m_enter_trace, exit_trace, data.m_enter_trace.m_end_pos, data.m_direction, local )
		 && !( interfaces::m_trace_system->get_point_contents( data.m_enter_trace.m_end_pos, MASK_SHOT_HULL, nullptr ) & MASK_SHOT_HULL ) )
		return true;

	const surfacedata_t* exit_surface_data = interfaces::m_surface_data->get_surface_data( exit_trace.m_surface.m_surface_props );
	const auto exit_material = exit_surface_data->m_game.m_material;

	const float enter_penetration_modifier = enter_surface_data->m_game.m_penetration_modifier;
	const float exit_penetration_modifier = exit_surface_data->m_game.m_penetration_modifier;

	float damage_lost_modifier = 0.16f;
	float penetration_modifier = 0.0f;

	// [side change] we only handle new penetration method and expect that 'sv_penetration_type' is 1

	if ( enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS ) {
		damage_lost_modifier = 0.05f;
		penetration_modifier = 3.0f;
	}
	else if ( ( ( data.m_enter_trace.m_contents >> 3 ) & CONTENTS_SOLID ) || ( ( data.m_enter_trace.m_surface.m_flags >> 7 ) & SURF_LIGHT ) )
		penetration_modifier = 1.0f;
	else if ( enter_material == CHAR_TEX_FLESH && ff_damage_reduction_bullets->get_float( ) == 0.0f && data.m_enter_trace.m_hit_entity != nullptr
		 && ( ( c_cs_player* )data.m_enter_trace.m_hit_entity )->is_player( ) && !mp_teammates_are_enemies->get_bool( )
		 && local->get_team( ) == ( ( c_cs_player* ) data.m_enter_trace.m_hit_entity )->get_team( ) ) {

		const float penetrate_damage = ff_damage_bullet_penetration->get_float( );

		if ( penetrate_damage == 0.0f )
			return true;

		// shoot through teammates
		penetration_modifier = penetrate_damage;
	}
	else
		penetration_modifier = ( enter_penetration_modifier + exit_penetration_modifier ) * 0.5f;

	if ( enter_material == exit_material ) {
		if ( exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD )
			penetration_modifier = 3.0f;
		else if ( exit_material == CHAR_TEX_PLASTIC )
			penetration_modifier = 2.0f;
	}

	// [side change] used squared length, because game wastefully converts it back to squared
	const float trace_distance_sqr = ( exit_trace.m_end_pos - data.m_enter_trace.m_end_pos ).length_sqr( );

	// penetration modifier
	const float modifier = ( penetration_modifier > 0.0f ? 1.0f / penetration_modifier : 0.0f );

	// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	const float lost_damage = ( data.m_current_damage * damage_lost_modifier + ( weapon_data->m_penetration > 0.0f ? 3.75f / weapon_data->m_penetration : 0.0f ) * ( modifier * 3.0f ) ) + ( ( modifier * trace_distance_sqr ) / 24.0f );

	// reduce damage power each time we hit something other than a grate
	data.m_current_damage -= std::max( lost_damage, 0.0f );

	// check do we still have enough damage to deal?
	if ( data.m_current_damage < 1.0f )
		return true;

	data.m_position = exit_trace.m_end_pos;
	--data.m_penetrate_count;
	return false;
}

void c_autowall::clip_trace_to_players( const vec3_t& abs_start, const vec3_t& abs_end, const int contents_mask, i_trace_filter* filter, c_game_trace* trace, const float min_range, const float max_range ) {
	c_game_trace new_trace = { };
	float smallest_fraction = trace->m_fraction;

	const ray_t ray( abs_start, abs_end );
	for ( int i = 1; i <= interfaces::m_global_vars->m_max_clients; i++ ) {
		c_cs_player* player = ( c_cs_player *)interfaces::m_entity_list->get_client_entity( i );

		if ( player == nullptr || !player->is_alive( ) || player->is_dormant( ) )
			continue;

		if ( filter != nullptr && !filter->should_hit_entity( player, contents_mask ) )
			continue;

		const vec3_t& position = player->world_space_center( );

		const vec3_t point = position - abs_start;
		vec3_t direction = abs_end - abs_start;
		const float flLength = direction.normalize( ).length( );
		const float flRangeAlong = direction.dot_product( point );

		// calculate distance to ray
		float flRange;
		if ( flRangeAlong < 0.0f )
			// off start point
			flRange = -point.length( );
		else if ( flRangeAlong > flLength )
			// off end point
			flRange = -( position - abs_end ).length( );
		else
			// within ray bounds
			flRange = ( position - ( direction * flRangeAlong + abs_start ) ).length( );

		if ( flRange < min_range || flRange > max_range )
			continue;

		interfaces::m_trace_system->clip_ray_to_entity( ray, contents_mask | CONTENTS_HITBOX, player, &new_trace );
		if ( trace->m_fraction < smallest_fraction ) {
			*trace = new_trace;
			smallest_fraction = new_trace.m_fraction;
		}
	}
}