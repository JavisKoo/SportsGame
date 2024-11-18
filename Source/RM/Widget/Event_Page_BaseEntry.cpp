// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_Page_BaseEntry.h"
#include "Engine.h"

void UEvent_Page_BaseEntry::NativeConstruct()
{
	Super::NativeConstruct();

	AdvanceWebBrowser = Cast<UAdvanceWebBrowser>(GetWidgetFromName(TEXT("WebBrowser")));
}

void UEvent_Page_BaseEntry::LoadURL(FString URL, FString BindObjectName, UObject* Object, bool bIsPermenent)
{
	if (AdvanceWebBrowser == nullptr)
	{
		AdvanceWebBrowser = Cast<UAdvanceWebBrowser>(GetWidgetFromName(TEXT("WebBrowser")));
		if (AdvanceWebBrowser == nullptr)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("AdvancedWebBrowser is null"));
			return;	
		}
	}
	AdvanceWebBrowser->LoadURL(URL);
	BindObjectName = (BindObjectName.IsEmpty()) ? TEXT("obj") : BindObjectName;
	
	if (BindObject != nullptr)
	{
		/* Unbind�� ��Ű�� �ٸ� ������ �Ű��� ���� �˾��� �������� �� ������ �� �� ���� �ʳ�?
		 * - Unbind��ſ� WidgetBP���� ������ UObject Class ������ ��� �ִٰ� Destroy()�� �����Ѵ�?
		 */
		AdvanceWebBrowser->UnbindUObject(BindObjectName, BindObject);
		if (BindObject != this)
			BindObject->ConditionalBeginDestroy();	
	}
		
	Object = (Object == nullptr) ? this : Object;
	BindObject = Object;
	AdvanceWebBrowser->BindUObject(BindObjectName, BindObject, bIsPermenent);	
}

void UEvent_Page_BaseEntry::LoadURLButtonWeb(FString URL, FString BindObjectName, UObject* Object, bool bIsPermenent)
{
	if (AdvanceWebBrowserWithButton == nullptr)
	{
		AdvanceWebBrowserWithButton = Cast<UAdvanceWebBrowser>(GetWidgetFromName(TEXT("WebBrowser1")));
		if (AdvanceWebBrowserWithButton == nullptr)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("AdvancedWebBrowser is null"));
			return;
		}
	}
	AdvanceWebBrowserWithButton->LoadURL(URL);
	BindObjectName = (BindObjectName.IsEmpty()) ? TEXT("obj") : BindObjectName;

	if (BindObject != nullptr)
	{
		/* Unbind�� ��Ű�� �ٸ� ������ �Ű��� ���� �˾��� �������� �� ������ �� �� ���� �ʳ�?
		 * - Unbind��ſ� WidgetBP���� ������ UObject Class ������ ��� �ִٰ� Destroy()�� �����Ѵ�?
		 */
		AdvanceWebBrowserWithButton->UnbindUObject(BindObjectName, BindObject);
		if (BindObject != this)
			BindObject->ConditionalBeginDestroy();
	}

	Object = (Object == nullptr) ? this : Object;
	BindObject = Object;
	AdvanceWebBrowserWithButton->BindUObject(BindObjectName, BindObject, bIsPermenent);
}