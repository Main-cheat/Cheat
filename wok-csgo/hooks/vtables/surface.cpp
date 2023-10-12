#include "../hooks.h"

void __fastcall hooks::surface::lock_cursor::fn(i_surface* ecx, void* edx) {

	return input::m_blocked ? ecx->unlock_cursor() : original(ecx, edx);
}