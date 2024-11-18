#include "CCSRadioButton.h"
#include "Components/Button.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SButton.h"
#include "Components/ButtonSlot.h"

UCCSRadioButton::UCCSRadioButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer){
}

TSharedRef<SWidget> UCCSRadioButton::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	MyButton->SetOnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandle2Clicked));
	return Widget;
}

FReply UCCSRadioButton::SlateHandle2Clicked()
{
	if (ButtonGroup > -1) {
		bool Exist = false;

		UPanelWidget* Parent = GetParent();
		if (Parent) {
			int n = Parent->GetChildrenCount();
			for (int i = 0; i < n; i++) {
				UCCSRadioButton* Button = Cast<UCCSRadioButton>(Parent->GetChildAt(i));
				if (Button) {
					if (this != Button) {
						if (ButtonGroup == Button->ButtonGroup) {
							Button->SetIsEnabled(true);
							Exist = true;
						}
					}
				}
			}
		}
		if (Exist) {
			SetIsEnabled(false);
		}
	}
	return SlateHandleClicked();
}