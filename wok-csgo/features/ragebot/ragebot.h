#pragma once
#include "../../globals.h"

struct scan_data_t {
	vec3_t m_point {};
	float m_damage {};
};

struct target_data_t {
	c_cs_player* m_player = nullptr;
	c_lag_record* m_record = nullptr;
	scan_data_t m_scan_data {};
};

class c_ragebot : public c_singleton< c_ragebot > {
public:
	void on_create_move( );

	bool can_hit_point( vec3_t start, vec3_t point );
private:
	
	void scan_player( target_data_t& data, c_lag_record* record );
};

