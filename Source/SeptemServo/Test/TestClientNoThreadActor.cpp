// Copyright (c) 2013-2019 7Mersenne All Rights Reserved.

#include "TestClientNoThreadActor.h"

#include "../Protocol/ServoProtocol.h"

// Sets default values
ATestClientNoThreadActor::ATestClientNoThreadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ServerIP = TEXT("127.0.0.1");
	Port = 3717;

	FIPv4Address ServerIPAdress;
	FIPv4Address::Parse(ServerIP, ServerIPAdress);
	ServerEndPoint.Address = ServerIPAdress;
	ServerEndPoint.Port = Port;

	ClientSocket = nullptr;
	bStartGameConnect = false;
	ClientState = 0;
}

// Called when the game starts or when spawned
void ATestClientNoThreadActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (bStartGameConnect)
	{
		ConnectToServer();
	}
}

void ATestClientNoThreadActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseSocket();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATestClientNoThreadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nullptr != ClientSocket)
	{
		// detect server is connect
		if (ClientSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
		{
			// safe disconnect
			ReleaseSocket();
		}
	}
}

void ATestClientNoThreadActor::ConnectToServer()
{
	if (nullptr == ClientSocket)
	{
		// 1. create socket
		ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ClientNoThread"), false);

		// 2. connect socket
		TSharedRef<FInternetAddr> ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr(ServerEndPoint.Address.Value, ServerEndPoint.Port);
		bool bConnect = ClientSocket->Connect(*ServerAddr);

		if (!bConnect)
		{
			UE_LOG(LogTemp, Display, TEXT("ATestClientNoThreadActor: connect to server failed! \n"));
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
			ClientSocket = nullptr;
		}
		else {
			ClientState = 1;
		}
	}
}

void ATestClientNoThreadActor::Disconnect()
{
	ReleaseSocket();
}

void ATestClientNoThreadActor::ReleaseSocket()
{
	if (nullptr != ClientSocket)
	{
		ClientSocket->Shutdown(ESocketShutdownMode::ReadWrite);
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
		ClientSocket = nullptr;
		ClientState = 0;
		UE_LOG(LogTemp, Display, TEXT("ATestClientNoThreadActor: client socket close \n"));
	}
}

void ATestClientNoThreadActor::SendByte(uint8 InByte)
{
	if (ClientSocket)
	{
		int32 bytesSend = 0;
		static TArray<uint8> buffer;
		
		buffer.Reset(1024);
		buffer.Add(InByte);
		if (ClientSocket->Send(buffer.GetData(), buffer.Num(), bytesSend))
		{
			UE_LOG(LogTemp, Display, TEXT("ATestClientNoThreadActor: socket send  %d bytes done. buffer num = %d \n"), bytesSend, buffer.Num());
		}
		else {
			ReleaseSocket();
		}
	}
}

void ATestClientNoThreadActor::SendByteUDP(uint8 InByte)
{
	if (ClientSocket)
	{
		int32 bytesSend = 0;
		static TArray<uint8> buffer;

		buffer.Reset(1024);
		buffer.Add(InByte);

		TSharedRef<FInternetAddr> ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr(ServerEndPoint.Address.Value, ServerEndPoint.Port);

		if (ClientSocket->SendTo(buffer.GetData(), buffer.Num(), bytesSend, *ServerAddr))
		{
			UE_LOG(LogTemp, Display, TEXT("ATestClientNoThreadActor: socket udp send done. \n"));
		}
		else {
			ReleaseSocket();
		}
	}
}

void ATestClientNoThreadActor::SendHeartbeat()
{
	if (ClientSocket)
	{
		FSNetPacket* ptr = FSNetPacket::CreateHeartbeat();
		TSharedPtr<FSNetPacket, ESPMode::ThreadSafe> ptrPacket(ptr);

		int32 bytesSend = 0;
		TArray<uint8> buffer;

		buffer.Reset(1024);

		if(ptrPacket.Get())
			ptrPacket.Get()->WriteToArray(buffer);

		if (ClientSocket->Send(buffer.GetData(), buffer.Num(), bytesSend))
		{
			UE_LOG(LogTemp, Display, TEXT("ATestClientNoThreadActor: socket heartbeat send  %d bytes done. buffer num = %d \n"), bytesSend, buffer.Num());
		}
		else {
			ReleaseSocket();
		}
	}
}

