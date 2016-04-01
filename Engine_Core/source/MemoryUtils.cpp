#include "MemoryUtils.h"


bool IsBigEndian()
{
	static char SwapTest[2] = { 1, 0 };
	return ((*(unsigned short*)SwapTest) != 1);
}

void SwapBytes(char* Bytes, int BytesCount)
{
	char* Start = Bytes;
	char* End = Bytes + BytesCount - 1;
	char Temp;
	while (Start < End)
	{
		Temp = *Start;
		*Start++ = *End;
		*End-- = Temp;
	}
}
