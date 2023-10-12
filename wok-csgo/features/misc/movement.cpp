#include "../features.h"

void c_movement::on_create_move(bool a1) {
	if (!globals::m_local->is_alive())
		return;

	if (a1) {
		rotate(m_view_angles);

		return compute_buttons();
	}

	if ( !globals::m_local->get_flags( ).has( FL_ONGROUND ) ) {
		globals::m_cur_cmd->m_buttons.remove(IN_JUMP);

		auto wish_angles = globals::m_cur_cmd->m_view_angles;

		if ( globals::m_cur_cmd->m_move.x != 0.f || globals::m_cur_cmd->m_move.y != 0.f )
			wish_angles.y = std::remainder(
				wish_angles.y
				+ std::remainder(
					math::rad_to_deg(
						std::atan2( globals::m_cur_cmd->m_move.x, globals::m_cur_cmd->m_move.y )
					) - 90.f, 360.f
				), 360.f
			);

		globals::m_cur_cmd->m_move.x = 0.f;
		globals::m_cur_cmd->m_move.y = 0.f;

		const auto speed_2d = globals::m_local->get_velocity( ).length_2d( );

		const auto ideal_strafe = std::min( 90.f, math::rad_to_deg( std::asin( 21.f / speed_2d ) ) );

		const auto mult = m_strafe_flip ? 1.f : -1.f;

		m_strafe_flip = !m_strafe_flip;

		auto delta = std::remainder( wish_angles.y - m_old_yaw, 360.f );
		if ( delta )
			globals::m_cur_cmd->m_move.y = delta < 0.f ? 450.f : -450.f;

		delta = std::fabsf( delta );

		if ( delta >= 20.f
			 || ideal_strafe >= delta ) {
			const auto vel_angle = math::rad_to_deg( std::atan2( globals::m_local->get_velocity( ).y, globals::m_local->get_velocity( ).x ) );
			const auto vel_delta = std::remainder( wish_angles.y - vel_angle, 360.f );

			if ( speed_2d <= 15.f
				 || ideal_strafe >= vel_delta ) {
				if ( speed_2d <= 15.f
					 || vel_delta >= -ideal_strafe ) {
					globals::m_cur_cmd->m_move.y = 450.f * mult;
					wish_angles.y += ideal_strafe * mult;
				}
				else {
					globals::m_cur_cmd->m_move.y = 450.f;
					wish_angles.y = vel_angle - ideal_strafe;
				}
			}
			else {
				globals::m_cur_cmd->m_move.y = -450.f;
				wish_angles.y = vel_angle + ideal_strafe;
			}

			rotate( wish_angles );
		}
	}

	/* call all movement related stuff such as auto_strafe etc... here.... */

	compute_buttons();
}


void c_movement::rotate(const qangle_t& wish_angles) {
	if (globals::m_cur_cmd->m_view_angles.z != 0.f
		&& !globals::m_local->get_flags().has(FL_ONGROUND)) {
		globals::m_cur_cmd->m_move.y = 0.f;
	}

	auto move_2d = vec2_t(globals::m_cur_cmd->m_move.x, globals::m_cur_cmd->m_move.y);

	if (const auto speed_2d = move_2d.length()) {
		const auto delta = globals::m_cur_cmd->m_view_angles.y - wish_angles.y;

		vec2_t v1;

		math::sin_cos(
			math::deg_to_rad(
				remainderf(math::rad_to_deg(math::atan2(move_2d.y / speed_2d, move_2d.x / speed_2d)) + delta, 360.f)
			), v1.x, v1.y
		);

		const auto cos_x = math::cos(
			math::deg_to_rad(remainderf(math::rad_to_deg(math::atan2(0.f, speed_2d)), 360.f))
		);

		move_2d.x = cos_x * v1.y * speed_2d;
		move_2d.y = cos_x * v1.x * speed_2d;

		if (globals::m_local->get_move_type() == MOVE_TYPE_LADDER) {
			if (wish_angles.x < 45.f
				&& std::fabsf(delta) <= 65.f
				&& globals::m_cur_cmd->m_view_angles.x >= 45.f) {
				move_2d.x *= -1.f;
			}
		}
		else if (std::fabsf(globals::m_cur_cmd->m_view_angles.x) > 90.f) {
			move_2d.x *= -1.f;
		}
	}

	static const auto cl_forwardspeed = interfaces::m_cvar_system->find_var(FNV1A("cl_forwardspeed"));
	static const auto cl_sidespeed = interfaces::m_cvar_system->find_var(FNV1A("cl_sidespeed"));
	static const auto cl_upspeed = interfaces::m_cvar_system->find_var(FNV1A("cl_upspeed"));

	const auto max_forward_speed = cl_forwardspeed->get_float();
	const auto max_side_speed = cl_sidespeed->get_float();
	const auto max_up_speed = cl_upspeed->get_float();

	globals::m_cur_cmd->m_move = vec3_t(
		math::clamp(move_2d.x, -max_forward_speed, max_forward_speed),
		math::clamp(move_2d.y, -max_side_speed, max_side_speed),
		math::clamp(globals::m_cur_cmd->m_move.z, -max_up_speed, max_up_speed)
	);
}

void c_movement::compute_buttons() {
	globals::m_cur_cmd->m_buttons.remove(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);

	if (globals::m_local->get_move_type() == MOVE_TYPE_LADDER) {
		if (std::fabsf(globals::m_cur_cmd->m_move.x) > 200.f) {
			globals::m_cur_cmd->m_buttons.add(globals::m_cur_cmd->m_move.x > 0.f ? IN_FORWARD : IN_BACK);
		}

		if (std::fabsf(globals::m_cur_cmd->m_move.y) > 200.f) {
			globals::m_cur_cmd->m_buttons.add(globals::m_cur_cmd->m_move.y > 0.f ? IN_MOVERIGHT : IN_MOVELEFT);
		}

		return;
	}

	if (globals::m_cur_cmd->m_move.x != 0.f) {
		globals::m_cur_cmd->m_buttons.add(globals::m_cur_cmd->m_move.x > 0.f ? IN_FORWARD : IN_BACK);
	}

	if (globals::m_cur_cmd->m_move.y != 0.f) {
		globals::m_cur_cmd->m_buttons.add(globals::m_cur_cmd->m_move.y > 0.f ? IN_MOVERIGHT : IN_MOVELEFT);
	}
}