#include "ThirdParty/Utility.h"

FString EnumToString(const TCHAR* Enum, int32 EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
	if (!EnumPtr)
		return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();

	
#if WITH_EDITOR
	return EnumPtr->GetNameStringByIndex(EnumValue);
	// return EnumPtr->GetDisplayNameText(EnumValue).ToString();
#else
	return EnumPtr->GetNameStringByIndex(EnumValue);
	// return EnumPtr->GetEnumName(EnumValue);
#endif
}
