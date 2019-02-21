// Copyright (c) 2013-2019 7Mersenne All Rights Reserved.

#include "ListenThread.h"

FListenThread::FListenThread()
	:FRunnable()
	,TimeToDie(false)
	, bStopped(false)
	, IPAdress(0ui32) // default ip = 0.0.0.0
	,Port(3717)
	,Thread(nullptr)
{
}

FListenThread::~FListenThread()
{
	// cleanup thread
	if (nullptr != Thread)
	{
		delete Thread;
		Thread = nullptr;
	}
	// cleanup events
	// null events here
}

bool FListenThread::Init()
{
	return false;
}

uint32 FListenThread::Run()
{
	//FPlatformMisc::MemoryBarrier();
	return 0;
}

void FListenThread::Stop()
{
	if (!bStopped) {
		TimeToDie = true;
		// call father function
		//FRunnable::Stop(); // father function == {}

		// because pthread->kill will call stop
		// you cannot call pthread->kill here

		bStopped = true;
	}
}

void FListenThread::Exit()
{
	UE_LOG(LogTemp, Display, TEXT("FListenThread: exit()\n"));
}

/**
 * Tells the thread to exit. If the caller needs to know when the thread
 * has exited, it should use the bShouldWait value and tell it how long
 * to wait before deciding that it is deadlocked and needs to be destroyed.
 * NOTE: having a thread forcibly destroyed can cause leaks in TLS, etc.
 *
 * @return True if the thread exited graceful, false otherwise
 */
bool FListenThread::KillThread()
{
	bool bDidExit = true;

	TimeToDie = true;

	if (nullptr != Thread)
	{
		// Trigger the thread so that it will come out of the wait state if
		// it isn't actively doing work
		//if(event) event->Trigger();

		// If waiting was specified, wait the amount of time. If that fails,
		// brute force kill that thread. Very bad as that might leak.
		Thread->WaitForCompletion();
		
		// Clean up the event
		// if(event) FPlatformProcess::ReturnSynchEventToPool(event);
		// event = nullptr;

		// here will call Stop()
		delete Thread;
		Thread = nullptr;
	}

	return bDidExit;
}

FListenThread * FListenThread::Create(int32 InPort)
{
	// if you need create event
	//Event = FPlatformProcess::GetSynchEventFromPool();

	// create runnable
	FListenThread* runnable = new FListenThread();
	runnable->Port = InPort;

	// create thread with runnable
	FRunnableThread* thread = FRunnableThread::Create(runnable, TEXT("FPrimeNumberWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify 
	
	if (nullptr == thread)
	{
		// create failed
		delete runnable;
		return nullptr;
	}

	// setting thread
	runnable->Thread = thread;
	return runnable;
}
