#pragma once

class Pattern
{
public:
	// Will only support latest engine build
	const char* name;
	const char* pattern;
	const char* mask;
	const char* build = "9899";
};

namespace patterns
{
const Pattern EmitWaterPolys = {
	"EmitWaterPolys",
	"\x55\x8B\xEC\x83\xEC\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x56\xF3\x0F\x10\x15", 
	"xxxxx?x????xxxx?xxxxx"};
const Pattern gl_texturemode_hook_callback = {
	"gl_texturemode_hook_callback",
	"\x55\x8B\xEC\x8B\x45\x00\x85\xC0\x0F\x84\x00\x00\x00\x00\x8B\x40",
	"xxxxx?xxxx????xx"};
}