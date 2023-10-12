#include "local_animations.h"

void c_local_animations::on_create_move( ) {
	if ( !globals::m_local || !globals::m_local->is_alive( ) )
		return;

	const auto state = globals::m_local->get_anim_state( );
	if ( !state )
		return;

	const auto curtime = interfaces::m_global_vars->m_cur_time;
	const auto frametime = interfaces::m_global_vars->m_frame_time;

	interfaces::m_global_vars->m_cur_time = TICKS_TO_TIME( globals::m_local->get_tick_base( ) );
	interfaces::m_global_vars->m_frame_time = interfaces::m_global_vars->m_interval_per_tick;

	auto& real_data = m_anim_data[ 0 ];

	if ( state->m_last_frame_count == interfaces::m_global_vars->m_frame_count )
		state->m_last_frame_count = interfaces::m_global_vars->m_frame_count - 1;

	std::memcpy( real_data.m_layers.data( ), globals::m_local->get_anim_layers( ).data( ), sizeof( anim_layers_t ) );

	globals::allow_anim_update( ) = true;

	globals::m_local->update_client_side_animation( );

	globals::allow_anim_update( ) = false;

	if ( globals::m_packet ) {
		m_last_sent_angle = state->m_foot_yaw;
		std::memcpy( real_data.m_poses.data( ), globals::m_local->get_pose_params( ).data( ), sizeof( pose_params_t ) );
	}

	real_data.m_layers.at( ANIMATION_LAYER_LEAN ).m_weight = 0.0f;

	state->m_foot_yaw = globals::angles::m_real.y;
	globals::m_local->set_abs_angles( qangle_t( 0.f, m_last_sent_angle, 0.f ) );

	std::memcpy( globals::m_local->get_anim_layers( ).data( ), real_data.m_layers.data( ), sizeof( anim_layers_t ) );
	std::memcpy( globals::m_local->get_pose_params( ).data( ), real_data.m_poses.data( ), sizeof( pose_params_t ) );

	interfaces::m_global_vars->m_cur_time = curtime;
	interfaces::m_global_vars->m_frame_time = frametime;

	globals::allow_setup_bones( ) = true;
	globals::m_local->setup_bones( m_anim_data[ 0 ].m_matrix.data( ), 256, BONE_FLAG_USED_BY_ANYTHING, globals::m_local->get_sim_time( ) );
	globals::m_local->setup_bones_attachment_helper( globals::m_local->get_studio_hdr( ) );
	globals::allow_setup_bones( ) = false;
	
	for ( int i = 0; i < 256; i++ )
		m_anim_data[ 0 ].m_bone_origins.at( i ) = globals::m_local->get_abs_origin( ) - m_anim_data[ 0 ].m_matrix.at( i ).get_column( 3 );
}

bool c_local_animations::on_bone_setup( matrix3x4_t* bone_to_out, int bone_count ) {
	std::memcpy( globals::m_local->get_bone_cache( )->m_cached_bones, m_anim_data[ 0 ].m_matrix.data( ), sizeof( matrix3x4_t ) * globals::m_local->get_bone_cache( )->m_cached_bones_count );
	std::memcpy( bone_to_out, m_anim_data[ 0 ].m_matrix.data( ), sizeof( matrix3x4_t ) * globals::m_local->get_bone_cache( )->m_cached_bones_count );
	return true;
}

void c_local_animations::on_animation_update( ) {
	for ( int i = 0; i < 256; i++ )
		m_anim_data[ 0 ].m_matrix.at( i ).set_column( globals::m_local->get_abs_origin( ) - m_anim_data[ 0 ].m_bone_origins.at( i ), 3 );

}

void c_local_animations::on_fire_bullet( ) {

}