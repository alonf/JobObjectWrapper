#include "StdAfx.h"
#include "IOCounters.h"

namespace JobManagement 
{
	JobIOCounters::JobIOCounters(const IO_COUNTERS& ioCounters)
	{
		ReadOperationCount = ioCounters.ReadOperationCount;
		WriteOperationCount = ioCounters.WriteOperationCount;
		OtherOperationCount = ioCounters.OtherOperationCount;
		ReadTransferCount = ioCounters.ReadTransferCount;
		WriteTransferCount = ioCounters.WriteTransferCount;
		OtherTransferCount = ioCounters.OtherTransferCount;
	}
}