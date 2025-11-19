#pragma once

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>

using namespace godot;

class BVHParser {
public:
	BVHParser(Ref<FileAccess> bvh_file);

	bool parse();

private:
	// parsing character functions
	void _skip_whitespace();
	bool _is_at_end();
	bool _expect(const String& str);
	bool _expect(uint8_t c);
	uint8_t _peek();
	uint8_t _advance();

	bool _parse_hierarchy();
	bool _parse_root();
	bool _parse_joint();

	bool _parse_motion();

	uint8_t* start;
	uint8_t* curr;
	PackedByteArray source;
	uint64_t length;
};