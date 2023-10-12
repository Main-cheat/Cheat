#include "hooks.h"

namespace hooks {
	void init( ) {
		MH_Initialize( );

		HOOK_VFUNC( interfaces::m_key_values_system, key_values_system::alloc_key_values_memory::index, key_values_system::alloc_key_values_memory::fn, key_values_system::alloc_key_values_memory::original );

		HOOK_VFUNC( interfaces::m_d3d_device, d3d_device::reset::index, d3d_device::reset::fn, d3d_device::reset::original );
		HOOK_VFUNC( interfaces::m_d3d_device, d3d_device::present::index, d3d_device::present::fn, d3d_device::present::original );

		HOOK_VFUNC( interfaces::m_client_dll, client_dll::create_move::index, client_dll::create_move::gate, client_dll::create_move::original );
		HOOK_VFUNC( interfaces::m_client_dll, client_dll::frame_stage_notify::index, client_dll::frame_stage_notify::fn, client_dll::frame_stage_notify::original );

		HOOK_VFUNC( interfaces::m_client_mode, client_mode::override_view::index, client_mode::override_view::fn, client_mode::override_view::original );

		HOOK_VFUNC( interfaces::m_entity_list, client_entity_list::on_add_entity::index, client_entity_list::on_add_entity::fn, client_entity_list::on_add_entity::original );
		HOOK_VFUNC( interfaces::m_entity_list, client_entity_list::on_remove_entity::index, client_entity_list::on_remove_entity::fn, client_entity_list::on_remove_entity::original );

		HOOK_VFUNC( interfaces::m_model_render, model_render::draw_model_execute::index, model_render::draw_model_execute::fn, model_render::draw_model_execute::original );

		HOOK_VFUNC( interfaces::m_panel, panel::paint_traverse::index, panel::paint_traverse::fn, panel::paint_traverse::original );

		HOOK_VFUNC( interfaces::m_surface, surface::lock_cursor::index, surface::lock_cursor::fn, surface::lock_cursor::original );

		HOOK_VFUNC( c_cs_player::get_vtable( ), player::eye_angles::index, player::eye_angles::fn, player::eye_angles::original );
		HOOK_VFUNC( c_cs_player::get_vtable( ), player::update_client_side_animation::index, player::update_client_side_animation::fn, player::update_client_side_animation::original );
		HOOK( SIG( "client.dll", "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 28 8B" ), player::build_transformations::fn, player::build_transformations::original );
		HOOK( SIG( "client.dll", "55 8B EC 83 E4 F8 81 EC FC 00 00 00 53 56 8B F1 57" ), player::do_extra_bones_processing::fn, player::do_extra_bones_processing::original );
		HOOK( SIG( "client.dll", "56 8B F1 80 BE FC 26" ), player::get_client_renderable::fn, player::get_client_renderable::original );
		HOOK( SIG( "client.dll", "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6" ), player::standard_blending_rules::fn, player::standard_blending_rules::original );
		HOOK( SIG( "client.dll", "57 8B F9 8B 07 8B 80 78" ), player::should_skip_animation_frame::fn, player::should_skip_animation_frame::original );
		HOOK( SIG( "client.dll", "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38 83" ), player::clamp_bones_in_bbox::fn, player::clamp_bones_in_bbox::original );

		HOOK_VFUNC( i_client_renderable::get_vtable( ), renderable::setup_bones::index, renderable::setup_bones::fn, renderable::setup_bones::original );

		HOOK( SIG( "client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 57 83 BE" ), game::interpolate_viewmodel::fn, game::interpolate_viewmodel::original );

		MH_EnableHook( MH_ALL_HOOKS );
	}

	void undo( ) {
		MH_DisableHook( MH_ALL_HOOKS );
		MH_Uninitialize( );
	}
}
