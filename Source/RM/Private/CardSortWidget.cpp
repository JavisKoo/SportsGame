// Fill out your copyright notice in the Description page of Project Settings.


#include "CardSortWidget.h"

void UCardSortWidget::SortByName(const TMap<int32, FString>& InputMap, const bool isDescending, TMap<int32, FString>& OutMap)
{
	TMap<int32, FString> OriginMap = InputMap;

	OriginMap.ValueStableSort([isDescending](const FString& A, const FString& B)
		{
			if (isDescending)
			{

				return A > B;
			}
			else
			{
				return A < B;
			}
		}
	);
	OutMap = OriginMap;
}

void UCardSortWidget::SortByPower(const TMap<int32, int32>& InputMap, const bool isDescending, TMap<int32, int32>& OutMap)
{
	TMap<int32, int32> OriginMap = InputMap;
	OriginMap.ValueStableSort([isDescending](const int32& A, const int32& B)
		{
			if (isDescending)
			{
				return A > B;
			}
			else
			{
				return A < B;
			}
		}
	);

	OutMap = OriginMap;

}

void UCardSortWidget::SortByPosition()
{
}
