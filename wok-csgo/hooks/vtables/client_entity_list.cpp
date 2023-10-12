#include "../hooks.h"

void __fastcall hooks::client_entity_list::on_add_entity::fn( i_client_entity_list* ecx, void* edx, i_handle_entity* ent, c_base_handle handle ) {
	const auto unknown = reinterpret_cast< i_client_unknown* const >( ent );

	if ( const auto& base_entity = unknown->get_base_entity( ) )
		listener->on_add_entity( base_entity, handle );

	original( ecx, edx, ent, handle );
}

void __fastcall hooks::client_entity_list::on_remove_entity::fn( i_client_entity_list* ecx, void* edx, i_handle_entity* ent, c_base_handle handle ) {
	const auto unknown = reinterpret_cast< i_client_unknown* const >( ent );

	if ( const auto& base_entity = unknown->get_base_entity( ) )
		listener->on_remove_entity( base_entity, handle );

	original( ecx, edx, ent, handle );
}