#pragma once

// C++ headers
#include <fstream>
#include <list>
#include <string>
#include <thread>
#include <math.h>
#include <assert.h>
#include <io.h>

// header for AVX and before
#include <immintrin.h>

// basic types
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

// windows.h: disable as much as possible to speed up compilation.
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#endif
#define NOGDICAPMASKS
// #define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOIME
#include "windows.h"

// include vulkan application
#include "vulkan.h"

// fatal error reporting (with a pretty window)
#define FATALERROR( fmt, ... ) FatalError( "Error on line %d of %s: " fmt "\n", __LINE__, __FILE__, ##__VA_ARGS__ )
#define FATALERROR_IF( condition, fmt, ... ) do { if ( ( condition ) ) FATALERROR( fmt, ##__VA_ARGS__ ); } while ( 0 )
#define FATALERROR_IN( prefix, errstr, fmt, ... ) FatalError( prefix " returned error '%s' at %s:%d" fmt "\n", errstr, __FILE__, __LINE__, ##__VA_ARGS__ );
#define FATALERROR_IN_CALL( stmt, error_parser, fmt, ... ) do { auto ret = ( stmt ); if ( ret ) FATALERROR_IN( #stmt, error_parser( ret ), fmt, ##__VA_ARGS__ ) } while ( 0 )

// timer
struct Timer
{
	Timer() { reset(); }
	float elapsed() const
	{
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - start);
		return (float)time_span.count();
	}
	void reset() { start = std::chrono::high_resolution_clock::now(); }
	std::chrono::high_resolution_clock::time_point start;
};

// Nils's jobmanager
class Job
{
public:
	virtual void Main() = 0;
protected:
	friend class JobThread;
	void RunCodeWrapper();
};
class JobThread
{
public:
	void CreateAndStartThread(unsigned int threadId);
	void Go();
	void BackgroundTask();
	HANDLE m_GoSignal, m_ThreadHandle;
	int m_ThreadID;
};
class JobManager	// singleton class!
{
protected:
	JobManager(unsigned int numThreads);
public:
	~JobManager();
	static void CreateJobManager(unsigned int numThreads);
	static JobManager* GetJobManager();
	static void GetProcessorCount(uint& cores, uint& logical);
	void AddJob2(Job* a_Job);
	unsigned int GetNumThreads() { return m_NumThreads; }
	void RunJobs();
	void ThreadDone(unsigned int n);
	int MaxConcurrent() { return m_NumThreads; }
protected:
	friend class JobThread;
	Job* GetNextJob();
	static JobManager* m_JobManager;
	Job* m_JobList[256];
	CRITICAL_SECTION m_CS;
	HANDLE m_ThreadDone[64];
	unsigned int m_NumThreads, m_JobCount;
	JobThread* m_JobThreadList;
};

// forward declaration of helper functions
void FatalError(const char* fmt, ...);			// seem to use in OpenCL which is not used in this project
bool FileIsNewer(const char* file1, const char* file2);
bool FileExists(const char* f);
bool RemoveFile(const char* f);
std::string TextFileRead(const char* _File);
int LineCount(const std::string s);
void TextFileWrite(const std::string& text, const char* _File);

// InstructionSet.cpp
// Compile by using: cl /EHsc /W4 InstructionSet.cpp
// processor: x86, x64
// Uses the __cpuid intrinsic to get information about
// CPU extended instruction set support.

// #include <iostream>
#include <bitset>
#include <intrin.h>

// instruction set detection
#ifdef _WIN32
#define cpuid(info, x) __cpuidex(info, x, 0)
#else
#include <cpuid.h>
void cpuid(int info[4], int InfoType) { __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]); }
#endif
class CPUCaps // from https://github.com/Mysticial/FeatureDetector
{
public:
	static inline bool HW_MMX = false;
	static inline bool HW_x64 = false;
	static inline bool HW_ABM = false;
	static inline bool HW_RDRAND = false;
	static inline bool HW_BMI1 = false;
	static inline bool HW_BMI2 = false;
	static inline bool HW_ADX = false;
	static inline bool HW_PREFETCHWT1 = false;
	// SIMD: 128-bit
	static inline bool HW_SSE = false;
	static inline bool HW_SSE2 = false;
	static inline bool HW_SSE3 = false;
	static inline bool HW_SSSE3 = false;
	static inline bool HW_SSE41 = false;
	static inline bool HW_SSE42 = false;
	static inline bool HW_SSE4a = false;
	static inline bool HW_AES = false;
	static inline bool HW_SHA = false;
	// SIMD: 256-bit
	static inline bool HW_AVX = false;
	static inline bool HW_XOP = false;
	static inline bool HW_FMA3 = false;
	static inline bool HW_FMA4 = false;
	static inline bool HW_AVX2 = false;
	// SIMD: 512-bit
	static inline bool HW_AVX512F = false;    //  AVX512 Foundation
	static inline bool HW_AVX512CD = false;   //  AVX512 Conflict Detection
	static inline bool HW_AVX512PF = false;   //  AVX512 Prefetch
	static inline bool HW_AVX512ER = false;   //  AVX512 Exponential + Reciprocal
	static inline bool HW_AVX512VL = false;   //  AVX512 Vector Length Extensions
	static inline bool HW_AVX512BW = false;   //  AVX512 Byte + Word
	static inline bool HW_AVX512DQ = false;   //  AVX512 Doubleword + Quadword
	static inline bool HW_AVX512IFMA = false; //  AVX512 Integer 52-bit Fused Multiply-Add
	static inline bool HW_AVX512VBMI = false; //  AVX512 Vector Byte Manipulation Instructions
	// constructor
	CPUCaps()
	{
		int info[4];
		cpuid(info, 0);
		int nIds = info[0];
		cpuid(info, 0x80000000);
		unsigned nExIds = info[0];
		// detect Features
		if (nIds >= 0x00000001)
		{
			cpuid(info, 0x00000001);
			HW_MMX = (info[3] & ((int)1 << 23)) != 0;
			HW_SSE = (info[3] & ((int)1 << 25)) != 0;
			HW_SSE2 = (info[3] & ((int)1 << 26)) != 0;
			HW_SSE3 = (info[2] & ((int)1 << 0)) != 0;
			HW_SSSE3 = (info[2] & ((int)1 << 9)) != 0;
			HW_SSE41 = (info[2] & ((int)1 << 19)) != 0;
			HW_SSE42 = (info[2] & ((int)1 << 20)) != 0;
			HW_AES = (info[2] & ((int)1 << 25)) != 0;
			HW_AVX = (info[2] & ((int)1 << 28)) != 0;
			HW_FMA3 = (info[2] & ((int)1 << 12)) != 0;
			HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;
		}
		if (nIds >= 0x00000007)
		{
			cpuid(info, 0x00000007);
			HW_AVX2 = (info[1] & ((int)1 << 5)) != 0;
			HW_BMI1 = (info[1] & ((int)1 << 3)) != 0;
			HW_BMI2 = (info[1] & ((int)1 << 8)) != 0;
			HW_ADX = (info[1] & ((int)1 << 19)) != 0;
			HW_SHA = (info[1] & ((int)1 << 29)) != 0;
			HW_PREFETCHWT1 = (info[2] & ((int)1 << 0)) != 0;
			HW_AVX512F = (info[1] & ((int)1 << 16)) != 0;
			HW_AVX512CD = (info[1] & ((int)1 << 28)) != 0;
			HW_AVX512PF = (info[1] & ((int)1 << 26)) != 0;
			HW_AVX512ER = (info[1] & ((int)1 << 27)) != 0;
			HW_AVX512VL = (info[1] & ((int)1 << 31)) != 0;
			HW_AVX512BW = (info[1] & ((int)1 << 30)) != 0;
			HW_AVX512DQ = (info[1] & ((int)1 << 17)) != 0;
			HW_AVX512IFMA = (info[1] & ((int)1 << 21)) != 0;
			HW_AVX512VBMI = (info[2] & ((int)1 << 1)) != 0;
		}
		if (nExIds >= 0x80000001)
		{
			cpuid(info, 0x80000001);
			HW_x64 = (info[3] & ((int)1 << 29)) != 0;
			HW_ABM = (info[2] & ((int)1 << 5)) != 0;
			HW_SSE4a = (info[2] & ((int)1 << 6)) != 0;
			HW_FMA4 = (info[2] & ((int)1 << 16)) != 0;
			HW_XOP = (info[2] & ((int)1 << 11)) != 0;
		}
	}
};

//EOF