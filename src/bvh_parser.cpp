#include "bvh_parser.h"

BVHParser::BVHParser(Ref<FileAccess> bvh_file)
{
	length = bvh_file->get_length();
	source = bvh_file->get_buffer(length);
	start = &source[0];
	curr = start;
}

bool BVHParser::parse()
{
	if (_parse_hierarchy() && _parse_motion()) {
		vformat("Succeeded BVH parsing");
		return true;
	}

	vformat("Failed BVH Parsing");
	return false;
}

void BVHParser::_skip_whitespace()
{
	for (;;) {
		switch (*curr) {
		case ' ':
		case '\r':
		case '\t':
		case '\n':
			curr++;
			continue;
		}
		break;
	}
}

bool BVHParser::_is_at_end()
{
	return (curr - &source[0]) < length;
}

bool BVHParser::_expect(const String& str)
{
	return false;
}

bool BVHParser::_expect(uint8_t c)
{
	return false;
}

uint8_t BVHParser::_peek()
{
	return 0;
}

uint8_t BVHParser::_advance()
{
	return 0;
}

bool BVHParser::_parse_hierarchy()
{
	return false;
}

bool BVHParser::_parse_root()
{
	return false;
}

bool BVHParser::_parse_joint()
{
	return false;
}

bool BVHParser::_parse_motion()
{
	return false;
}
