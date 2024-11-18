#include "RMDefinedStruct.h"


FString FPlayerCharacter::ToJson()
{
	int i = 0;
	int genderint = 0;
	if (Gender ==EGenderRM::Male){genderint=0;}
	else if (Gender == EGenderRM::Female){genderint = 1;}
	else if (Gender == EGenderRM::Male2){genderint=2;}
	else if (Gender == EGenderRM::Female2){genderint=3;}
	else if (Gender  == EGenderRM::Male3){genderint=4;}
	else if (Gender == EGenderRM::Female3){genderint=5;}
	else if (Gender == EGenderRM::Male4){genderint=6;}
	else if (Gender == EGenderRM::Female4){genderint=7;}
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	// TSharedPtr<FJsonObject> BaseJsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("gender", FString::Printf(TEXT("%d"), genderint));
	
	JsonObject->SetStringField("topSize", FString::Printf(TEXT("%.2f"), TopSize));
	JsonObject->SetStringField("bottomSize", FString::Printf(TEXT("%.2f"), BottomSize));
	JsonObject->SetStringField("cheekSize", FString::Printf(TEXT("%.2f"), CheekSize));
	JsonObject->SetStringField("tallScale", FString::Printf(TEXT("%.2f"), Scale));
	JsonObject->SetStringField("headScale", FString::Printf(TEXT("%.2f"), HeadScale));
	JsonObject->SetStringField("handScale", FString::Printf(TEXT("%.2f"), HandScale));
	JsonObject->SetStringField("skinColor", SkinColor.ToFColor(true).ToHex());
	// JsonObject->SetObjectField("BaseInfo", BaseJsonObject);

	// TSharedPtr<FJsonObject> EyeJsonObject = MakeShareable(new FJsonObject());
	i = 0;
	for (auto It : EyeMaterialScalarParameter)
	{
		FString str = EnumToString(TEXT("EEyeMaterialScalarParameter"), i++).Replace(TEXT(" "), TEXT(""), ESearchCase::Type::IgnoreCase);
		str[0] = std::tolower(str[0]);
		JsonObject->SetStringField(str, FString::Printf(TEXT("%.2f"), It.Value));
	}
	// JsonObject->SetObjectField("EyeInfo", EyeJsonObject);

	// TSharedPtr<FJsonObject> CostumeJsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("head", Costume[(ECostumePart)0].ToString());
	JsonObject->SetStringField("hair", Costume[(ECostumePart)1].ToString());
	JsonObject->SetStringField("top", Costume[(ECostumePart)2].ToString());
	JsonObject->SetStringField("bottom", Costume[(ECostumePart)3].ToString());
	JsonObject->SetStringField("shoes", Costume[(ECostumePart)4].ToString());
	// JsonObject->SetStringField("faceSkin", Costume[(ECostumePart)5].ToString());
	//JsonObject->SetStringField("eyeBrow", Costume[(ECostumePart)6].ToString());
	//JsonObject->SetStringField("lip", Costume[(ECostumePart)7].ToString());
	// JsonObject->SetObjectField("CostumeInfo", CostumeJsonObject);

	i = 0;
	for (auto It : MorphTargetValue)
	{
		FString str = EnumToString(TEXT("EMorphTarget"), i++);
		str[0] = std::towlower(str[0]);
		str = str.Replace(TEXT(" "), TEXT(""), ESearchCase::Type::IgnoreCase);
		JsonObject->SetStringField(str, FString::Printf(TEXT("%.2f"), It.Value));
	}

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	return JsonString;
}
