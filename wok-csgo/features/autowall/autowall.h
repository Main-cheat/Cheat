#pragma once
#include "../../globals.h"

struct fire_bullet_data_t {
	vec3_t m_position = { };
	vec3_t m_direction = { };
	c_game_trace m_enter_trace = { };
	float m_current_damage = 0.0f;
	int m_penetrate_count = 0;
};

class c_autowall : public c_singleton< c_autowall > {
public:
	float get_damage( c_cs_player* attacker, const vec3_t& point, fire_bullet_data_t* data_out = nullptr );
	void  scale_damage( const int hit_group, c_cs_player* player, const float weapon_armor_ratio, const float weapon_headshot_multiplier, float* damage_to_scale );
	bool  simulate_fire_bullet( c_cs_player* attacker, c_base_combat_weapon* weapon, fire_bullet_data_t& data );
	bool  trace_to_exit( const c_game_trace& enter_trace, c_game_trace& exit_trace, const vec3_t& position, const vec3_t& direction, i_handle_entity* clip_player );
	bool  handle_bullet_penetration( c_cs_player* local, const c_cs_weapon_data* weapon_data, const surfacedata_t* enter_surface_data, fire_bullet_data_t& data );
	void  clip_trace_to_players( const vec3_t& abs_start, const vec3_t& abs_end, const int contents_mask, i_trace_filter* filter, c_game_trace* trace, const float min_range = 0.0f, const float max_range = 60.0f );
};
#define autowall c_autowall::instance( )