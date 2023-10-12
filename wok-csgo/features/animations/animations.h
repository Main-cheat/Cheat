#pragma once
#include "../../globals.h"

using records_t = std::deque< c_lag_record >;

class c_animations : public c_singleton< c_animations > {
public:
	void on_frame( );

	bool valid_time( c_lag_record* record );
	float lerp_time( );

	__forceinline records_t get_records( c_cs_player* player ) {
		return m_player_records.at( player->get_index( ) );
	}

	__forceinline c_lag_record& get_first_available( c_cs_player* player ) {
		for ( auto it = m_player_records.at( player->get_index( ) ).begin( ); it != m_player_records.at( player->get_index( ) ).end( ); it++ ) {
			if ( valid_time( &*it ) )
				return *it;
		}
		return c_lag_record( player );
	}

	__forceinline c_lag_record& get_last_available( c_cs_player* player ) {
		for ( auto it = m_player_records.at( player->get_index( ) ).rbegin( ); it != m_player_records.at( player->get_index( ) ).rend( ); it++ ) {
			if ( valid_time( &*it ) )
				return *it;
		}
		return c_lag_record { };
	}

private:
	void update_player_animations( c_cs_player* player, c_lag_record* record, c_lag_record* previous_record );

	std::array< records_t, 65 > m_player_records;
};
#define animations c_animations::instance()