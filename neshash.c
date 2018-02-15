// 
// code to calc the "hash" on nes/snes mini settings file
// 
// snes mini
// the hash over: "/var/lib/clover/profiles/0/home-menu/save/system-save.json/data"
// is stored in:  "/var/lib/clover/profiles/0/home-menu/save/system-save.json/hash"
// 
// nes mini
// the hash over: "/var/lib/clover/profiles/0/home-menu/save/system-save.json"
// is stored in:  "/var/lib/clover/profiles/0/home-menu/save/system-save.json.hash"
// 
// xorloser  -  feb 1028
// 

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

u64 g_table[256] = {0};
int g_table_is_setup = 0;


void hash_table_setup()
{
	for(int i=0; i<256; i++)
	{
		u64 val = i;
		for(int j=0; j<8; j++)
		{
			if( val & 1 )
			{
				val = val >> 1;
				val ^= 0xD800000000000000ULL;
			}
			else
			{
				val >>= 1;
			}
		}
		
		g_table[i] = val;
	}
}


u64 hash_process(u8* pData, u32 dataSize)
{
	u32 i;
	u64 hash;
	
	if( !g_table_is_setup )
	{
		hash_table_setup();
		g_table_is_setup = 1;
	}
	
	hash = 0;
	for(i=0; i<dataSize; i++)
	{
		hash = g_table[ (hash ^ pData[i])&0xFF ] ^ (hash >> 8);
	}
	return hash;
}



int main(int argc, char* argv[])
{
	if( argc != 2 )
	{
		printf("neshash v1.0 - xorloser feb 2018\n");
		printf("Usage: neshash <data filename>\n");
		return 1;
	}
	
	const char* p_filename = argv[1];
	FILE* fd = fopen(p_filename, "rb");
	if( fd == NULL )
	{
		printf("Error opening %s\n", p_filename);
		return 2;
	}
	fseek(fd, 0, SEEK_END);
	int data_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	u8* p_data = malloc(data_size);
	fread(p_data, 1, data_size, fd);
	
	// we don't print a newline character here because the 'hash' files
	// don't have any newline character in them, so this way we can put
	// the output directly into the hash file.
	u64 hash = hash_process(p_data, data_size);
	printf("%016llX", hash);
	
	fclose(fd);
	free(p_data);
	return 0;
}

