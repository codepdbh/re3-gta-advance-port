#define WITHWINDOWS
#include "common.h"
#include "crossplatform.h"

#include "DMAudio.h"
#include "Record.h"
#include "Timer.h"

#ifdef GTA_PS2

#include <libpc.h>

uint32 CTimer::m_snTimeInMilliseconds;
uint32 CTimer::m_snTimeInMillisecondsNonClipped;
uint32 CTimer::m_snTimeInMillisecondsPauseMode;
uint32 CTimer::m_snPreviousTimeInMilliseconds;
int32 CTimer::m_snFrameTimeInCycles;
int32 CTimer::m_snFrameTimeInScanLines;
int32 CTimer::m_otherCount;
float CTimer::ms_fTimeScale;
float CTimer::ms_fTimeStep;
float CTimer::ms_fTimeStepNonClipped;
uint32 CTimer::m_FrameCounter;
uint32 CTimer::m_AnimationFrames;
bool CTimer::m_UserPause;
bool CTimer::m_CodePause;

volatile unsigned long sweHighCount;
int skyTimerHandlerHid = -1;

#define CYCLES_PER_MS 294912

void    
CTimer::Initialise(void)
{
	int control;

	m_snTimeInMilliseconds = 1;
	ms_fTimeScale = 1.0f;
	ms_fTimeStep = 1.0f;
	m_UserPause = false;
	m_CodePause = false;
	m_snPreviousTimeInMilliseconds = 0;
	m_snTimeInMillisecondsNonClipped = 0;
	m_snFrameTimeInCycles = 0;
	m_snFrameTimeInScanLines = 0;

	control = SCE_PC0_CPU_CYCLE | (SCE_PC_U0|SCE_PC_S0|SCE_PC_K0|SCE_PC_EXL0);
	control |= SCE_PC1_DCACHE_MISS | (SCE_PC_U1);
	control |= SCE_PC_CTE;

	DI();
	scePcStart(control, 0, 0);
	EI();

	DI();
	scePcStop();
	EI();

	m_FrameCounter = 0;
	DMAudio.ResetTimers(m_snPreviousTimeInMilliseconds);
}

void
CTimer::Shutdown(void)
{
	scePcStop();
}

#define MS_PER_STEP (1000/50)

void
CTimer::Update(void)
{
	int control;

	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
	m_snFrameTimeInCycles = scePcGetCounter0();
	m_otherCount = scePcGetCounter1();
	m_snTimeInMillisecondsPauseMode += m_snFrameTimeInCycles / CYCLES_PER_MS;

	if(m_UserPause || m_CodePause)
		m_snFrameTimeInCycles = 0;

	m_snTimeInMilliseconds += ms_fTimeScale * m_snFrameTimeInCycles / CYCLES_PER_MS;
	m_snTimeInMillisecondsNonClipped += ms_fTimeScale * m_snFrameTimeInCycles / CYCLES_PER_MS;
	ms_fTimeStep = ms_fTimeScale * m_snFrameTimeInCycles / (MS_PER_STEP * CYCLES_PER_MS);
	if(ms_fTimeStep < 0.5f && !m_UserPause && !m_CodePause)
		ms_fTimeStep = 0.5f;
	ms_fTimeStepNonClipped = ms_fTimeStep;

	control = SCE_PC0_CPU_CYCLE | (SCE_PC_U0|SCE_PC_S0|SCE_PC_K0|SCE_PC_EXL0);
	control |= SCE_PC1_DCACHE_MISS | (SCE_PC_U1);
	control |= SCE_PC_CTE;

	DI();
	scePcStart(control, 0, 0);
	EI();

	m_FrameCounter++;

	if(!CRecordDataForGame::IsPlayingBack()){
		if(ms_fTimeStep > 3.0f)
			ms_fTimeStep = 3.0f;
		if(m_snTimeInMilliseconds > m_snPreviousTimeInMilliseconds + 3*MS_PER_STEP)
			m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 3*MS_PER_STEP;
	}
	if(CRecordDataForGame::IsRecording()){
		ms_fTimeStep = 1.0f;
		m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 1000/60;
	}
}

bool CTimer::GetIsSlowMotionActive(void)
{
	return ms_fTimeScale < 1.0f;
}

void CTimer::Stop(void)
{
	DI();
	scePcStop();
	EI();
}

void CTimer::StartUserPause(void)
{
	m_UserPause = true;
	m_AnimationFrames = 12;
}

void CTimer::EndUserPause(void)
{
	m_UserPause = false;
	m_AnimationFrames = 12;
}

// these are actually inlined, but I don't want to include libpc everywhere
uint32 CTimer::GetCyclesPerMillisecond(void) { return CYCLES_PER_MS; }
uint32 CTimer::GetCurrentTimeInCycles(void) { return scePcGetCounter0(); }

#else

uint32 CTimer::m_snTimeInMilliseconds;
uint32 CTimer::m_snTimeInMillisecondsPauseMode = 1;
uint32 CTimer::m_snTimeInMillisecondsNonClipped;
uint32 CTimer::m_snPreviousTimeInMilliseconds;
uint32 CTimer::m_FrameCounter;
float CTimer::ms_fTimeScale;
float CTimer::ms_fTimeStep;
float CTimer::ms_fTimeStepNonClipped;
bool  CTimer::m_UserPause;
bool  CTimer::m_CodePause;
#ifdef FIX_BUGS
uint32 CTimer::m_LogicalFrameCounter;
uint32 CTimer::m_LogicalFramesPassed;
#endif

uint32 _nCyclesPerMS = 1;

#ifdef _WIN32
LARGE_INTEGER _oldPerfCounter;
LARGE_INTEGER perfSuspendCounter;
#define RsTimerType uint32
#else
#define RsTimerType double
#endif

RsTimerType oldPcTimer;

RsTimerType suspendPcTimer;

uint32 suspendDepth;

void CTimer::Initialise(void)
{
	debug("Initialising CTimer...\n");
	
	ms_fTimeScale = 1.0f;
	ms_fTimeStep = 1.0f;
	suspendDepth = 0;
	m_UserPause = false;
	m_CodePause = false;
	m_snTimeInMillisecondsNonClipped = 0;
	m_snPreviousTimeInMilliseconds = 0;
	m_snTimeInMilliseconds = 1;
#ifdef FIX_BUGS
	m_LogicalFrameCounter = 0;
	m_LogicalFramesPassed = 0;
#endif
	
#ifdef _WIN32
	LARGE_INTEGER perfFreq;
	if ( QueryPerformanceFrequency(&perfFreq) )
	{
		OutputDebugString("Performance counter available\n");
		_nCyclesPerMS = uint32(perfFreq.QuadPart / 1000);
		QueryPerformanceCounter(&_oldPerfCounter);
	}
	else
#endif
	{
		OutputDebugString("Performance counter not available, using millesecond timer\n");
		_nCyclesPerMS = 0;
		oldPcTimer = RsTimer();
	}
	
	m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds;
	
	m_FrameCounter = 0;
	
	DMAudio.ResetTimers(m_snPreviousTimeInMilliseconds);
	
	debug("CTimer ready\n");
}

void CTimer::Shutdown(void)
{
	;
}
#ifdef FIX_BUGS
void CTimer::Update(void)
{
	static double frameTimeLogical = 0.0;
	static double frameTimeFraction = 0.0;
	static double frameTimeFractionScaled = 0.0;
	double frameTime;
	double dblUpdInMs;

	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
	
#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);
		
		int32 updInCycles = (pc.LowPart - _oldPerfCounter.LowPart); // & 0x7FFFFFFF; pointless
		
		_oldPerfCounter = pc;

		// bugfix from VC
		double updInCyclesScaled = GetIsPaused() ? updInCycles : updInCycles * ms_fTimeScale;

		frameTime = updInCyclesScaled / (double)_nCyclesPerMS;

		dblUpdInMs = (double)updInCycles / (double)_nCyclesPerMS;
	}
	else
#endif
	{
		RsTimerType timer = RsTimer();
		
		RsTimerType updInMs = timer - oldPcTimer;

		// bugfix from VC
		frameTime = GetIsPaused() ? (double)updInMs : (double)updInMs * ms_fTimeScale;

		oldPcTimer = timer;

		dblUpdInMs = (double)updInMs;
	}

	// count frames as if we're running at 30 fps
	m_LogicalFramesPassed = 0;
	frameTimeLogical += dblUpdInMs;
	while(frameTimeLogical >= 1000.0 / 30.0) {
		frameTimeLogical -= 1000.0 / 30.0;
		m_LogicalFramesPassed++;
	}
	m_LogicalFrameCounter += m_LogicalFramesPassed;

	frameTimeFraction += dblUpdInMs;
	frameTimeFractionScaled += frameTime;

	m_snTimeInMillisecondsPauseMode += uint32(frameTimeFraction);
															 
	if ( GetIsPaused() )
		ms_fTimeStep = 0.0f;
	else
	{
		m_snTimeInMilliseconds += uint32(frameTimeFractionScaled);
		m_snTimeInMillisecondsNonClipped += uint32(frameTimeFractionScaled);
		ms_fTimeStep = frameTime / 1000.0f * 50.0f;
	}
	frameTimeFraction -= uint32(frameTimeFraction);
	frameTimeFractionScaled -= uint32(frameTimeFractionScaled);
	
	if ( ms_fTimeStep < 0.01f && !GetIsPaused() )
		ms_fTimeStep = 0.01f;

	ms_fTimeStepNonClipped = ms_fTimeStep;
	
	if ( !CRecordDataForGame::IsPlayingBack() )
	{
		ms_fTimeStep = Min(3.0f, ms_fTimeStep);

		if ( (m_snTimeInMilliseconds - m_snPreviousTimeInMilliseconds) > 60 )
			m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 60;
	}
  
	if ( CRecordDataForChase::IsRecording() )
	{
		ms_fTimeStep = 1.0f;
		m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 16;
	}
  
	m_FrameCounter++;
}
#else
void CTimer::Update(void)
{
	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
	
#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);
		
		int32 updInCycles = (pc.LowPart - _oldPerfCounter.LowPart); // & 0x7FFFFFFF; pointless
		
		_oldPerfCounter = pc;

		float updInCyclesScaled = updInCycles * ms_fTimeScale;

		double frameTime = updInCyclesScaled / (double)_nCyclesPerMS;
		m_snTimeInMillisecondsPauseMode = m_snTimeInMillisecondsPauseMode + frameTime;
		
		if ( GetIsPaused() )
			ms_fTimeStep = 0.0f;
		else
		{
			m_snTimeInMilliseconds = m_snTimeInMilliseconds + frameTime;
			m_snTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped + frameTime;
			ms_fTimeStep = frameTime / 1000.0f * 50.0f;
		}
	}
	else
#endif
	{
		RsTimerType timer = RsTimer();
		
		RsTimerType updInMs = timer - oldPcTimer;
		
		double frameTime = (double)updInMs * ms_fTimeScale;

		oldPcTimer = timer;
		
		m_snTimeInMillisecondsPauseMode = m_snTimeInMillisecondsPauseMode + frameTime;
															 
		if ( GetIsPaused() )
			ms_fTimeStep = 0.0f;
		else
		{
			m_snTimeInMilliseconds = m_snTimeInMilliseconds + frameTime;
			m_snTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped + frameTime;
			ms_fTimeStep = frameTime / 1000.0f * 50.0f;
		}
	}
	
	if ( ms_fTimeStep < 0.01f && !GetIsPaused() )
		ms_fTimeStep = 0.01f;

	ms_fTimeStepNonClipped = ms_fTimeStep;
	
	if ( !CRecordDataForGame::IsPlayingBack() )
	{
		ms_fTimeStep = Min(3.0f, ms_fTimeStep);

		if ( (m_snTimeInMilliseconds - m_snPreviousTimeInMilliseconds) > 60 )
			m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 60;
	}
  
	if ( CRecordDataForChase::IsRecording() )
	{
		ms_fTimeStep = 1.0f;
		m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 16;
	}
  
	m_FrameCounter++;
}
#endif

void CTimer::Suspend(void)
{
	if ( ++suspendDepth > 1 )
		return;
	
#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
		QueryPerformanceCounter(&perfSuspendCounter);
	else
#endif
		suspendPcTimer = RsTimer();
}

void CTimer::Resume(void)
{
	if ( --suspendDepth != 0 )
		return;

#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);

		_oldPerfCounter.LowPart += pc.LowPart - perfSuspendCounter.LowPart;
	}
	else
#endif
		oldPcTimer += RsTimer() - suspendPcTimer;
}

uint32 CTimer::GetCyclesPerMillisecond(void)
{
#ifdef _WIN32
	if (_nCyclesPerMS != 0)
		return _nCyclesPerMS;
	else 
#endif
		return 1;
}

uint32 CTimer::GetCurrentTimeInCycles(void)
{
#ifdef _WIN32
	if ( _nCyclesPerMS != 0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);
		return (pc.LowPart - _oldPerfCounter.LowPart); // & 0x7FFFFFFF; pointless
	}
	else
#endif
		return RsTimer() - oldPcTimer;
}

bool CTimer::GetIsSlowMotionActive(void)
{
	return ms_fTimeScale < 1.0f;
}

void CTimer::Stop(void)
{
	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
}

void CTimer::StartUserPause(void)
{
	m_UserPause = true;
}

void CTimer::EndUserPause(void)
{
	m_UserPause = false;
}

uint32 CTimer::GetCyclesPerFrame()
{
	return 20;
}

#endif
