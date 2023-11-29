/*
MIT License

Copyright (c) 2021 IntriguingTiles

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "hooks.h"
#include "sigscanner.h"

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <gelf.h>

#include <unordered_map>
#include <string_view>

typedef DWORD u32;

u32 engineAddr = 0;
std::unordered_map<std::string_view, u32> engineSymbols;

bool getModuleAddress(const char* lib, u32* startOfModule, u32* endOfModule = nullptr);

void readSymbols(const char* lib, std::unordered_map<std::string_view, u32>& symbols)
{
	elf_version(EV_CURRENT);
	int fd = open(lib, O_RDONLY);
	Elf* elf = elf_begin(fd, ELF_C_READ, nullptr);
	Elf_Scn* scn = nullptr;
	GElf_Shdr shdr;

	while ((scn = elf_nextscn(elf, scn)) != nullptr)
	{
		gelf_getshdr(scn, &shdr);

		if (shdr.sh_type == SHT_SYMTAB)
			break;
	}

	Elf_Data* data = elf_getdata(scn, nullptr);
	int count = shdr.sh_size / shdr.sh_entsize;

	for (int i = 0; i < count; i++)
	{
		GElf_Sym sym;
		gelf_getsym(data, i, &sym);
		symbols.insert({elf_strptr(elf, shdr.sh_link, sym.st_name), sym.st_value});
	}

	elf_end(elf);
	close(fd);
}

// this sucks but it works for now
bool getModuleAddress(const char* lib, u32* startOfModule, u32* endOfModule)
{
	FILE* maps = fopen("/proc/self/maps", "r");
	char line[PATH_MAX];
	u32 start;
	u32 end;
	char permissions[5];
	int offset;
	char dev[12];
	int inode;
	char path[PATH_MAX];

	while (fgets(line, PATH_MAX, maps))
	{
		// clear the path in case there isn't one on this line
		memset(path, 0, PATH_MAX);
		int ret = sscanf(line, "%x-%x %s %x %s %d %s", &start, &end, permissions, &offset, dev, &inode, path);
		if (strstr(path, lib))
		{
			if (permissions[2] = 'x')
			{
				fclose(maps);
				if (startOfModule)
					*startOfModule = start;
				if (endOfModule)
					*endOfModule = end;
				return true;
			}
		}
	}

	fclose(maps);
	return false;
}

u32 getEngineSymbol(const char* symbol)
{
	return engineSymbols[symbol] + engineAddr;
}

// For getting information about the executing module
bool SigScan::GetModuleInfo(char* szModule)
{
	base = (void*)dlopen(szModule, RTLD_NOW);
	readSymbols(szModule, engineSymbols);
	getModuleAddress(szModule, &engineAddr);

	return base != nullptr;
}

void SigScan::FindFunction(DWORD* result, Pattern pattern)
{
	void *ptr = getEngineSymbol(pattern.name);

	if (!ptr)
		Sys_Error("Could not find function : %s", pattern.name);

	*result = (DWORD)ptr;
}