#include "main.h"

using namespace std;

optional<int> ParseInt(const basic_string<char> &str) {
    try {
        return make_optional<int>(stoi(str));
    } catch (...) {
        return nullopt;
    }
}

optional<int> ParseThreadCount(int argc, char *argv[], ostream &out) {
    if (argc != 2) {
        out << "Invalid argument count.\n";
        out << "Usage: lab1.exe <number of threads>\n";
        return nullopt;
    }
    return ParseInt(argv[1]);
}

void InitDtoObject(PTHREADDTO *pThreadDtoArray, int &i) {
    auto localIdx = *new int(i);
    pThreadDtoArray[i] = (PTHREADDTO) HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(THREADDTO)
    );

    if (pThreadDtoArray[i] == nullptr) {
        ExitProcess(2);
    }

    pThreadDtoArray[i]->addressOfIndex = &i;
    pThreadDtoArray[i]->addressOfLocalIndex = &localIdx;
    pThreadDtoArray[i]->indexByValue = i;
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam) {
    HANDLE hStdout;
    PTHREADDTO pThreadDto;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        return 1;

    pThreadDto = (PTHREADDTO) lpParam;

    StringCchPrintf(msgBuf, BUF_SIZE,
                    TEXT("addressOfIndex: %d, addressOfLocalIndex: %d, indexByValue: %d\n"),
                    *pThreadDto->addressOfIndex,
                    *pThreadDto->addressOfLocalIndex,
                    pThreadDto->indexByValue
    );
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(
            hStdout,
            msgBuf,
            (DWORD) cchStringSize,
            &dwChars,
            nullptr
    );

    ExitThread(0);
}

vector<HANDLE> InitHandles(const int threadCount) {
    vector<HANDLE> handles;
    PTHREADDTO pThreadDtoArray[threadCount];
    DWORD dwThreadIdArray[threadCount];

    for (int i = 0; i < threadCount; ++i) {
        InitDtoObject(pThreadDtoArray, i);

        auto currThread = CreateThread(
                nullptr,
                0,
                &ThreadProc,
                pThreadDtoArray[i],
                CREATE_SUSPENDED,
                &dwThreadIdArray[i]
        );

        handles.push_back(currThread);
    }
    return handles;
}

void ResumeHandles(const vector<HANDLE> &handles) {
    for (const auto &handle: handles) {
        ResumeThread(handle);
    }
}

void CloseThreads(vector<HANDLE> handles, int &threadCount) {
    for (int i = 0; i < threadCount; i++) {
        CloseHandle(handles[i]);
    }
}

int main(int argc, char *argv[]) {
    auto threadCount = ParseThreadCount(argc, argv, cout);
    if (!threadCount) {
        return 1;
    }

    auto handles = InitHandles(threadCount.value());
    ResumeHandles(handles);

    WaitForMultipleObjects(handles.size(), handles.data(), true, INFINITE);

    CloseThreads(handles, threadCount.value());
    return 0;
}
