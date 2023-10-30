#pragma warning(disable:4996)
#include <stdio.h>
#include <Windows.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#define SAFE_DELETE(ptr)                 do { free(ptr); (ptr) = NULL; } while(0)
#define CAPPING(char_array)              strupr(char_array)

#define SND_FREQUENCY					400
#define SND_FREQUENCY_NONE				0
#define SND_GAP							100
#define SND_SEP_CODE					50
#define SND_SEP_LETTER					150
#define SND_SEP_WORD					300
#define SND_BAR_RCV						{Beep(SND_FREQUENCY, SND_GAP * 3);	Beep(SND_FREQUENCY_NONE, SND_SEP_CODE);}
#define SND_DOT_RCV						{Beep(SND_FREQUENCY, SND_GAP);		Beep(SND_FREQUENCY_NONE, SND_SEP_CODE);}
#define SND_END_OF_LETTER				Beep(0, SND_SEP_LETTER);
#define SND_END_OF_WORD					Beep(0, SND_SEP_WORD);

int main()
{
	SND_BAR_RCV
		SND_BAR_RCV
		SND_DOT_RCV
		SND_BAR_RCV
		return 0;
}