#include "animations.h"

void c_animations::on_frame( ) {
	for ( auto& player : listener->get_players( ) ) {
		if ( !player->is_alive( ) || !player->is_enemy( globals::m_local ) || player->is_immune( ) ) {
			m_player_records.at( player->get_index( ) ).clear( );
			continue;
		}

		auto records = m_player_records.at( player->get_index( ) );

		/// remove invalid records
		for ( auto it = records.rbegin( ); it != records.rend( ); )
			it = valid_time( &*it ) ? next( it ) : decltype( it ) { records.erase( next( it ).base( ) ) };
		
		c_lag_record* previous = nullptr;
		if ( m_player_records.at( player->get_index( ) ).size( ) > 1 )
			previous = &m_player_records.at( player->get_index( ) ).front( );

		c_lag_record* record = &m_player_records.at( player->get_index( ) ).emplace_front( player );
		if ( previous ) {
			/// check for fake server update
			if ( record->m_layers.at( ANIMATION_LAYER_ALIVELOOP ).m_cycle == previous->m_layers.at( ANIMATION_LAYER_ALIVELOOP ).m_cycle ) {
				player->get_sim_time( ) = player->get_old_sim_time( );
				continue;
			}

			/// check for lc breaking
			if ( ( record->m_abs_origin - previous->m_abs_origin ).length_sqr( ) > 4096.f || record->m_sim_time < record->m_old_sim_time ) {
				std::for_each( m_player_records.at( player->get_index( ) ).begin( ), m_player_records.at( player->get_index( ) ).end( ), [ ]( c_lag_record& record ) {
					record.m_breaking_lc = true;
				} );
				record->m_breaking_lc = true;
			}

			/// check is player left dormant
			if ( !record->m_dormant && previous->m_dormant ) {
				std::for_each( m_player_records.at( player->get_index( ) ).begin( ), m_player_records.at( player->get_index( ) ).end( ), [ ]( c_lag_record& record ) {
					record.m_valid = false;
				} );
			}
		}

		/// the simulation time of the last player does not change, it does not need to be updated
		if ( record->m_sim_time == record->m_old_sim_time )
			continue;

		c_lag_record backup( player );
		backup.apply( player );

		/// update player animations
		this->update_player_animations( player, record, previous );
		player->mark_as_interpolated( true );

		globals::allow_setup_bones( ) = true;
		player->setup_bones( record->m_matrix.data( ), 256u, BONE_FLAG_USED_BY_ANYTHING, record->m_sim_time );
		globals::allow_setup_bones( ) = false;

		std::memcpy( record->m_matrix.data( ), player->get_bone_cache( )->m_cached_bones, sizeof( matrix3x4_t ) * player->get_bone_cache( )->m_cached_bones_count );

		backup.restore( player );
		
		/// remove old records
		while ( m_player_records.at( player->get_index( ) ).size( ) > 32 )
			m_player_records.at( player->get_index( ) ).pop_back( );
	}
}

void c_animations::update_player_animations( c_cs_player* player, c_lag_record* record, c_lag_record* previous ) {
	const auto animstate = player->get_anim_state( );
	if ( !animstate ) return;

	if ( record->m_sim_ticks > 1 ) {
		float update_time = record->m_sim_time - interfaces::m_global_vars->m_interval_per_tick;
		if ( record->m_flags.has( FL_ONGROUND ) ) {
			animstate->m_landing = false;
			animstate->m_on_ground = true;

			float land_time = 0.f;
			if ( record->m_layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_cycle > 0.0f && record->m_layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_playback_rate > 0.0f ) {
				int act = player->get_sequence_activity( record->m_layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_sequence );

				if ( act == 988 || act == 989 ) {
					land_time = record->m_layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_cycle / record->m_layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ].m_playback_rate;

					if ( land_time > 0.f )
						update_time = record->m_sim_time - land_time;
				}
			}
		}
		else {
			float jump_time = 0.f;
			if ( record->m_layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_cycle > 0.0f && record->m_layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_playback_rate > 0.0f ) {
				int act = player->get_sequence_activity( record->m_layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_sequence );

				if ( act == 985 ) {
					jump_time = record->m_layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_cycle / record->m_layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ].m_playback_rate;

					if ( jump_time > 0.f )
						update_time = record->m_sim_time - jump_time;
				}
			}

			animstate->m_on_ground = false;
			animstate->m_last_cur_time = jump_time;
		}

		animstate->m_feet_cycle = record->m_layers[ ANIMATION_LAYER_MOVEMENT_MOVE ].m_cycle;
		animstate->m_feet_weight = record->m_layers[ ANIMATION_LAYER_MOVEMENT_MOVE ].m_weight;
		animstate->m_acceleration_weight = record->m_layers[ ANIMATION_LAYER_LEAN ].m_weight;

		std::memcpy( player->get_anim_layers( ).data( ), record->m_layers.data( ), sizeof( anim_layers_t ) );
	}

	const float cur_time = interfaces::m_global_vars->m_cur_time;
	const float real_time = interfaces::m_global_vars->m_real_time;
	const float frame_time = interfaces::m_global_vars->m_frame_time;
	const float absolute_frame_time = interfaces::m_global_vars->m_absolute_frame_time;
	const float frame_count = interfaces::m_global_vars->m_frame_count;
	const float tick_count = interfaces::m_global_vars->m_tick_count;
	const float interp_amount = interfaces::m_global_vars->m_interpolation_amount;

	interfaces::m_global_vars->m_interpolation_amount = 0.f;
	if ( record->m_sim_ticks > 1 ) {
		for ( int sim_tick = 1; sim_tick <= record->m_sim_ticks; sim_tick++ ) {
			float sim_time = record->m_sim_time + TICKS_TO_TIME( sim_tick );

			interfaces::m_global_vars->m_cur_time = sim_time;
			interfaces::m_global_vars->m_real_time = sim_time;
			interfaces::m_global_vars->m_frame_time = interfaces::m_global_vars->m_interval_per_tick;
			interfaces::m_global_vars->m_absolute_frame_time = interfaces::m_global_vars->m_interval_per_tick;
			interfaces::m_global_vars->m_frame_count = TIME_TO_TICKS( sim_time );
			interfaces::m_global_vars->m_tick_count = TIME_TO_TICKS( sim_time );

			const bool backup_client_side_animation = player->get_client_side_animation( );
			globals::allow_anim_update( ) = player->get_client_side_animation( ) = true;

			player->update_client_side_animation( );

			globals::allow_anim_update( ) = false;
			player->get_client_side_animation( ) = backup_client_side_animation;

			interfaces::m_global_vars->m_cur_time = cur_time;
			interfaces::m_global_vars->m_real_time = real_time;
			interfaces::m_global_vars->m_frame_time = frame_time;
			interfaces::m_global_vars->m_absolute_frame_time = absolute_frame_time;
			interfaces::m_global_vars->m_frame_count = frame_count;
			interfaces::m_global_vars->m_tick_count = tick_count;
			interfaces::m_global_vars->m_interpolation_amount = interp_amount;
		}
	}
	else {
		interfaces::m_global_vars->m_cur_time = record->m_sim_time;
		interfaces::m_global_vars->m_real_time = record->m_sim_time;
		interfaces::m_global_vars->m_frame_time = interfaces::m_global_vars->m_interval_per_tick;
		interfaces::m_global_vars->m_absolute_frame_time = interfaces::m_global_vars->m_interval_per_tick;
		interfaces::m_global_vars->m_frame_count = TIME_TO_TICKS( record->m_sim_time );
		interfaces::m_global_vars->m_tick_count = TIME_TO_TICKS( record->m_sim_time );

		const bool backup_client_side_animation = player->get_client_side_animation( );
		globals::allow_anim_update( ) = player->get_client_side_animation( ) = true;

		player->update_client_side_animation( );

		globals::allow_anim_update( ) = false;
		player->get_client_side_animation( ) = backup_client_side_animation;

		interfaces::m_global_vars->m_cur_time = cur_time;
		interfaces::m_global_vars->m_real_time = real_time;
		interfaces::m_global_vars->m_frame_time = frame_time;
		interfaces::m_global_vars->m_absolute_frame_time = absolute_frame_time;
		interfaces::m_global_vars->m_frame_count = frame_count;
		interfaces::m_global_vars->m_tick_count = tick_count;
		interfaces::m_global_vars->m_interpolation_amount = interp_amount;
	}

	std::memcpy( player->get_anim_layers( ).data( ), record->m_layers.data( ), sizeof( anim_layers_t ) );

	player->invalidate_physics_recursive( 8 );
}

float c_animations::lerp_time( ) {
	/// *_* oh god sorry about that, take a kitten as an apology x3

	///           ╱|、
    ///          (˚ˎ 。7  
    ///           |、˜〵          
    ///          じしˍ,)ノ

	static const auto cl_interp = interfaces::m_cvar_system->find_var( FNV1A( "cl_interp" ) );
	static const auto cl_updaterate = interfaces::m_cvar_system->find_var( FNV1A( "cl_updaterate" ) );
	static const auto sv_minupdaterate = interfaces::m_cvar_system->find_var( FNV1A( "sv_minupdaterate" ) );
	static const auto sv_maxupdaterate = interfaces::m_cvar_system->find_var( FNV1A( "sv_maxupdaterate" ) );
	static const auto cl_interp_ratio = interfaces::m_cvar_system->find_var( FNV1A( "cl_interp_ratio" ) );
	static const auto sv_min_interp_ratio = interfaces::m_cvar_system->find_var( FNV1A( "sv_client_min_interp_ratio" ) );
	static const auto sv_max_interp_ratio = interfaces::m_cvar_system->find_var( FNV1A( "sv_client_max_interp_ratio" ) );

	const float update_rate = std::clamp< float >( cl_updaterate->get_float( ), sv_minupdaterate->get_float( ), sv_maxupdaterate->get_float( ) );
	const float interp_ratio = std::clamp< float >( cl_interp_ratio->get_float( ), sv_min_interp_ratio->get_float( ), sv_max_interp_ratio->get_float( ) );

	return std::clamp< float >( interp_ratio / update_rate, cl_interp->get_float( ), 1.f );
}

bool c_animations::valid_time( c_lag_record* record ) {
	static const auto sv_maxunlag = interfaces::m_cvar_system->find_var( FNV1A( "sv_maxunlag" ) );
	if ( !record || !record->m_player || !record->m_valid || record->m_breaking_lc )
		return false;

	const float lerp_time = this->lerp_time( );
	const float delta_time = std::clamp( interfaces::m_client_state->m_net_channel->get_latency( ) + lerp_time, 0.f, sv_maxunlag->get_float( ) ) -
																									( TICKS_TO_TIME( globals::m_local->get_tick_base( ) ) - record->m_sim_time );
	if ( fabs( delta_time ) > 0.2f )
		return false;

	/// omg v0lvo broke this check but i want to add it because i want to be like Soufiw
	const int dead_time = ( int ) ( ( float ) ( TICKS_TO_TIME( interfaces::m_global_vars->m_tick_count ) + interfaces::m_client_state->m_net_channel->get_latency( ) ) - 0.2f );
	if ( TIME_TO_TICKS( record->m_sim_time + lerp_time ) < dead_time )
		return false;

	return true;
}