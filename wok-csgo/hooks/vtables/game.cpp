#include "../hooks.h"

bool __fastcall hooks::game::interpolate_viewmodel::fn( FC_ARGS, float time ) {
	c_base_view_model* model = ( c_base_view_model* ) ecx;
	c_cs_player* owner = ( c_cs_player* ) interfaces::m_entity_list->get_client_entity_from_handle( model->get_owner( ) );
	if ( !owner || owner != globals::m_local )
		return original( ecx, edx, time );

	const auto backup_final_tick = globals::m_local->get_final_predicted_tick( );
	globals::m_local->get_final_predicted_tick( ) = TIME_TO_TICKS( interfaces::m_global_vars->m_cur_time );

	const auto backup_interp_amount = interfaces::m_global_vars->m_interpolation_amount;
	interfaces::m_global_vars->m_interpolation_amount = 0.f;

	const auto ret_addr = original( ecx, edx, time );

	globals::m_local->get_final_predicted_tick( ) = backup_final_tick;
	interfaces::m_global_vars->m_interpolation_amount = backup_interp_amount;

	return ret_addr;
}