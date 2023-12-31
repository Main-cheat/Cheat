#include "../features.h"
#include "../../hooks/hooks.h"

i_material* c_chams::create_material(std::string_view material_name, std::string_view shader_type, std::string_view material_data) {
	const auto key_values = reinterpret_cast<c_key_values*>(interfaces::m_mem_alloc->alloc(36u));

	key_values->init(shader_type.data());
	key_values->load_from_buffer(material_name.data(), material_data.data());

	return interfaces::m_material_system->create_material(material_name.data(), key_values);
}

void c_chams::override_material(int type, const col_t& clr, bool ignorez) {
	i_material* material = nullptr;

	switch (type) {
	case MATERIAL_TYPE_REGULAR: material = ignorez ? m_regular_z : m_regular; break;
	case MATERIAL_TYPE_FLAT: material = ignorez ? m_flat_z : m_flat; break;
	}

	material->alpha_modulate(clr.a() / 255.f);
	material->color_modulate(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f);

	if (const auto $envmaptint = material->find_var(_("$envmaptint"), nullptr, false)) {
		$envmaptint->set_value(vec3_t(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f));
	}

	interfaces::m_model_render->forced_material_override(material);
}

bool c_chams::on_draw_model(i_model_render* ecx, void* edx, void* context, const draw_model_state_t& state, const model_render_info_t& info, matrix3x4_t* bones) {

	const auto entity = reinterpret_cast<c_base_entity*>(interfaces::m_entity_list->get_client_entity(info.m_index));
	if (!entity
		|| !entity->is_player())
		return true;

	const auto player = reinterpret_cast<c_cs_player*>(entity);
	if (!player->is_alive() 
		|| !player->is_enemy(globals::m_local))
		return true;

	for ( auto& record : animations->get_records( player ) ) {
		if ( !animations->valid_time( &record ) || record.m_abs_origin.dist_to( player->get_abs_origin( ) ) < 1.f )
			continue;

		override_material( MATERIAL_TYPE_FLAT, col_t::palette_t::white( 40 ), true );
		hooks::model_render::draw_model_execute::original( ecx, edx, context, state, info, record.m_matrix.data( ) );
	}

	if ( auto record = animations->get_last_available( player ); animations->valid_time( &record ) ) {
		override_material( MATERIAL_TYPE_FLAT, col_t::palette_t::red( 200 ), true );
		hooks::model_render::draw_model_execute::original( ecx, edx, context, state, info, record.m_matrix.data( ) );
	}

	override_material( MATERIAL_TYPE_REGULAR, col_t::palette_t::light_blue( ), true );

	hooks::model_render::draw_model_execute::original( ecx, edx, context, state, info, bones );

	override_material(MATERIAL_TYPE_REGULAR, col_t::palette_t::green(), false);

	return true;
}
