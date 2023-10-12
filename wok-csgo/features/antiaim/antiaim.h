#pragma once
#include "../../globals.h"

class c_antiaim : public c_singleton< c_antiaim > {
public:
	void on_pre_prediction( );
	void on_create_move( );

private:
	float get_pitch( );
	float get_yaw( );
	float get_fake_yaw( );
	float get_roll( );

	struct {
		bool m_switch_jitter {};
		int m_desync_side { -1 };
		int m_max_choke { 14 };
	} m_data;
};
#define antiaim c_antiaim::instance( )