// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HXVideoStreamNetClientActor.generated.h"

class HXStreamNetClient;
class FRunnable;
class FRunnableThread;

UCLASS()
class HXVIDEORECEIVERPLUGIN_API AHXVideoStreamNetClientActor : public AActor, public FRunnable
{

	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this actor's properties
	AHXVideoStreamNetClientActor();

	~AHXVideoStreamNetClientActor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//视频线程相关函数
	virtual uint32 Run() override;
	virtual bool Init() override;
	virtual void Stop() override;
	virtual void Exit() override;

	//连接服务器
	UFUNCTION(BlueprintCallable, Category = "HXVideoStreamNetClient")
	bool Connect(const FString& ip, const FString& port);

	//关闭与服务器的连接
	UFUNCTION(BlueprintCallable, Category = "HXVideoStreamNetClient")
	void Close();

	//获得动态图像数据
	UFUNCTION(BlueprintCallable, Category = "HXVideoStreamNetClient")
	UTexture2D* GetLiveTexture();

private:
	void GetVideoStreamDataFromSvr();
	void RefreshTexture2D();
	bool CreateDynamicTexture(int32 tw, int32 th);

private:
	//视频服务器相关
	HXStreamNetClient		*mpNetClient;
	FString						m_VideoSvrIP;
	FString						m_VideoSvrPort;
	bool							m_bConnectedVideoSvr;

	//视频帧相关
	UTexture2D				*m_LiveTexture;
	TArray<FColor>			m_PixelColorArray;
	int								m_TextureWidth		= 0;
	int								m_TextureHeight	= 0;
	int								m_TextureSize		= 0;

	//视频线程相关
	FRunnableThread		*m_VideoReceiveThread;
	FCriticalSection			m_Mutex;
	bool							m_bRunVideoThread;
};
