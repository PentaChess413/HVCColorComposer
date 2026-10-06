// Probably will crash and burn in PPC
#include "SpDebug.h"

void MacLog(char *format)
{
	NSLog(@"%s", format);
}