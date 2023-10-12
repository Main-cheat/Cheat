#pragma once
#include "../../globals.h"

class c_local_animations : public c_singleton< c_local_animations > {
public:
	void on_create_move( );
	bool on_bone_setup( matrix3x4_t* bone_to_out, int bone_count );
	void on_animation_update( );
	void on_fire_bullet( );
private:
	struct {
		qangle_t m_angle;
		anim_layers_t m_layers;
		pose_params_t m_poses;

		bone_matrix_t m_matrix;
		std::array< vec3_t, 256u > m_bone_origins;
	} m_anim_data[ 2 ];

	float m_last_sent_angle {};
	float m_last_anim_time {};
	float m_spawn_time {};
};
#define local_animations c_local_animations::instance( )