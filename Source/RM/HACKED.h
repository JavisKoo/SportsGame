// HACKED.h (모듈최상위 헤더)

// 이 함수가 호출된 함수의 이름과 라인을 매크로로 String화 시킴 - 출처 : 이득우의 언리얼 C++ 게임 개발의 정석
#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

// 위의 CALLINFO와 함께 아무 내용이 없는 로그를 찍는 매크로 - 출처 : 이득우의 언리얼 C++ 게임 개발의 정석
#define LOG_S(Verbosity) UE_LOG(HACKED_LOG, Verbosity, TEXT("%s"), *LOG_CALLINFO)

// CALLINFO와 함께 텍스트로 로그를 찍는 매크로
#define LOG(Verbosity, Format, ...) UE_LOG(HACKED_LOG, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

// Screen에다가 LOG를 직접 찍는 매크로
#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

// 위의 함수에 시간을 넣는 매크로. Tick에서 DeltaTime을 인자로 넣으면 보기 편함.
#define	LOG_SCREEN_T(Time, Format, ...) GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

// LOG에 Verbosity를 인자에서 빼고 매크로화 시킨것. 
#define LOG_WARNING(Format, ...) UE_LOG(HACKED_LOG, Warning, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_ERROR(Format, ...) UE_LOG(HACKED_LOG, Error, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

// assert()와 거의 비슷한 용도로 사용가능한 매크로. ...에는 retval이 들어감.
#define CHECK(Expr, ...) {if(!(Expr)) {LOG(Error, "ASSERTION : %s", "'"#Expr"'"); return __VA_ARGS__;}}


