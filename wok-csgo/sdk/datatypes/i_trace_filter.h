#pragma once

class i_handle_entity;

struct surface_t {
	const char*	m_name;
	short		m_surface_props;
	uint16_t	m_flags;
};

struct plane_t {
	vec3_t		m_normal;
	float		m_dist;
	char		m_type;
	char		m_signbits;
	char		pad0[2];
};

class i_trace_filter {
public:
	virtual bool should_hit_entity(i_handle_entity* handle, int mask) = 0;
	virtual e_trace_type get_trace_type() const = 0;
};

class c_trace_filter : public i_trace_filter {
public:
	c_trace_filter() = default;
	c_trace_filter(i_handle_entity* skip, e_trace_type type = TRACE_EVERYTHING) {
		m_skip = skip;
		m_type = type;
	}

	__forceinline bool should_hit_entity(i_handle_entity* handle, int mask) { return handle != m_skip; }

	__forceinline e_trace_type get_trace_type() const { return m_type; }

	i_handle_entity* m_skip = nullptr;
	e_trace_type     m_type = TRACE_EVERYTHING;
};

using should_hit_fn_t = bool( __cdecl* )( i_handle_entity* const, const int );
class c_trace_filter_skip_two_entities {
public:
	__forceinline c_trace_filter_skip_two_entities( ) {
		static const auto trace_filter_skip_two_entities = SIG( "client.dll", "C7 45 ? ? ? ? ? 89 45 E4 8B 01" ).self_offset( 0x3u ).cast< void* >( );

		m_vtable = *reinterpret_cast< std::uintptr_t* >( trace_filter_skip_two_entities );
	}

	__forceinline c_trace_filter_skip_two_entities( i_handle_entity* const ignore_entity0, i_handle_entity* const ignore_entity1, const int collision_group = 0 ) {
		static const auto trace_filter_skip_two_entities = SIG( "client.dll", "C7 45 ? ? ? ? ? 89 45 E4 8B 01" ).self_offset( 0x3u ).cast< void* >( );

		m_vtable = *reinterpret_cast< std::uintptr_t* >( trace_filter_skip_two_entities );
		m_ignore_entity0 = ignore_entity0;
		m_ignore_entity1 = ignore_entity1;
		m_collision_group = collision_group;
	}

	__forceinline i_trace_filter* get( ) {
		return ( i_trace_filter* ) this;
	}

	std::uintptr_t	m_vtable {};
	i_handle_entity* m_ignore_entity0 {};
	int				m_collision_group {};
	should_hit_fn_t	m_should_hit_fn {};
	i_handle_entity* m_ignore_entity1 {};
};

class c_trace_filter_simple {
public:
	__forceinline c_trace_filter_simple( ) {
		static const auto trace_filter_simple = SIG( "client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52" ).self_offset( 0x3Du ).cast< void* >( );

		m_vtable = *reinterpret_cast< std::uintptr_t* >( trace_filter_simple );
	}

	__forceinline c_trace_filter_simple( i_handle_entity* const ignore_entity, const int collision_group = 0 ) {
		static const auto trace_filter_simple = SIG( "client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52" ).self_offset( 0x3Du ).cast< void* >( );

		m_vtable = *reinterpret_cast< std::uintptr_t* >( trace_filter_simple );
		m_ignore_entity = ignore_entity;
		m_collision_group = collision_group;
	}

	__forceinline i_trace_filter* get( ) {
		return ( i_trace_filter* ) this;
	}

	std::uintptr_t	m_vtable {};
	i_handle_entity* m_ignore_entity {};
	int				m_collision_group {};
	should_hit_fn_t	m_should_hit_fn {};
};


class c_trace_filter_world_only : public i_trace_filter {
public:
	__forceinline bool should_hit_entity(i_handle_entity* handle, int mask) { return false; }

	__forceinline e_trace_type get_trace_type() const { return TRACE_WORLD_ONLY; }
};

class c_trace_filter_world_and_props_only : public i_trace_filter {
public:
	__forceinline bool should_hit_entity(i_handle_entity* handle, int mask) { return false; }

	__forceinline e_trace_type get_trace_type() const { return TRACE_EVERYTHING; }
};

class c_trace_filter_no_players : public c_trace_filter {
public:
	__forceinline bool should_hit_entity(i_handle_entity* handle, int mask) override { return false; }
};

class c_trace_entity : public i_trace_filter {
public:
	c_trace_entity() = default;
	c_trace_entity(i_handle_entity* hit) { m_hit = hit; }

	__forceinline bool should_hit_entity(i_handle_entity* handle, int mask) { return handle == m_hit; }

	__forceinline e_trace_type get_trace_type() const { return TRACE_ENTITIES_ONLY; }

	i_handle_entity* m_hit = nullptr;
};

struct ray_t {
	ray_t ( ) : m_world_axis_transform ( NULL ) { };
	ray_t ( const vec3_t& start, const vec3_t& end ) { init ( start, end ); }
	ray_t ( const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs ) { init ( start, end, mins, maxs ); }

	__forceinline void init ( const vec3_t& start, const vec3_t& end )
	{
		m_start = start;
		m_delta = end - start;
		m_start_offset = vec4a_t ( 0.f, 0.f, 0.f );
		m_extents = vec4a_t ( 0.f, 0.f, 0.f );

		m_world_axis_transform = nullptr;

		m_is_ray = true;
		m_is_swept = m_delta.length_sqr ( ) != 0.f;
	}

	__forceinline void init ( const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs )
	{
		m_delta = end - start;

		m_world_axis_transform = nullptr;
		m_is_swept = m_delta.length ( ) != 0.f;

		m_extents = maxs - mins;
		m_extents *= 0.5f;
		m_is_ray = m_extents.length_sqr ( ) < 1e-6;

		m_start_offset = maxs + mins;
		m_start_offset *= 0.5f;
		m_start = start + m_start_offset;
		m_start_offset *= -1.0f;
	}

	vec4a_t				m_start;
	vec4a_t				m_delta;
	vec4a_t				m_start_offset;
	vec4a_t				m_extents;
	const matrix3x4_t*	m_world_axis_transform;
	bool				m_is_ray;
	bool				m_is_swept;
};

class c_base_trace {
public:
	c_base_trace() = default;

	__forceinline bool is_disp_surface() { return m_disp_flags & DISPSURF_FLAG_SURFACE; }

	__forceinline bool is_disp_surface_walkable() { return m_disp_flags & DISPSURF_FLAG_WALKABLE; }

	__forceinline bool is_disp_surface_buildable() { return m_disp_flags & DISPSURF_FLAG_BUILDABLE; }

	__forceinline bool is_disp_surface_prop1() { return m_disp_flags & DISPSURF_FLAG_SURFPROP1; }

	__forceinline bool is_disp_surface_prop2() { return m_disp_flags & DISPSURF_FLAG_SURFPROP2; }

	vec3_t		m_start_pos;
	vec3_t		m_end_pos;
	plane_t		m_plane;

	float		m_fraction;

	int		m_contents;
	unsigned short	m_disp_flags;

	bool		m_all_solid;
	bool		m_start_solid;
};

class c_game_trace : public c_base_trace {
public:
	c_game_trace() = default;

	__forceinline int get_entity_index() const { return m_hit_entity ? m_hit_entity->get_index() : -1; }

	__forceinline bool did_hit() const { return m_fraction < 1.f || m_all_solid || m_start_solid; }

	__forceinline bool did_hit_world() const { return get_entity_index() == 0; }

	__forceinline bool did_hit_non_world_entity() const { return m_hit_entity && !did_hit_world(); }

	__forceinline bool is_visible() const { return m_fraction > 0.97f; }

	float			m_fraction_left_solid;
	surface_t		m_surface;
	int			m_hitgroup;
	short			m_physics_bone;
	unsigned short		m_world_surface_index;
	i_client_entity*	m_hit_entity;
	int			m_hitbox;
};
