#include "precomp.h"
#include "Vulkan_Experiment_01.h"

#pragma comment( linker, "/subsystem:windows /ENTRY:mainCRTStartup" )

#ifdef WIN32
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

void main()
{
	std::cout << "Hello CMake." << std::endl;
}

#pragma region Jobmanager
DWORD JobThreadProc(LPVOID lpParameter)
{
	JobThread* JobThreadInstance = (JobThread*)lpParameter;
	JobThreadInstance->BackgroundTask();
	return 0;
}

void JobThread::CreateAndStartThread(unsigned int threadId)
{
	m_GoSignal = CreateEvent(0, FALSE, FALSE, 0);
	m_ThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&JobThreadProc, (LPVOID)this, 0, 0);
	m_ThreadID = threadId;
}
void JobThread::BackgroundTask()
{
	while (1)
	{
		WaitForSingleObject(m_GoSignal, INFINITE);
		while (1)
		{
			Job* job = JobManager::GetJobManager()->GetNextJob();
			if (!job)
			{
				JobManager::GetJobManager()->ThreadDone(m_ThreadID);
				break;
			}
			job->RunCodeWrapper();
		}
	}
}

void JobThread::Go()
{
	SetEvent(m_GoSignal);
}

void Job::RunCodeWrapper()
{
	Main();
}

JobManager* JobManager::m_JobManager = 0;

JobManager::JobManager(unsigned int threads) : m_NumThreads(threads)
{
	InitializeCriticalSection(&m_CS);
}

JobManager::~JobManager()
{
	DeleteCriticalSection(&m_CS);
}

void JobManager::CreateJobManager(unsigned int numThreads)
{
	m_JobManager = new JobManager(numThreads);
	m_JobManager->m_JobThreadList = new JobThread[numThreads];
	for (unsigned int i = 0; i < numThreads; i++)
	{
		m_JobManager->m_JobThreadList[i].CreateAndStartThread(i);
		m_JobManager->m_ThreadDone[i] = CreateEvent(0, FALSE, FALSE, 0);
	}
	m_JobManager->m_JobCount = 0;
}

void JobManager::AddJob2(Job* a_Job)
{
	m_JobList[m_JobCount++] = a_Job;
}

Job* JobManager::GetNextJob()
{
	Job* job = 0;
	EnterCriticalSection(&m_CS);
	if (m_JobCount > 0) job = m_JobList[--m_JobCount];
	LeaveCriticalSection(&m_CS);
	return job;
}

void JobManager::RunJobs()
{
	if (m_JobCount == 0) return;
	for (unsigned int i = 0; i < m_NumThreads; i++) m_JobThreadList[i].Go();
	WaitForMultipleObjects(m_NumThreads, m_ThreadDone, TRUE, INFINITE);
}

void JobManager::ThreadDone(unsigned int n)
{
	SetEvent(m_ThreadDone[n]);
}

DWORD CountSetBits(ULONG_PTR bitMask)
{
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1, bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
	for (DWORD i = 0; i <= LSHIFT; ++i) bitSetCount += ((bitMask & bitTest) ? 1 : 0), bitTest /= 2;
	return bitSetCount;
}

void JobManager::GetProcessorCount(uint& cores, uint& logical)
{
	// https://github.com/GPUOpen-LibrariesAndSDKs/cpu-core-counts
	cores = logical = 0;
	char* buffer = NULL;
	DWORD len = 0;
	if (FALSE == GetLogicalProcessorInformationEx(RelationAll, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer, &len))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			buffer = (char*)malloc(len);
			if (GetLogicalProcessorInformationEx(RelationAll, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer, &len))
			{
				char* ptr = buffer;
				while (ptr < buffer + len)
				{
					PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pi = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)ptr;
					if (pi->Relationship == RelationProcessorCore)
					{
						cores++;
						for (size_t g = 0; g < pi->Processor.GroupCount; ++g)
							logical += CountSetBits(pi->Processor.GroupMask[g].Mask);
					}
					ptr += pi->Size;
				}
			}
			free(buffer);
		}
	}
}

JobManager* JobManager::GetJobManager()
{
	if (!m_JobManager)
	{
		uint c, l;
		GetProcessorCount(c, l);
		CreateJobManager(l);
	}
	return m_JobManager;
}
#pragma endregion

#pragma region help function
bool FileIsNewer(const char* file1, const char* file2)
{
	struct stat f1;
	struct stat f2;

	auto ret = stat(file1, &f1);
	FATALERROR_IF(ret, "File %s not found!", file1);

	if (stat(file2, &f2)) return true; // second file does not exist

#ifdef _MSC_VER
	return f1.st_mtime >= f2.st_mtime;
#else
	if (f1.st_mtim.tv_sec >= f2.st_mtim.tv_sec)
		return true;
	return f1.st_mtim.tv_nsec >= f2.st_mtim.tv_nsec;
#endif
}

bool FileExists(const char* f)
{
	ifstream s(f);
	return s.good();
}

bool RemoveFile(const char* f)
{
	if (!FileExists(f)) return false;
	return !remove(f);
}

uint FileSize(string filename)
{
	ifstream s(filename);
	return s.good();
}

string TextFileRead(const char* _File)
{
	ifstream s(_File);
	string str((istreambuf_iterator<char>(s)), istreambuf_iterator<char>());
	s.close();
	return str;
}

int LineCount(const string s)
{
	const char* p = s.c_str();
	int lines = 0;
	while (*p) if (*p++ == '\n') lines++;
	return lines;
}

void TextFileWrite(const string& text, const char* _File)
{
	ofstream s(_File, ios::binary);
	int len = (int)text.size();
	s.write((const char*)&len, sizeof(len));
	s.write(text.c_str(), len);
}

#pragma endregion
