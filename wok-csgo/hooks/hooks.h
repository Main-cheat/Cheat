#pragma once
#include "../globals.h"
#include "../features/features.h"
#include "..\utils\minhook\MinHook.h"

#define FC_ARGS void* ecx, void* edx

#define HOOK( target, hook, original ) \
    if ( MH_CreateHook( memory::address_t{ target }.cast< LPVOID >( ), \
        reinterpret_cast< LPVOID >( &hook ), reinterpret_cast< LPVOID* >( &original ) ) != MH_OK ) \
        MessageBoxA(nullptr, "can't hook " #hook ".", "cheat", MB_OKCANCEL | MB_ICONERROR ) \

#define HOOK_VFUNC( vft, index, hook, original ) \
    if ( MH_CreateHook( ( *memory::address_t{ vft }.cast< LPVOID** >( ) )[ index ], \
        hook, reinterpret_cast< LPVOID* >( &original ) ) != MH_OK ) \
        MessageBoxA(nullptr, "can't hook " #hook ".", "cheat", MB_OKCANCEL | MB_ICONERROR) \

namespace hooks {
	void init();

	void undo();

	namespace game {
		namespace interpolate_viewmodel {
			bool __fastcall fn( FC_ARGS, float time );
			inline decltype ( &fn ) original;
		}
	}

	namespace key_values_system {
		namespace alloc_key_values_memory {
			constexpr auto index = 2u;
			using T = void*(__thiscall*)(void*, int);
			void* __fastcall fn(void* ecx, void* edx, int size);

			inline decltype (&fn) original;
		}
	}

	namespace client_entity_list {
		namespace on_add_entity {
			constexpr auto index = 11u;
			void __fastcall fn( i_client_entity_list* ecx, void* edx, i_handle_entity* ent, c_base_handle handle );
			inline decltype ( &fn ) original;
		}

		namespace on_remove_entity {
			constexpr auto index = 12u;
			void __fastcall fn( i_client_entity_list* ecx, void* edx, i_handle_entity* ent, c_base_handle handle );
			inline decltype ( &fn ) original;
		}
	}

	namespace d3d_device {
		namespace reset {
			constexpr auto index = 16u;
			using T = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
			long __stdcall fn(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_params);

			inline decltype (&fn) original;
		}

		namespace present {
			constexpr auto index = 17u;
			using T = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
			long __stdcall fn(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region);

			inline decltype (&fn) original;
		}
	}

	namespace client_dll {
		namespace frame_stage_notify {
			constexpr auto index = 37u;
			using T = void(__stdcall*)(e_client_frame_stage);
			void __stdcall fn(e_client_frame_stage stage);

			inline decltype ( &fn ) original;
		}

		namespace create_move {
			constexpr auto index = 22u;
			using T = void(__thiscall*)(void*, int, float, bool);
			void __stdcall gate(int sequence_number, float input_sample_frame_time, bool active);
			void __stdcall fn(int sequence_number, float input_sample_frame_time, bool active, bool& packet);

			inline decltype (&gate) original;
		}
	}

	namespace client_mode {
		namespace override_view {
			constexpr auto index = 18u;
			using T = void(__stdcall*)(view_setup_t*);
			void __stdcall fn(view_setup_t* view);

			inline decltype (&fn) original;
		}
	}

	namespace model_render {
		namespace draw_model_execute {
			constexpr auto index = 21u;
			using T = void(__thiscall*)(i_model_render*, void*, const draw_model_state_t&, const model_render_info_t&, matrix3x4_t*);
			void __fastcall fn(i_model_render* ecx, void* edx, void* context, const draw_model_state_t& state, const model_render_info_t& info, matrix3x4_t* bones);

			inline decltype (&fn) original;
		}
	}

	namespace panel {
		namespace paint_traverse {
			constexpr auto index = 41u;
			using T = void(__thiscall*)(void*, uint32_t, bool, bool);
			void __fastcall fn(void* ecx, void* edx, uint32_t id, bool force_repaint, bool allow_force);

			inline decltype (&fn) original;
		}
	}

	namespace surface {
		namespace lock_cursor {
			constexpr auto index = 67u;
			using T = void(__thiscall*)(i_surface*);
			void __fastcall fn(i_surface* ecx, void* edx);

			inline decltype (&fn) original;
		}
	}

	namespace player {
		namespace eye_angles {
			constexpr auto index = 170u;
			qangle_t* __fastcall fn(c_cs_player* ecx, void* edx);

			inline decltype (&fn) original;
		}

		namespace update_client_side_animation {
			constexpr auto index = 224u;
			void __fastcall fn( c_cs_player* ecx, void* edx );

			inline decltype ( &fn ) original;
		}

		namespace build_transformations {
			void __fastcall fn( c_cs_player* ecx, void* edx, c_studio_hdr* hdr, vec3_t* pos, void* q, matrix3x4_t& camera_transform, int bone_mask, void* bone_computed );
			inline decltype ( &fn ) original;
		}

		namespace do_extra_bones_processing {
			void __fastcall fn( c_cs_player* ecx, void* edx, int a2, int a3, int a4, int a5, int a6, int a7 );
			inline decltype( &fn ) original;
		}

		namespace get_client_renderable {
			void* __fastcall fn( c_cs_player*, void* );
			inline decltype( &fn ) original;
		}

		namespace standard_blending_rules {
			void __fastcall fn( c_cs_player* ecx, void* edx, void* hdr, vec3_t* pos, void* q, float curtime, int mask );
			inline decltype( &fn ) original;
		}

		namespace should_skip_animation_frame {
			bool __fastcall fn( c_base_animating* ecx, void* edx );
			inline decltype( &fn ) original;
		}

		namespace clamp_bones_in_bbox {
			void __fastcall fn( c_cs_player* ecx, void* edx, void* mat, int mask );
			inline decltype( &fn ) original;
		}
	}

	namespace renderable {
		namespace setup_bones {
			constexpr auto index = 13u;
			using T = bool(__thiscall*)(i_client_renderable*, matrix3x4_t*, int, int, float);
			bool __fastcall fn(i_client_renderable* ecx, void* edx, matrix3x4_t* bones, int max_bones, int mask, float time);

			inline decltype (&fn) original;
		}
	}
}
