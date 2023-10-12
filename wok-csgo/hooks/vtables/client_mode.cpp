#include "../hooks.h"

void __stdcall hooks::client_mode::override_view::fn(view_setup_t* view) {
	original(view);
}