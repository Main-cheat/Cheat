#include "listener.h"

void c_listener::on_add_entity( c_base_entity* entity, c_base_handle handle ) {
	if ( !entity || !entity->get_client_class( ) || handle.get_entry_index( ) < 0 )
		return;

	m_entity_list.insert( { entity->get_index( ), FNV1A_RT( entity->get_client_class( )->m_network_name ) } );
}

void c_listener::on_remove_entity( c_base_entity* entity, c_base_handle handle ) {
	if ( !entity || !entity->get_client_class( ) || handle.get_entry_index( ) < 0 )
		return;

	if ( m_entity_list.find( entity->get_index( ) ) == m_entity_list.end( ) )
		return;

	m_entity_list.erase( m_entity_list.find( entity->get_index( ) ) );
}

void c_listener::on_frame( ) {
	if ( m_entity_list.size( ) >= interfaces::m_entity_list->get_number_of_entities( false ) )
		return;

	m_entity_list.clear( );
	for ( int i = 0; i <= interfaces::m_entity_list->get_highest_entity_index( ); i++ ) {
		c_base_entity* entity = ( c_base_entity* ) interfaces::m_entity_list->get_client_entity( i );
		if ( !entity || !entity->get_client_class( ) )
			continue;

		m_entity_list.insert( { i, FNV1A_RT( entity->get_client_class( )->m_network_name ) } );
	}
}