#define FPL_IMPLEMENTATION
#define FPL_NO_AUDIO
#define FPL_NO_VIDEO
#define FPL_NO_WINDOW
#include <final_platform_layer.hpp>

#define FT_IMPLEMENTATION
#include "final_test.h"

static void TestInit() {
	ft::Msg("Test InitPlatform with All init flags\n");
	{
		fpl::ClearPlatformErrors();
		bool result = fpl::InitPlatform(fpl::InitFlags::All);
		FT_IS_TRUE(result);
		const char *errorStr = fpl::GetPlatformError();
		ft::AssertStringEquals("", errorStr);
		fpl::ReleasePlatform();
	}
	ft::Msg("Test InitPlatform with None init flags\n");
	{
		fpl::ClearPlatformErrors();
		bool result = fpl::InitPlatform(fpl::InitFlags::None);
		FT_IS_TRUE(result);
		const char *errorStr = fpl::GetPlatformError();
		ft::AssertStringEquals("", errorStr);
		fpl::ReleasePlatform();
	}
}

static void TestSizes() {
	// @NOTE(final): This may be pretty useless, because stdint.h guarantees the size
	FT_EXPECTS(1, sizeof(uint8_t));
	FT_EXPECTS(1, sizeof(int8_t));
	FT_EXPECTS(2, sizeof(uint16_t));
	FT_EXPECTS(2, sizeof(int16_t));
	FT_EXPECTS(4, sizeof(uint32_t));
	FT_EXPECTS(4, sizeof(int32_t));
	FT_EXPECTS(8, sizeof(uint64_t));
	FT_EXPECTS(8, sizeof(int64_t));
#if defined(FT_ARCH_X64)
	FT_EXPECTS(8, sizeof(intptr_t));
	FT_EXPECTS(8, sizeof(uintptr_t));
	FT_EXPECTS(8, sizeof(size_t));
#else
	FT_EXPECTS(4, sizeof(intptr_t));
	FT_EXPECTS(4, sizeof(uintptr_t));
	FT_EXPECTS(4, sizeof(size_t));
#endif
}

static void TestMacros() {
	//
	// FPL_ARRAYCOUNT
	//
	ft::Msg("[FPL_ARRAYCOUNT] Test static char array\n");
	{
		char staticArray[137] = {};
		uint32_t actual = FPL_ARRAYCOUNT(staticArray);
		FT_EXPECTS(137, actual);
	}
	ft::Msg("[FPL_ARRAYCOUNT] Test static int array\n");
	{
		int staticArray[349] = {};
		uint32_t actual = FPL_ARRAYCOUNT(staticArray);
		FT_EXPECTS(349, actual);
	}
	ft::Msg("[FPL_ARRAYCOUNT] Test static bool array\n");
	{
		bool staticArray[961] = {};
		uint32_t actual = FPL_ARRAYCOUNT(staticArray);
		FT_EXPECTS(961, actual);
	}
	ft::Msg("[FPL_ARRAYCOUNT] Test static void pointer array\n");
	{
		void *staticArray[35] = {};
		uint32_t actual = FPL_ARRAYCOUNT(staticArray);
		FT_EXPECTS(35, actual);
	}

	// @NOTE(final): This is a simple/stupid macro, so when you pass a pointer, you basically get 2 always
	ft::Msg("[FPL_ARRAYCOUNT] Test nullptr\n");
	{
		int *emptyArray = nullptr;
		uint32_t actual = FPL_ARRAYCOUNT(emptyArray);
		uint32_t expected = sizeof(int *) / sizeof(int);
		FT_EXPECTS(expected, actual);
	}
	ft::Msg("[FPL_ARRAYCOUNT] Test pointer from references static array\n");
	{
		int staticArray[3] = {};
		int *refArray = &staticArray[0];
		uint32_t actual = FPL_ARRAYCOUNT(refArray);
		uint32_t expected = sizeof(int *) / sizeof(int);
		FT_EXPECTS(expected, actual);
	}

	//
	// FPL_OFFSETOF
	//
	ft::Msg("[FPL_OFFSETOF] Test alignment of 4 (High to low)\n");
	{
#	pragma pack(push, 4)
		struct TestStruct {
			uint64_t a;
			uint32_t b;
			uint16_t c;
			uint8_t d;
		};
#	pragma pack(pop)
		FT_EXPECTS(0, FPL_OFFSETOF(TestStruct, a));
		FT_EXPECTS(8, FPL_OFFSETOF(TestStruct, b));
		FT_EXPECTS(12, FPL_OFFSETOF(TestStruct, c));
		FT_EXPECTS(14, FPL_OFFSETOF(TestStruct, d));
	}

	ft::Msg("[FPL_OFFSETOF] Test alignment of 4 (Low to High)\n");
	{
#	pragma pack(push, 4)
		struct TestStruct {
			uint8_t a;
			uint16_t b;
			uint32_t c;
			uint64_t d;
		};
#	pragma pack(pop)
		FT_EXPECTS(0, FPL_OFFSETOF(TestStruct, a));
		FT_EXPECTS(2, FPL_OFFSETOF(TestStruct, b));
		FT_EXPECTS(4, FPL_OFFSETOF(TestStruct, c));
		FT_EXPECTS(8, FPL_OFFSETOF(TestStruct, d));
	}

	ft::Msg("[FPL_OFFSETOF] Test alignment of 8 (Low to High)\n");
	{
#	pragma pack(push, 8)
		struct TestStruct {
			uint8_t a;
			uint16_t b;
			uint8_t c[3];
			uint64_t d;
		};
#	pragma pack(pop)
		FT_EXPECTS(0, FPL_OFFSETOF(TestStruct, a));
		FT_EXPECTS(2, FPL_OFFSETOF(TestStruct, b));
		FT_EXPECTS(4, FPL_OFFSETOF(TestStruct, c));
		FT_EXPECTS(8, FPL_OFFSETOF(TestStruct, d));
	}

	//
	// FPL_MIN/FPL_MAX
	//
	ft::Msg("[FPL_MIN] Test integers\n");
	{
		ft::AssertS32Equals(3, FPL_MIN(3, 7));
		ft::AssertS32Equals(3, FPL_MIN(7, 3));
		ft::AssertS32Equals(-7, FPL_MIN(-7, -3));
		ft::AssertS32Equals(-7, FPL_MIN(-3, -7));
		struct TestStruct {
			int a;
			int b;
		};
		TestStruct instance = { 3, 7 };
		TestStruct *instancePtr = &instance;
		ft::AssertS32Equals(3, FPL_MIN(instancePtr->a, instancePtr->b));
	}
	ft::Msg("[FPL_MIN] Test floats\n");
	{
		ft::AssertFloatEquals(3.0f, FPL_MIN(3.0f, 7.0f));
		ft::AssertFloatEquals(3.0f, FPL_MIN(7.0f, 3.0f));
		ft::AssertFloatEquals(-7.0f, FPL_MIN(-7.0f, -3.0f));
		ft::AssertFloatEquals(-7.0f, FPL_MIN(-3.0f, -7.0f));
		struct TestStruct {
			float a;
			float b;
		};
		TestStruct instance = { 3.0f, 7.0f };
		TestStruct *instancePtr = &instance;
		ft::AssertFloatEquals(3.0f, FPL_MIN(instancePtr->a, instancePtr->b));
	}
	ft::Msg("[FPL_MAX] Test integers\n");
	{
		ft::AssertS32Equals(7, FPL_MAX(3, 7));
		ft::AssertS32Equals(7, FPL_MAX(7, 3));
		ft::AssertS32Equals(-3, FPL_MAX(-3, -7));
		ft::AssertS32Equals(-3, FPL_MAX(-7, -3));
		struct TestStruct {
			int a;
			int b;
		};
		TestStruct instance = { 3, 7 };
		TestStruct *instancePtr = &instance;
		ft::AssertS32Equals(7, FPL_MAX(instancePtr->a, instancePtr->b));
	}
	ft::Msg("[FPL_MAX] Test floats\n");
	{
		ft::AssertFloatEquals(7.0f, FPL_MAX(3.0f, 7.0f));
		ft::AssertFloatEquals(7.0f, FPL_MAX(7.0f, 3.0f));
		ft::AssertFloatEquals(-3.0f, FPL_MAX(-3.0f, -7.0f));
		ft::AssertFloatEquals(-3.0f, FPL_MAX(-7.0f, -3.0f));
		struct TestStruct {
			float a;
			float b;
		};
		TestStruct instance = { 3.0f, 7.0f };
		TestStruct *instancePtr = &instance;
		ft::AssertFloatEquals(7.0f, FPL_MAX(instancePtr->a, instancePtr->b));
	}

	//
	// FPL_KILOBYTES, FPL_MEGABYTES, ...
	//
	{
		ft::Msg("[FPL_KILOBYTES] Test 0 KB \n");
		ft::AssertSizeEquals(0, FPL_KILOBYTES(0));
		ft::Msg("[FPL_KILOBYTES] Test 8 KB \n");
		ft::AssertSizeEquals(8192, FPL_KILOBYTES(8));
		ft::Msg("[FPL_MEGABYTES] Test 0 MB \n");
		ft::AssertSizeEquals(0, FPL_MEGABYTES(0));
		ft::Msg("[FPL_MEGABYTES] Test 8 MB \n");
		ft::AssertSizeEquals(8388608, FPL_MEGABYTES(8));
		ft::Msg("[FPL_GIGABYTES] Test 0 GB \n");
		ft::AssertSizeEquals(0, FPL_GIGABYTES(0));
		ft::Msg("[FPL_GIGABYTES] Test 1 GB \n");
		ft::AssertSizeEquals(1073741824, FPL_GIGABYTES(1));
#if defined(FT_ARCH_X64)
		ft::Msg("[FPL_GIGABYTES] Test 4 GB \n");
		ft::AssertSizeEquals(4294967296, FPL_GIGABYTES(4));
		ft::Msg("[FPL_TERABYTES] Test 0 TB \n");
		ft::AssertSizeEquals(0, FPL_TERABYTES(0));
		ft::Msg("[FPL_TERABYTES] Test 2 TB \n");
		ft::AssertSizeEquals(2199023255552, FPL_TERABYTES(2));
#endif
	}
}

static void TestMemory() {
	ft::Msg("Test normal allocation and deallocation\n");
	{
		size_t memSize = FPL_KILOBYTES(42);
		uint8_t *mem = (uint8_t *)fpl::memory::MemoryAllocate(memSize);
		for(size_t i = 0; i < memSize; ++i) {
			uint8_t value = *mem++;
			ft::AssertU8Equals(0, value);
		}
		fpl::memory::MemoryFree(mem);
	}
	{
		size_t memSize = FPL_MEGABYTES(512);
		void *mem = fpl::memory::MemoryAllocate(memSize);
		FT_IS_NOT_NULL(mem);
		fpl::memory::MemoryFree(mem);
	}

	ft::Msg("Test aligned allocation and deallocation\n");
	{
		size_t memSize = FPL_KILOBYTES(42);
		uint8_t *mem = (uint8_t *)fpl::memory::MemoryAlignedAllocate(memSize, 16);
		for(size_t i = 0; i < memSize; ++i) {
			uint8_t value = *(mem + i);
			ft::AssertU8Equals(0, value);
		}
		fpl::memory::MemoryAlignedFree(mem);
	}
	{
		size_t memSize = FPL_MEGABYTES(512);
		void *mem = fpl::memory::MemoryAlignedAllocate(memSize, 16);
		FT_IS_NOT_NULL(mem);
		fpl::memory::MemoryAlignedFree(mem);
	}
}

static void TestPaths() {
	if(fpl::InitPlatform(fpl::InitFlags::None)) {

		char homePathBuffer[1024] = {};
		fpl::paths::GetHomePath(homePathBuffer, FPL_ARRAYCOUNT(homePathBuffer));
		ft::Msg("Home Path:\n%s\n", homePathBuffer);

		char exeFilePathBuffer[1024] = {};
		fpl::paths::GetExecutableFilePath(exeFilePathBuffer, FPL_ARRAYCOUNT(exeFilePathBuffer));
		ft::Msg("Executable file Path:\n%s\n", exeFilePathBuffer);

		char extractedPathBuffer[1024] = {};
		fpl::paths::ExtractFilePath(exeFilePathBuffer, extractedPathBuffer, FPL_ARRAYCOUNT(extractedPathBuffer));
		ft::Msg("Extracted path:\n%s\n", extractedPathBuffer);

		char *exeFileName = fpl::paths::ExtractFileName(exeFilePathBuffer);
		ft::Msg("Extracted filename:\n%s\n", exeFileName);

		char *exeFileExt = fpl::paths::ExtractFileExtension(exeFilePathBuffer);
		ft::Msg("Extracted extension:\n%s\n", exeFileExt);

		char combinedPathBuffer[1024 * 10] = {};
		fpl::paths::CombinePath(combinedPathBuffer, FPL_ARRAYCOUNT(combinedPathBuffer), 4, "Hallo", "Welt", "der", "Programmierer");
		ft::Msg("Combined path:\n%s\n", combinedPathBuffer);

		char changedFileExtBuffer[1024] = {};
		fpl::paths::ChangeFileExtension(exeFilePathBuffer, ".obj", changedFileExtBuffer, FPL_ARRAYCOUNT(changedFileExtBuffer));
		ft::Msg("Changed file ext 1:\n%s\n", changedFileExtBuffer);
		fpl::paths::ChangeFileExtension(exeFileName, ".obj", changedFileExtBuffer, FPL_ARRAYCOUNT(changedFileExtBuffer));
		ft::Msg("Changed file ext 2:\n%s\n", changedFileExtBuffer);
		fpl::paths::ChangeFileExtension(".dll", ".obj", changedFileExtBuffer, FPL_ARRAYCOUNT(changedFileExtBuffer));
		ft::Msg("Changed file ext 3:\n%s\n", changedFileExtBuffer);
		fpl::paths::ChangeFileExtension("", ".obj", changedFileExtBuffer, FPL_ARRAYCOUNT(changedFileExtBuffer));
		ft::Msg("Changed file ext 4:\n%s\n", changedFileExtBuffer);
		fpl::paths::ChangeFileExtension(".dll", "", changedFileExtBuffer, FPL_ARRAYCOUNT(changedFileExtBuffer));
		ft::Msg("Changed file ext 5:\n%s\n", changedFileExtBuffer);
		fpl::paths::ChangeFileExtension("", "", changedFileExtBuffer, FPL_ARRAYCOUNT(changedFileExtBuffer));
		ft::Msg("Changed file ext 5:\n%s\n", changedFileExtBuffer);

		fpl::ReleasePlatform();
	}
}

static void TestHardware() {
	char cpuNameBuffer[1024] = {};
	fpl::hardware::GetProcessorName(cpuNameBuffer, FPL_ARRAYCOUNT(cpuNameBuffer));
	ft::Msg("Processor name:\n%s\n", cpuNameBuffer);

	uint32_t coreCount = fpl::hardware::GetProcessorCoreCount();
	ft::Msg("Processor cores:%d\n", coreCount);

	fpl::hardware::MemoryInfos memInfos = fpl::hardware::GetSystemMemoryInfos();
	ft::Msg("Physical total memory (bytes):%zu\n", memInfos.totalPhysicalSize);
	ft::Msg("Physical available memory (bytes):%zu\n", memInfos.availablePhysicalSize);
	ft::Msg("Physical used memory (bytes):%zu\n", memInfos.usedPhysicalSize);
	ft::Msg("Virtual total memory (bytes):%zu\n", memInfos.totalVirtualSize);
	ft::Msg("Virtual used memory (bytes):%zu\n", memInfos.usedVirtualSize);
	ft::Msg("Page total memory (bytes):%zu\n", memInfos.totalPageSize);
	ft::Msg("Page used memory (bytes):%zu\n", memInfos.usedPageSize);
}



static void EmptyThreadproc(const fpl::threading::ThreadContext &context, void *data) {
}

struct ThreadData {
	int num;
	int sleepFor;
};

static void SingleThreadProc(const fpl::threading::ThreadContext &context, void *data) {
	ThreadData *d = (ThreadData *)data;
	ft::Msg("Sleep in thread %d for %d ms\n", d->num, d->sleepFor);
	fpl::threading::ThreadSleep(d->sleepFor);
}

static void SimpleMultiThreadTest(const uint32_t threadCount) {
	ft::Line();
	ThreadData threadData[fpl::common::MAX_THREAD_COUNT] = {};
	for(uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		threadData[threadIndex].num = threadIndex + 1;
		threadData[threadIndex].sleepFor = (1 + threadIndex) * 500;
	}
	fpl::threading::ThreadContext *threads[fpl::common::MAX_THREAD_COUNT];
	ft::Msg("Start %d threads\n", threadCount);
	for(uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		threads[threadIndex] = fpl::threading::ThreadCreate(SingleThreadProc, &threadData[threadIndex]);
	}
	ft::Msg("Wait all %d threads for exit\n", threadCount);
	fpl::threading::ThreadWaitForAll(threads, threadCount);
	ft::Msg("All %d threads are done\n", threadCount);
	for(uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		assert(threads[threadIndex]->currentState == fpl::threading::ThreadState::Stopped);
	}
	ft::Msg("Destroy %d threads\n", threadCount);
	for(uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		fpl::threading::ThreadDestroy(threads[threadIndex]);
	}
}

struct SlaveThreadData {
	ThreadData base;
	fpl::threading::ThreadMutex *mutex;
	fpl::threading::ThreadSignal signal;
};

struct MasterThreadData {
	ThreadData base;
	fpl::threading::ThreadMutex mutex;
	fpl::threading::ThreadSignal *signals[fpl::common::MAX_SIGNAL_COUNT];
	uint32_t signalCount;
};

static void ThreadSlaveProc(const fpl::threading::ThreadContext &context, void *data) {
	SlaveThreadData *d = (SlaveThreadData *)data;

	ft::Msg("Slave-Thread %d waits for signal\n", d->base.num);
	fpl::threading::SignalWaitForOne(*d->mutex, d->signal);
	ft::Msg("Got signal on Slave-Thread %d\n", d->base.num);

	ft::Msg("Slave-Thread %d is done\n", d->base.num);
}

static void ThreadMasterProc(const fpl::threading::ThreadContext &context, void *data) {
	MasterThreadData *d = (MasterThreadData *)data;
	ft::Msg("Master-Thread %d waits for 5 seconds\n", d->base.num);
	fpl::threading::ThreadSleep(5000);

	ft::Msg("Master-Thread %d sets %d signals\n", d->base.num, d->signalCount);
	fpl::threading::MutexLock(d->mutex);
	for(uint32_t signalIndex = 0; signalIndex < d->signalCount; ++signalIndex) {
		fpl::threading::SignalSet(*d->signals[signalIndex]);
	}
	fpl::threading::MutexUnlock(d->mutex);

	ft::Msg("Master-Thread %d is done\n", d->base.num);
}

struct MutableThreadData {
	fpl::threading::ThreadMutex lock;
	volatile int32_t value;
	bool useLock;
};

struct WriteThreadData {
	ThreadData base;
	MutableThreadData *data;
	int32_t valueToWrite;
};

struct ReadThreadData {
	ThreadData base;
	MutableThreadData *data;
	int32_t expectedValue;
};

static void WriteDataThreadProc(const fpl::threading::ThreadContext &context, void *data) {
	WriteThreadData *d = (WriteThreadData *)data;
	fpl::threading::ThreadSleep(d->base.sleepFor);
	fpl::atomics::AtomicStoreS32(&d->data->value, d->valueToWrite);
}

static void ReadDataThreadProc(const fpl::threading::ThreadContext &context, void *data) {
	ReadThreadData *d = (ReadThreadData *)data;
	fpl::threading::ThreadSleep(d->base.sleepFor);
	int32_t actualValue = fpl::atomics::AtomicLoadS32(&d->data->value);
	FT_EXPECTS(d->expectedValue, actualValue);
}

static void SyncThreadsTest() {
	ft::Line();
	ft::Msg("Sync test for 1 reader and 1 writer\n");
	{
		MutableThreadData mutableData = {};
		mutableData.lock = fpl::threading::MutexCreate();
		mutableData.useLock = false;
		mutableData.value = 0;

		ReadThreadData readData = {};
		readData.base.num = 2;
		readData.base.sleepFor = 5000;
		readData.data = &mutableData;
		readData.expectedValue = 42;

		WriteThreadData writeData = {};
		writeData.base.num = 1;
		writeData.base.sleepFor = 3000;
		writeData.data = &mutableData;
		writeData.valueToWrite = 42;

		fpl::threading::ThreadContext *threads[2];
		uint32_t threadCount = FPL_ARRAYCOUNT(threads);

		ft::Msg("Start %d threads\n", threadCount);
		threads[0] = fpl::threading::ThreadCreate(ReadDataThreadProc, &readData);
		threads[1] = fpl::threading::ThreadCreate(WriteDataThreadProc, &writeData);

		ft::Msg("Wait for %d threads to exit\n", threadCount);
		fpl::threading::ThreadWaitForAll(threads, threadCount);

		ft::Msg("Release resources for %d threads\n", threadCount);
		for(uint32_t index = 0; index < threadCount; ++index) {
			fpl::threading::ThreadDestroy(threads[index]);
		}
		fpl::threading::MutexDestroy(mutableData.lock);
	}
}

static void ConditionThreadsTest(const uint32_t threadCount) {
	FT_ASSERT(threadCount > 1);

	ft::Line();
	ft::Msg("Condition test for %d threads\n", threadCount);

	MasterThreadData masterData = {};
	masterData.base.num = 1;
	masterData.mutex = fpl::threading::MutexCreate();

	SlaveThreadData slaveDatas[fpl::common::MAX_THREAD_COUNT] = {};
	uint32_t slaveThreadCount = threadCount - 1;
	for(uint32_t threadIndex = 0; threadIndex < slaveThreadCount; ++threadIndex) {
		slaveDatas[threadIndex].base.num = 2 + threadIndex;
		slaveDatas[threadIndex].mutex = &masterData.mutex;
		slaveDatas[threadIndex].signal = fpl::threading::SignalCreate();
		masterData.signals[masterData.signalCount++] = &slaveDatas[threadIndex].signal;
	}

	ft::Msg("Start %d slave threads, 1 master thread\n", slaveThreadCount);
	fpl::threading::ThreadContext *threads[fpl::common::MAX_THREAD_COUNT];
	for(uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		if(threadIndex == slaveThreadCount) {
			threads[threadIndex] = fpl::threading::ThreadCreate(ThreadMasterProc, &masterData);
		} else {
			threads[threadIndex] = fpl::threading::ThreadCreate(ThreadSlaveProc, &slaveDatas[threadIndex]);
		}
	}

	ft::Msg("Wait for %d threads to exit\n", threadCount);
	fpl::threading::ThreadWaitForAll(threads, threadCount);

	ft::Msg("Release resources for %d threads\n", threadCount);
	for(uint32_t threadIndex = 0; threadIndex < slaveThreadCount; ++threadIndex) {
		fpl::threading::SignalDestroy(slaveDatas[threadIndex].signal);
	}
	fpl::threading::MutexDestroy(masterData.mutex);
	for(uint32_t threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
		fpl::threading::ThreadContext *thread = threads[threadIndex];
		FT_EXPECTS(fpl::threading::ThreadState::Stopped, thread->currentState);
		fpl::threading::ThreadDestroy(thread);
	}
}

static void TestThreading() {
	if(fpl::InitPlatform(fpl::InitFlags::None)) {
		//
		// Single threading test
		//
		ft::Line();
		ft::Msg("Test 1 empty thread\n");
		{
			fpl::threading::ThreadContext *thread;
			ft::Msg("Start thread\n");
			thread = fpl::threading::ThreadCreate(EmptyThreadproc, nullptr);
			ft::Msg("Wait thread for exit\n");
			fpl::threading::ThreadWaitForOne(thread);
			ft::Msg("Thread is done\n");
			FT_EXPECTS(fpl::threading::ThreadState::Stopped, thread->currentState);
			fpl::threading::ThreadDestroy(thread);
		}

		ft::Line();
		ft::Msg("Test 1 sleeping-thread\n");
		{
			ThreadData threadData = {};
			threadData.num = 1;
			threadData.sleepFor = 3000;
			ft::Msg("Start thread %d\n", threadData.num);
			fpl::threading::ThreadContext *thread = fpl::threading::ThreadCreate(SingleThreadProc, &threadData);
			ft::Msg("Wait thread %d for exit\n", threadData.num);
			fpl::threading::ThreadWaitForOne(thread);
			ft::Msg("Thread %d is done\n", threadData.num);
			FT_EXPECTS(fpl::threading::ThreadState::Stopped, thread->currentState);
			fpl::threading::ThreadDestroy(thread);
		}

		//
        // Multi threads test
		//
        uint32_t coreCount = fpl::hardware::GetProcessorCoreCount();
        int32_t threadCountForCores = coreCount > 2 ? coreCount - 1 : 1;
        {
            SimpleMultiThreadTest(2);
            SimpleMultiThreadTest(3);
            SimpleMultiThreadTest(4);
            SimpleMultiThreadTest(threadCountForCores);
        }

		//
		// Sync tests
		//
		{
			SyncThreadsTest();
		}

		//
		// Condition tests
		//
        {
            ConditionThreadsTest(2);
			ConditionThreadsTest(3);
            ConditionThreadsTest(4);
			ConditionThreadsTest(threadCountForCores);
        }

		fpl::ReleasePlatform();
	}
}

static void TestFiles() {
	ft::Msg("Test File Exists\n");
	{
		bool nonExisting = fpl::files::FileExists("C:\\Windows\\i_am_not_existing.lib");
		FPL_ASSERT(!nonExisting);
		bool notepadExists = fpl::files::FileExists("C:\\Windows\\notepad.exe");
		FPL_ASSERT(notepadExists);
	}
	ft::Msg("Test File Size\n");
	{
		uint32_t emptySize = fpl::files::GetFileSize32("C:\\Windows\\i_am_not_existing.lib");
		FPL_ASSERT(emptySize == 0);
		uint32_t notepadSize = fpl::files::GetFileSize32("C:\\Windows\\notepad.exe");
		FPL_ASSERT(notepadSize > 0);
	}
	ft::Msg("Test Directory Iterations\n");
	{
		fpl::files::FileEntry fileEntry;
		if(ListFilesBegin("C:\\*", fileEntry)) {
			ft::Msg("%s\n", fileEntry.path);
			while(ListFilesNext(fileEntry)) {
				ft::Msg("%s\n", fileEntry.path);
			}
			ListFilesEnd(fileEntry);
		}
	}
}

static void TestAtomics() {
	ft::Msg("Test AtomicExchangeU32 with different values\n");
	{
		const uint32_t expectedBefore = 42;
		const uint32_t expectedAfter = 1337;
		volatile uint32_t t = expectedBefore;
		uint32_t r = fpl::atomics::AtomicExchangeU32(&t, expectedAfter);
		ft::AssertU32Equals(expectedBefore, r);
		ft::AssertU32Equals(expectedAfter, (uint32_t)t);
	}
	ft::Msg("Test AtomicExchangeU32 with negative value\n");
	{
		const uint32_t expectedBefore = 42;
		const uint32_t exchangeValue = -1;
		const uint32_t expectedAfter = (uint32_t)UINT32_MAX;
		volatile uint32_t t = expectedBefore;
		uint32_t r = fpl::atomics::AtomicExchangeU32(&t, exchangeValue);
		ft::AssertU32Equals(expectedBefore, r);
		ft::AssertU32Equals(expectedAfter, (uint32_t)t);
	}
	ft::Msg("Test AtomicExchangeU32 with same value\n");
	{
		const uint32_t expectedBefore = 1;
		const uint32_t exchangeValue = expectedBefore;
		const uint32_t expectedAfter = exchangeValue;
		volatile uint32_t t = expectedBefore;
		uint32_t r = fpl::atomics::AtomicExchangeU32(&t, exchangeValue);
		ft::AssertU32Equals(expectedBefore, r);
		ft::AssertU32Equals(expectedAfter, (uint32_t)t);
	}
	ft::Msg("Test AtomicExchangeS32 with different values\n");
	{
		const int32_t expectedBefore = 42;
		const int32_t exchangeValue = 1337;
		const int32_t expectedAfter = exchangeValue;
		volatile int32_t t = expectedBefore;
		int32_t r = fpl::atomics::AtomicExchangeS32(&t, exchangeValue);
		ft::AssertS32Equals(expectedBefore, r);
		ft::AssertS32Equals(expectedAfter, (int32_t)t);
	}
	ft::Msg("Test AtomicExchangeS32 with negative value\n");
	{
		const int32_t expectedBefore = 42;
		const int32_t exchangeValue = -1;
		const int32_t expectedAfter = exchangeValue;
		volatile int32_t t = expectedBefore;
		int32_t r = fpl::atomics::AtomicExchangeS32(&t, exchangeValue);
		ft::AssertS32Equals(expectedBefore, r);
		ft::AssertS32Equals(expectedAfter, (int32_t)t);
	}
	ft::Msg("Test AtomicExchangeS32 with same value\n");
	{
		const int32_t expectedBefore = 1;
		const int32_t exchangeValue = expectedBefore;
		const int32_t expectedAfter = exchangeValue;
		volatile int32_t t = expectedBefore;
		int32_t r = fpl::atomics::AtomicExchangeS32(&t, exchangeValue);
		ft::AssertS32Equals(expectedBefore, r);
		ft::AssertS32Equals(expectedAfter, (int32_t)t);
	}

	ft::Msg("Test AtomicExchangeU64 with different values\n");
	{
		const uint64_t expectedBefore = 42;
		const uint64_t expectedAfter = 1337;
		volatile uint64_t t = expectedBefore;
		uint64_t r = fpl::atomics::AtomicExchangeU64(&t, expectedAfter);
		ft::AssertU64Equals(expectedBefore, r);
		ft::AssertU64Equals(expectedAfter, (uint64_t)t);
	}
	ft::Msg("Test AtomicExchangeU64 with negative value\n");
	{
		const uint64_t expectedBefore = 42;
		const uint64_t exchangeValue = -1;
		const uint64_t expectedAfter = (uint64_t)UINT64_MAX;
		volatile uint64_t t = expectedBefore;
		uint64_t r = fpl::atomics::AtomicExchangeU64(&t, exchangeValue);
		ft::AssertU64Equals(expectedBefore, r);
		ft::AssertU64Equals(expectedAfter, (uint64_t)t);
	}
	ft::Msg("Test AtomicExchangeU64 with same value\n");
	{
		const uint64_t expectedBefore = 1;
		const uint64_t exchangeValue = expectedBefore;
		const uint64_t expectedAfter = exchangeValue;
		volatile uint64_t t = expectedBefore;
		uint64_t r = fpl::atomics::AtomicExchangeU64(&t, exchangeValue);
		ft::AssertU64Equals(expectedBefore, r);
		ft::AssertU64Equals(expectedAfter, (uint64_t)t);
	}
	ft::Msg("Test AtomicExchangeS64 with different values\n");
	{
		const int64_t expectedBefore = 42;
		const int64_t exchangeValue = 1337;
		const int64_t expectedAfter = exchangeValue;
		volatile int64_t t = expectedBefore;
		int64_t r = fpl::atomics::AtomicExchangeS64(&t, exchangeValue);
		ft::AssertS64Equals(expectedBefore, r);
		ft::AssertS64Equals(expectedAfter, (int64_t)t);
	}
	ft::Msg("Test AtomicExchangeS64 with negative value\n");
	{
		const int64_t expectedBefore = 42;
		const int64_t exchangeValue = -1;
		const int64_t expectedAfter = exchangeValue;
		volatile int64_t t = expectedBefore;
		int64_t r = fpl::atomics::AtomicExchangeS64(&t, exchangeValue);
		ft::AssertS64Equals(expectedBefore, r);
		ft::AssertS64Equals(expectedAfter, (int64_t)t);
	}
	ft::Msg("Test AtomicExchangeS64 with same value\n");
	{
		const int64_t expectedBefore = 1;
		const int64_t exchangeValue = expectedBefore;
		const int64_t expectedAfter = exchangeValue;
		volatile int64_t t = expectedBefore;
		int64_t r = fpl::atomics::AtomicExchangeS64(&t, exchangeValue);
		ft::AssertS64Equals(expectedBefore, r);
		ft::AssertS64Equals(expectedAfter, (int64_t)t);
	}

	ft::Msg("Test AtomicCompareAndExchangeU32 with exchange\n");
	{
		volatile uint32_t value = 3;
		uint32_t comparand = 3;
		uint32_t exchange = 11;
		uint32_t after = fpl::atomics::AtomicCompareAndExchangeU32(&value, comparand, exchange);
		ft::AssertU32Equals((uint32_t)11, (uint32_t)value);
		ft::AssertU32Equals((uint32_t)3, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeU32 no exchange\n");
	{
		volatile uint32_t value = 5;
		uint32_t comparand = 3;
		uint32_t exchange = 11;
		uint32_t after = fpl::atomics::AtomicCompareAndExchangeU32(&value, comparand, exchange);
		ft::AssertU32Equals((uint32_t)5, (uint32_t)value);
		ft::AssertU32Equals((uint32_t)5, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeS32 with exchange\n");
	{
		volatile int32_t value = -3;
		int32_t comparand = -3;
		int32_t exchange = 11;
		int32_t after = fpl::atomics::AtomicCompareAndExchangeS32(&value, comparand, exchange);
		ft::AssertS32Equals((int32_t)11, (int32_t)value);
		ft::AssertS32Equals((int32_t)-3, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeS32 no exchange\n");
	{
		volatile int32_t value = -5;
		int32_t comparand = -3;
		int32_t exchange = 11;
		int32_t after = fpl::atomics::AtomicCompareAndExchangeS32(&value, comparand, exchange);
		ft::AssertS32Equals((int32_t)-5, (int32_t)value);
		ft::AssertS32Equals((int32_t)-5, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeU64 with exchange\n");
	{
		volatile uint64_t value = 3;
		uint64_t comparand = 3;
		uint64_t exchange = 11;
		uint64_t after = fpl::atomics::AtomicCompareAndExchangeU64(&value, comparand, exchange);
		ft::AssertU64Equals((uint64_t)11, (uint64_t)value);
		ft::AssertU64Equals((uint64_t)3, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeU64 no exchange\n");
	{
		volatile uint64_t value = 5;
		uint64_t comparand = 3;
		uint64_t exchange = 11;
		uint64_t after = fpl::atomics::AtomicCompareAndExchangeU64(&value, comparand, exchange);
		ft::AssertU64Equals((uint64_t)5, (uint64_t)value);
		ft::AssertU64Equals((uint64_t)5, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeS64 with exchange\n");
	{
		volatile int64_t value = -3;
		int64_t comparand = -3;
		int64_t exchange = 11;
		int64_t after = fpl::atomics::AtomicCompareAndExchangeS64(&value, comparand, exchange);
		ft::AssertS64Equals((int64_t)11, (int64_t)value);
		ft::AssertS64Equals((int64_t)-3, after);
	}
	ft::Msg("Test AtomicCompareAndExchangeS64 no exchange\n");
	{
		volatile int64_t value = -5;
		int64_t comparand = -3;
		int64_t exchange = 11;
		int64_t after = fpl::atomics::AtomicCompareAndExchangeS64(&value, comparand, exchange);
		ft::AssertS64Equals((int64_t)-5, (int64_t)value);
		ft::AssertS64Equals((int64_t)-5, after);
	}
}

static void TestStrings() {
	ft::Msg("Test ansi string length\n");
	{
		uint32_t actual = fpl::strings::GetAnsiStringLength(nullptr);
		ft::AssertU32Equals(0, actual);
	}
	{
		uint32_t actual = fpl::strings::GetAnsiStringLength("");
		ft::AssertU32Equals(0, actual);
	}
	{
		uint32_t actual = fpl::strings::GetAnsiStringLength("ABC");
		ft::AssertU32Equals(3, actual);
	}
	{
		uint32_t actual = fpl::strings::GetAnsiStringLength("ABC Hello World!");
		ft::AssertU32Equals(16, actual);
	}
	{
		char buffer[32];
		buffer[0] = 'A';
		buffer[1] = 'B';
		buffer[2] = 'C';
		buffer[3] = 0;
		uint32_t actual = fpl::strings::GetAnsiStringLength(buffer);
		ft::AssertU32Equals(3, actual);
	}

	ft::Msg("Test wide string length\n");
	{
		uint32_t actual = fpl::strings::GetWideStringLength(nullptr);
		ft::AssertU32Equals(0, actual);
	}
	{
		uint32_t actual = fpl::strings::GetWideStringLength(L"");
		ft::AssertU32Equals(0, actual);
	}
	{
		uint32_t actual = fpl::strings::GetWideStringLength(L"ABC");
		ft::AssertU32Equals(3, actual);
	}
	{
		uint32_t actual = fpl::strings::GetWideStringLength(L"ABC Hello World!");
		ft::AssertU32Equals(16, actual);
	}
	{
		wchar_t buffer[32];
		buffer[0] = 'A';
		buffer[1] = 'B';
		buffer[2] = 'C';
		buffer[3] = 0;
		uint32_t actual = fpl::strings::GetWideStringLength(buffer);
		ft::AssertU32Equals(3, actual);
	}

	ft::Msg("Test string equal\n");
	{
		bool res = fpl::strings::IsStringEqual(nullptr, nullptr);
		FT_EXPECTS(true, res);
	}
	{
		bool res = fpl::strings::IsStringEqual(nullptr, 0, nullptr, 0);
		FT_EXPECTS(true, res);
	}
	{
		bool res = fpl::strings::IsStringEqual(nullptr, "");
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("B", "A");
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("A", "A");
		FT_EXPECTS(true, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("Hello", "World");
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("World", "World");
		FT_EXPECTS(true, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("B", 1, "A", 1);
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("A", 1, "A", 1);
		FT_EXPECTS(true, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("Hello", 5, "World", 5);
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("Hello", 3, "World", 5);
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("World", 5, "Hello", 3);
		FT_EXPECTS(false, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("Hello", 5, "Hello", 5);
		FT_EXPECTS(true, res);
	}
	{
		bool res = fpl::strings::IsStringEqual("Hello", 3, "Hello", 3);
		FT_EXPECTS(true, res);
	}

	ft::Msg("Test format ansi string\n");
	{
		char *res = fpl::strings::FormatAnsiString(nullptr, 0, nullptr);
		FT_EXPECTS(nullptr, res);
	}
	{
		char buffer[1];
		char *res = fpl::strings::FormatAnsiString(buffer, 0, "");
		FT_EXPECTS(nullptr, res);
	}
	{
		char buffer[1];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "A");
		FT_EXPECTS(nullptr, res);
	}
	{
		char buffer[2];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "A");
		bool matches = fpl::strings::IsStringEqual("A", 1, res, 1);
		FT_EXPECTS(true, matches);
	}
	{
		char buffer[5];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "Hello");
		FT_EXPECTS(nullptr, res);
	}
	{
		char buffer[6];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "Hello");
		bool r = fpl::strings::IsStringEqual("Hello", 5, res, 5);
		FT_EXPECTS(true, r);
	}
	{
		char buffer[6];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "%s", "Hello");
		bool r = fpl::strings::IsStringEqual("Hello", 5, res, 5);
		FT_EXPECTS(true, r);
	}
	{
		char buffer[20];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "%4xd-%2d-%2d %2d:%2d:%2d", 2009, 11, 17, 13, 47, 25);
		FT_EXPECTS(nullptr, res);
	}
	{
		char buffer[20];
		char *res = fpl::strings::FormatAnsiString(buffer, FPL_ARRAYCOUNT(buffer), "%4d-%2d-%2d %2d:%2d:%2d", 2009, 11, 17, 13, 47, 25);
		bool r = fpl::strings::IsStringEqual("2009-11-17 13:47:25", res);
		FT_EXPECTS(true, r);
	}
}


int main(int argc, char *args[]) {
	TestSizes();
	TestMacros();
	TestAtomics();
	TestMemory();
	TestPaths();
	TestFiles();
	TestStrings();
	TestThreading();
	TestInit();
	return 0;
}