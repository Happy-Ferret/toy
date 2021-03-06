//  Copyright (c) 2018 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#pragma once

#include <blocks/Forward.h>
#include <toy/toy.h>

using namespace mud;
using namespace toy;

extern "C"
{
	//_BLOCKS_EXPORT void ex_blocks_game(GameShell& app, Game& game);
}

#ifdef TOY_ECS
namespace mud
{
	template <> struct TypedBuffer<Well*>	{ static size_t index() { return 12; } };
	template <> struct TypedBuffer<Camp*>	{ static size_t index() { return 13; } };
	template <> struct TypedBuffer<Shield*>	{ static size_t index() { return 14; } };
	template <> struct TypedBuffer<Slug*>	{ static size_t index() { return 15; } };
	template <> struct TypedBuffer<Tank*>	{ static size_t index() { return 16; } };
}
#endif

enum CustomCollisionGroup : short int
{
	CM_ENERGY = 1 << 10,
};

class refl_ _BLOCKS_EXPORT Well : public Entity
{
public:
	constr_ Well(HSpatial parent, const vec3& position);
	~Well();

	comp_ attr_ CSpatial m_spatial;
	comp_ attr_ CEmitter m_emitter;

	void next_frame(size_t tick, size_t delta);
};

class refl_ _BLOCKS_EXPORT Faction
{
public:
	constr_ Faction(uint32_t id, const Colour& colour);
	~Faction();

	attr_ uint32_t m_id;
	attr_ Colour m_colour;

	Tank* m_leader = nullptr;

	Material* m_highlight2;
	Material* m_highlight11;

	static const size_t s_max_factions = 10U;
};

export_ extern _BLOCKS_EXPORT std::vector<Faction> g_factions;

class refl_ _BLOCKS_EXPORT Camp : public Entity
{
public:
	constr_ Camp(HSpatial parent, const vec3& position, Faction& faction);

	comp_ attr_ CSpatial m_spatial;

	attr_ vec3 m_position;
	attr_ Faction& m_faction;
};

class refl_ _BLOCKS_EXPORT Shield : public Entity
{
public:
	constr_ Shield(HSpatial parent, const vec3& position, Faction& faction, float radius);
	~Shield();

	comp_ attr_ CSpatial m_spatial;
	comp_ attr_ CEmitter m_emitter;

	attr_ Faction& m_faction;
	attr_ float m_radius;
	attr_ float m_charge;
	attr_ float m_discharge;

	OCollider m_collider;

	void next_frame(size_t tick, size_t delta);
};

class refl_ _BLOCKS_EXPORT Slug : public Entity
{
public:
	Slug(HSpatial parent, const vec3& source, const quat& rotation, const vec3& velocity, float power = 1.f);
	~Slug();

	comp_ attr_ CSpatial m_spatial;

	attr_ vec3 m_source;
	attr_ vec3 m_velocity;
	attr_ float m_power = 1.f;

	bool m_impacted = false;
	bool m_destroy = false;
	vec3 m_impact = Zero3;

	//OSolid m_solid;
	OCollider m_collider;

	void update(Spatial& spatial);
};

class refl_ _BLOCKS_EXPORT Tank : public Entity
{
public:
	constr_ Tank(HSpatial parent, const vec3& position, Faction& faction);
	~Tank();

	comp_ attr_ CSpatial m_spatial;
	comp_ attr_ CMovable m_movable;
	comp_ attr_ CEmitter m_emitter;
	comp_ attr_ CReceptor m_receptor;

	Faction& m_faction;

	OSolid m_solid;

	vec3 m_force = Zero3;
	vec3 m_torque = Zero3;

	quat m_turret_angle = ZeroQuat;

	float m_cooldown = 0.f;
	float m_hitpoints = 100.f;
	float m_energy = 100.f;
	float m_range = 50.f;

	bool m_stealth = false;
	float m_shock = 0.f;

	Tank* m_target = nullptr;
	vec3 m_dest = Zero3;

	bool m_ia = true;

	std::vector<object_ptr<Slug>> m_slugs;

	void next_frame(Spatial& spatial, Movable& movable, Receptor& receptor, size_t tick, size_t delta);

	void shoot(bool critical = false);

	inline quat turret_rotation() { return m_turret_angle; } //quat(vec3(0.f, m_turret_angle, 0.f)); }
	inline vec3 turret_direction() { return rotate(turret_rotation(), -Z3); }
};

class refl_ _BLOCKS_EXPORT BlockWorld : public Complex
{
public:
	constr_ BlockWorld(const std::string& name);
	~BlockWorld();

	comp_ attr_ World m_world;
	comp_ attr_ BulletWorld m_bullet_world;
	comp_ attr_ Navmesh m_navmesh;

	attr_ uvec3 m_block_subdiv = uvec3(20, 4, 20);
	attr_ vec3 m_tile_scale = vec3(4.f);
	attr_ vec3 m_block_size;

	attr_ vec3 m_world_size;
	
	std::map<ivec2, TileBlock*> m_blocks;
	TileBlock* m_center_block = nullptr;

	void generate_block(GfxSystem& gfx_system, const ivec2& coord);
};

class refl_ _BLOCKS_EXPORT Player
{
public:
	Player(BlockWorld& world);
	BlockWorld* m_world;
	Tank m_tank;
};

_BLOCKS_EXPORT void paint_tank(Gnode& parent, Tank& tank);
