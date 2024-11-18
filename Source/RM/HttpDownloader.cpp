#include "HttpDownloader.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/Paths.h"


UHttpDownloader::UHttpDownloader() {
	HttpModule = &FHttpModule::Get();
}

void UHttpDownloader::ExecuteDownload(FString sSourceURL, FString sPath, FString sName)
{
	this->SourceURL = sSourceURL;
	this->Path = sPath;
	this->Name = sName;

	auto HttpRequest = HttpModule->CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(this->SourceURL);
	HttpRequest->OnRequestProgress().BindUObject(this, &UHttpDownloader::HttpRequestProgressDelegate);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpDownloader::HttpRequestFinishedDelegate);

	HttpRequest->ProcessRequest();
}

void UHttpDownloader::HttpRequestProgressDelegate(FHttpRequestPtr RequestPtr, int32 SendBytes, int32 RecvBytes)
{
	int32 TotalSize = RequestPtr->GetResponse()->GetContentLength();
	float Percent = (float)RecvBytes / TotalSize;

	OnDownloadProcessCallback.Broadcast(RecvBytes, TotalSize, Percent);
}

void UHttpDownloader::HttpRequestFinishedDelegate(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	OnJsonDownloadFinishedCallback.Broadcast(Response->GetContentAsString());
	/*

	FString BasePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + Path;
	FString FileSavePath = BasePath + Name;		// 경로를 지정 한다.

	// Save File.
	if (Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode())) {
		// 해당 플렛폼에 맞는 File 클래스 만들기.
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		// Dir tree 를 만든다.
		PlatformFile.CreateDirectoryTree(*BasePath);
		IFileHandle* FileHandler = PlatformFile.OpenWrite(*FileSavePath);
		if (FileHandler) {
			// 파일을 새로 쓴다.
			FileHandler->Write(Response->GetContent().GetData(), Response->GetContentLength());
			FileHandler->Flush();		// 저장.

			delete FileHandler;
			bWasSuccessful = true;
		}
		else {
			bWasSuccessful = false;
		}
	}
	else {
		bWasSuccessful = false;
	}

	OnDownloadFinishedCallback.Broadcast(bWasSuccessful, FileSavePath);
	*/
}