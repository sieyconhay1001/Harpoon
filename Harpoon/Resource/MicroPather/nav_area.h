#pragma once
#include "nav_hiding_spot.h"
#include "nav_structs.h"

namespace nav_mesh {

	struct Bounds {
		Vector nw, sw, se, ne;
	};

	class nav_area : public nav_area_critical_data {
	public:
		nav_area( nav_buffer& buffer );

		vec3_t get_center( )								{ return m_center; }
		std::uint32_t get_id( )								{ return m_id; }		
		
		std::vector< nav_connect_t >& get_connections( )	{ return m_connections; }
		
		std::vector<nav_hiding_spot>& get_hiding_spots()     { return m_hiding_spots;  }
		std::vector<nav_spot_encounter_t>& get_encounter_spots() { return m_spot_encounters; }
		std::vector<nav_area_bind_info_t>& get_vis_areas() {		return m_potentially_visible_areas;		}

		bool is_within( vec3_t position );

		Bounds GetBounds(){

			Bounds newBound;
			newBound.nw = { m_nw_corner.toVector() };
			newBound.sw = { m_nw_corner.x, m_se_corner.y, m_sw_z };
			newBound.se = { m_se_corner.toVector() };
			newBound.ne = { m_se_corner.x, m_nw_corner.y, m_ne_z };
			return newBound;
		}


	private:
		void load( nav_buffer& buffer );

		std::uint16_t m_place = 0;

		std::uint32_t m_id = 0,
			m_attribute_flags = 0;
	
		float m_light_intensity[ 4 ] = { 0.f };
		float m_earliest_occupy_time[ 2 ] = { 0.f };
		nav_area_bind_info_t m_inherit_visibility_from = { };

		std::vector< nav_connect_t > m_connections = { };		
		std::vector< nav_hiding_spot > m_hiding_spots = { };
		std::vector< nav_spot_encounter_t > m_spot_encounters = { };
		std::vector< nav_ladder_connect_t > m_ladder_connections[ 2 ] = { };
		std::vector< nav_area_bind_info_t > m_potentially_visible_areas = { };
	};
}