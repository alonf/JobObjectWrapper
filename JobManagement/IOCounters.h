#pragma once
#include "stdafx.h"
#include <Windows.h>
namespace JobManagement 
{
	public value class JobIOCounters
	{
	public:
		property System::UInt64 ReadOperationCount;
		property System::UInt64 WriteOperationCount;
		property System::UInt64 OtherOperationCount;
		property System::UInt64 ReadTransferCount;
		property System::UInt64 WriteTransferCount;
		property System::UInt64 OtherTransferCount;
	internal:
		JobIOCounters(const IO_COUNTERS& ioCounters);
	};
}