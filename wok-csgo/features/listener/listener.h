#pragma once
#include "../../common_includes.h"
#include "../../sdk/interfaces.h"
#include "../../singleton.h"

class c_listener : public c_singleton< c_listener > {
public:
	void on_add_entity( c_base_entity* entity, c_base_handle handle );
	void on_remove_entity( c_base_entity* entity, c_base_handle handle );

	__forceinline std::vector< c_cs_player* > get_players( ) {
		std::vector< c_cs_player* > list = {};
		for ( const auto& [ key, network_name ] : m_entity_list ) {
			if ( network_name != FNV1A( "CCSPlayer" ) )
				continue;

			list.emplace_back( ( c_cs_player* )interfaces::m_entity_list->get_client_entity( key ) );
		}

		return list;
	}

	template < typename t >
	__forceinline std::vector< t > get_as( uint32_t hash ) {
		std::vector< t > list = {};
		for ( const auto& [ key, network_name ] : m_entity_list ) {
			if ( network_name != hash )
				continue;

			list.emplace_back( ( t ) interfaces::m_entity_list->get_client_entity( key ) );
		}

		return list;
	}

	void on_frame( );

public:
	std::unordered_map< int, uint32_t > m_entity_list;
};
#define listener c_listener::instance()