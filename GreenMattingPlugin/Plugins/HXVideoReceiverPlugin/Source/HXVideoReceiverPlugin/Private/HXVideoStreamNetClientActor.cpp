// Fill out your copyright notice in the Description page of Project Settings.

#include "HXVideoStreamNetClientActor.h"
#include "Engine/Texture2D.h"
#include "Misc/ConfigCacheIni.h"
#include "HXStreamNetClient.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

// Sets default values
AHXVideoStreamNetClientActor::AHXVideoStreamNetClientActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FString VipConfigFile = FPaths::ProjectConfigDir() + TEXT("LiveStreamConfig.ini");

	// 从配置文件中读取视频流服务器的IP地址和端口号
	bool ret = GConfig->GetString(TEXT("LiveAddress"), TEXT("IP"), m_VideoSvrIP, VipConfigFile);
	if (!ret)
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to get IP address from LiveStreamConfig.ini file."));
	}

	ret = GConfig->GetString(TEXT("LiveAddress"), TEXT("Port"), m_VideoSvrPort, VipConfigFile);
	if (!ret)
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to get Port number from LiveStreamConfig.ini file."));
	}

	//
	mpNetClient = CreateVideoStreamNetClientService();

	m_bRunVideoThread = false;
	m_bConnectedVideoSvr = false;
}


AHXVideoStreamNetClientActor::~AHXVideoStreamNetClientActor()
{
	if (m_VideoReceiveThread)
	{
		delete m_VideoReceiveThread;
		m_VideoReceiveThread = nullptr;
	}

	if(mpNetClient)
	{
		delete mpNetClient;
		mpNetClient = nullptr;
	}

	//if (m_LiveTexture)
	//{
	//	m_LiveTexture->ConditionalBeginDestroy();
	//	m_LiveTexture = NULL;
	//}

	m_bRunVideoThread = false;
	m_bConnectedVideoSvr = false;
}

// Called when the game starts or when spawned
void AHXVideoStreamNetClientActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHXVideoStreamNetClientActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshTexture2D();
}


bool AHXVideoStreamNetClientActor::Init()
{
	m_PixelColorArray.Empty();

	m_bRunVideoThread = true;
	
	return true;
}


uint32 AHXVideoStreamNetClientActor::Run()
{
	
	while (1)
	{
		if (!m_bRunVideoThread )
		{
			break;
		}

		if (m_bConnectedVideoSvr)
		{
			GetVideoStreamDataFromSvr();
		}

		FPlatformProcess::Sleep(0.03f);
	}

	return 0;
}

void AHXVideoStreamNetClientActor::Stop()
{
	m_bRunVideoThread = false;
}

void AHXVideoStreamNetClientActor::Exit()
{
	
}

bool AHXVideoStreamNetClientActor::Connect(const FString& ip, const FString& port)
{
	if (!mpNetClient)
	{
		
		UE_LOG(LogTemp, Log, TEXT("Failed to create HXVideoStreamNetClient object."));
		return false;
	}

	m_VideoSvrIP = ip;
	m_VideoSvrPort = port;

	FTCHARToUTF8 cIP(*m_VideoSvrIP);
	int32 iport = FCString::Atoi(*m_VideoSvrPort);
	
	m_bConnectedVideoSvr =  mpNetClient->Connect((char*)cIP.Get(), iport);
	
	if (!m_bConnectedVideoSvr)
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to connect server %s %s."), *m_VideoSvrIP, *m_VideoSvrPort);
	}
	else
	{
		//创建视频连接线程
		m_VideoReceiveThread = FRunnableThread::Create((FRunnable*)this, TEXT("GetVideoStreamDataThread"), 0, TPri_BelowNormal);

	}

	return m_bConnectedVideoSvr;
}


void AHXVideoStreamNetClientActor::GetVideoStreamDataFromSvr()
{

	if (mpNetClient == NULL)
	{
		UE_LOG(LogTemp, Log, TEXT("HXVideoStreamNetClient Object is NULL"));
		return;
	}

	int index = 0;
	uint8_t* pRawData = NULL;

	bool ret = mpNetClient->GetStreamData(&pRawData, m_TextureWidth, m_TextureHeight, m_TextureSize);
	if (ret)
	{
		{
			FScopeLock ScopeLock(&m_Mutex);
			m_PixelColorArray.Empty();
			for (int i = 0; i < m_TextureSize / 3; ++i)
			{
				uint8 B = pRawData[index + 0];
				uint8 G = pRawData[index + 1];
				uint8 R = pRawData[index + 2];
				FColor pixelValue(B, G, R, 0);
				m_PixelColorArray.Add(pixelValue);

				index += 3;
			}
		}
	}
}


void AHXVideoStreamNetClientActor::RefreshTexture2D()
{
	if (!m_bRunVideoThread || !m_bConnectedVideoSvr)
	{
		return;
	}

	FScopeLock ScopeLock(&m_Mutex);

	if (m_LiveTexture == NULL && m_TextureWidth >0 && m_TextureHeight > 0)
	{
		CreateDynamicTexture(m_TextureWidth, m_TextureHeight);
	}

	if (m_LiveTexture && m_PixelColorArray.Num() > 0)
	{
		//将RGB像素数据写到Texture2D
		FColor* MipData = static_cast<FColor*>(m_LiveTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
		FMemory::Memcpy(MipData, m_PixelColorArray.GetData(), m_TextureSize/3*4);
		m_LiveTexture->PlatformData->Mips[0].BulkData.Unlock();

		m_LiveTexture->UpdateResource();
	}
}


void AHXVideoStreamNetClientActor::Close()
{
	if (mpNetClient)
	{
		mpNetClient->Close();
	}
}


UTexture2D* AHXVideoStreamNetClientActor::GetLiveTexture()
{
	FScopeLock ScopeLock(&m_Mutex);

	if (m_LiveTexture)
	{
		return m_LiveTexture;
	}

	return NULL;

}

bool AHXVideoStreamNetClientActor::CreateDynamicTexture(int32 tw, int32 th)
{
	if (m_LiveTexture == NULL ||
		m_LiveTexture->PlatformData == NULL ||
		m_LiveTexture->PlatformData->SizeX != tw ||
		m_LiveTexture->PlatformData->SizeY != th)
	{
		m_LiveTexture = UTexture2D::CreateTransient(tw, th, PF_B8G8R8A8);
	}
	return true;
}
