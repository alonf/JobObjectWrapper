
/*******************************************************************************************************  
*
* IOCounters.h
*
* Copyright 2007 The JobObjectWrapper Team  
* http://www.codeplex.com/JobObjectWrapper/Wiki/View.aspx?title=Team
*
* This program is licensed under the Microsoft Permissive License (Ms-PL).  You should 
* have received a copy of the license along with the source code.  If not, an online copy
* of the license can be found at http://www.codeplex.com/JobObjectWrapper/Project/License.aspx.
*   
*  Notes :
*      - First release by Alon Fliess
********************************************************************************************************/

#pragma once
#include "stdafx.h"
#include <Windows.h>
namespace JobManagement 
{
	/// <summary>
	/// Contains basic accounting and I/O accounting information for a job object.
	/// </summary>
	/// <remarks>
	/// Includes information for all processes that have ever been associated with the job, 
	/// in addition to the information for all processes currently associated with the job. 
	/// </remarks>
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