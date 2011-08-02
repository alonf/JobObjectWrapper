
/*******************************************************************************************************  
*
* JobException.h
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
	/// This exception class is used to inform the caller of most of the Job Object wrapper errors.
	/// </summary>
	/// <remarks> 
	/// If the error occurred by Win32 the Win32 GetLastError() code can be found in <see cref="JobException::Win32LastError"/> 
	/// and the text FormatMessage message can be found in <see cref="JobException::Win32ErrorMessage"/>
	/// </remarks>
	[System::Serializable]
	public ref class JobException : public System::Exception
	{
	public:
		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		JobException() {}

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="useCurrentWin32Error">true value will peek the current Win32 error.</param>
		JobException(bool useCurrentWin32Error) : _win32Error(useCurrentWin32Error ? ::GetLastError() : 0), System::Exception(Win32ErrorMessage) {}

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="useCurrentWin32Error">true value will peek the current Win32 error.</param>
		/// <param name="innerException">The exception that is the cause of the current exception, or a null reference
        /// (Nothing in Visual Basic) if no inner exception is specified.
		/// </param>
		JobException(bool useCurrentWin32Error, System::Exception ^innerException) : _win32Error(useCurrentWin32Error ? ::GetLastError() : 0), System::Exception(Win32ErrorMessage, innerException) {}

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="win32Error">The win32 error that is the cause of the current exception</param>
		JobException(unsigned int win32Error) : _win32Error(win32Error), System::Exception(Win32ErrorMessage) {}

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="win32Error">The win32 error that is the cause of the current exception</param>
		/// <param name="innerException">The exception that is the cause of the current exception, or a null reference
        /// (Nothing in Visual Basic) if no inner exception is specified.
		/// </param>
		JobException(unsigned int win32Error, System::Exception ^innerException) : _win32Error(win32Error), System::Exception(Win32ErrorMessage, innerException) {}

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="message">The message that describes the error.</param>
		JobException(System::String ^message) : System::Exception(message) {}

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="message">The message that describes the error.</param>
		/// <param name="innerException">The exception that is the cause of the current exception, or a null reference
        /// (Nothing in Visual Basic) if no inner exception is specified.
		/// </param>
		JobException(System::String ^message, System::Exception ^innerException)
			: System::Exception(message, innerException) {}
	protected:

		/// <summary>
		/// Initializes a new instance of JobException
		/// </summary>
		/// <param name="info">The <see cref="System::Runtime::Serialization::SerializationInfo"/> that holds the serialized
        /// object data about the exception being thrown.</param>
		/// <param name="context"> The <see cref="System::Runtime::Serialization::StreamingContext"/> that contains contextual
        /// information about the source or destination.</param>
		JobException(System::Runtime::Serialization::SerializationInfo ^info,
			System::Runtime::Serialization::StreamingContext context)
			: System::Exception(info, context) {}
	public:
		/// <summary>
		/// The win32 error that is the cause of the current exception
		/// </summary>
		/// <returns></returns>
		property System::Nullable<unsigned int> Win32LastError {System::Nullable<unsigned int> get();}
		/// <summary>
		/// The win32 error message describing the cause of the current exception.
		/// </summary>
		/// <returns>The win32 error message describing the cause of the current exception.</returns>
		property System::String ^Win32ErrorMessage { System::String ^get(); }

	private:
		System::Nullable<unsigned int> _win32Error;
	};
}