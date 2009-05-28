
/*******************************************************************************************************  
*
* IOCounters.cpp
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