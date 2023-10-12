#include "../hooks.h"

void __stdcall hooks::client_dll::frame_stage_notify::fn(e_client_frame_stage stage) {

	if (stage == FRAME_RENDER_START
		&& interfaces::m_engine->is_in_game() && globals::m_local->is_alive()) {
		interfaces::m_engine->get_view_angles(globals::angles::m_view);
	}

	if ( stage == FRAME_NET_UPDATE_END )
		listener->on_frame( );

	if ( stage == FRAME_NET_UPDATE_END )
		animations->on_frame( );

	original(stage);
}

__declspec (naked) void __stdcall hooks::client_dll::create_move::gate(int sequence_number, float input_sample_frame_time, bool active) {
	__asm {
		push ebp
		mov ebp, esp
		push ebx
		push esp
		push dword ptr [active]
		push dword ptr [input_sample_frame_time]
		push dword ptr [sequence_number]
		call fn
		pop ebx
		pop ebp
		retn 0Ch
	}
}

void __stdcall hooks::client_dll::create_move::fn(int sequence_number, float input_sample_frame_time, bool active, bool& packet) {

	original(sequence_number, input_sample_frame_time, active);

	globals::m_packet = packet = true;

	if (!globals::m_local)
		return;

	globals::m_weapon.init( globals::m_local );

	const auto cmd = interfaces::m_input->get_user_cmd(sequence_number);
	if (!cmd
		|| !cmd->m_command_number)
		return;

	globals::m_cur_cmd = cmd;

	movement->set_view_angles(cmd->m_view_angles);

	engine_prediction->update();

	movement->on_create_move(false);

	antiaim->on_pre_prediction( );

	engine_prediction->process();

	antiaim->on_create_move( );

	c_ragebot::instance( )->on_create_move( );

	engine_prediction->restore();

	cmd->m_view_angles.sanitize();

	globals::angles::m_anim = cmd->m_view_angles;
	if ( !globals::m_packet )
		globals::angles::m_real = cmd->m_view_angles;
	else
		globals::angles::m_fake = cmd->m_view_angles;

	movement->on_create_move(true);
	local_animations->on_create_move( );

	packet = globals::m_packet;

	const auto verified = interfaces::m_input->get_verified_user_cmd(sequence_number);

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->get_check_sum();
}
