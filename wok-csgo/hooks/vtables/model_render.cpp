#include "../hooks.h"

void __fastcall hooks::model_render::draw_model_execute::fn(i_model_render* ecx, void* edx, void* context, const draw_model_state_t& state, const model_render_info_t& info, matrix3x4_t* bones) {

	if (info.m_flags.has(STUDIO_SHADOW_DEPTH_TEXTURE))
		return original(ecx, edx, context, state, info, bones);
	
	if (chams->on_draw_model(ecx, edx, context, state, info, bones)) {
		original(ecx, edx, context, state, info, bones);
	}

	interfaces::m_model_render->forced_material_override();
}
