#include "ShareMemory.h"

void send() {
	SYSTEMTIME sys_time;
	LPVOID pBuffer;
	string strMapName("global_share_memory");
	HANDLE hMap = NULL;

	hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"global_share_memory");
	if (hMap == NULL) {
		cout << "no memorymap,i will creat a new one" << endl;
		hMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 0X1000, L"global_share_memory");
	}

	pBuffer = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	vector<double> start;
	start.push_back(1.5);
	memcpy((double*)pBuffer, &start[0], sizeof(start));

	system("pause");
	start.push_back(1.2);
	memcpy((double*)pBuffer, &start[0], sizeof(start));
	system("pause");

	UnmapViewOfFile(pBuffer);
	CloseHandle(hMap);
}