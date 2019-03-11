// Copyright (c) 2013-2019 7Mersenne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Private/HAL/PThreadRunnableThread.h"
#include "Networking.h"
#include "ConnectThreadPoolThread.h"

 /**
 * litsen runnable for server thread
 */
class SEPTEMSERVO_API FListenThread : public FRunnable
{
public:
	FListenThread();
	virtual ~FListenThread();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	// End FRunnable interface

	//~~~ Starting and Stopping Thread ~~~

	/** Makes sure this thread has stopped properly */
	// must use KillThread to void deadlock
	// if you use thread->kill() directly , easy to get deadlock or crash
	bool KillThread();// use KillThread instead of thread->kill
	static FListenThread* Create(int32 InPort = 3717, float InPoolTimespan = 0.05f);

	int32 GetLifecycleStep();
	int32 GetPoolLifecycleStep();

	// [Dangerous call] only for debug info
	FConnectThreadPoolThread* GetPoolThread();
	int32 GetRankID();
private:
	//---------------------------------------------
	// thread control
	//---------------------------------------------

	/** If true, the thread should exit. */
	TAtomic<bool> TimeToDie;  //check in run()

	// if ture means we had called stop();
	FThreadSafeBool bStopped; // check stop()

	FThreadSafeCounter LifecycleStep;
	//TAtomic<bool> bPause;  //or FThreadSafeBool bPause;
	//FEvent * Semaphore;

	//---------------------------------------------
	// server config
	//---------------------------------------------
	//FIPv4Endpoint ServerIPv4EndPoint;
	FIPv4Address IPAdress;
	int32 Port;
	int32 MaxBacklog;				// max count of client
	float PoolTimespan;

	// socket
	FSocket* ListenerSocket;
	void SafeDestorySocket();

	// server listen thread
	FRunnableThread* Thread;

	//---------------------------------------------
	// client connections
	//---------------------------------------------
	int32 RankId;	// consider volatile 

	FConnectThreadPoolThread* ConnectionPoolThread;
	void SafeConstructConnectionPool();
	void SafeDestructConnectionPool();
};
