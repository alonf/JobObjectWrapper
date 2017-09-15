<?xml version="1.0"?><doc>
<members>
<member name="T:JobManagement.JobException" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="19">
<summary>
This exception class is used to inform the caller of most of the Job Object wrapper errors.
</summary>
<remarks> 
If the error occurred by Win32 the Win32 GetLastError() code can be found in <see cref="P:JobManagement.JobException.Win32LastError"/> 
and the text FormatMessage message can be found in <see cref="P:JobManagement.JobException.Win32ErrorMessage"/>
</remarks>
</member>
<member name="M:JobManagement.JobException.#ctor" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="30">
<summary>
Initializes a new instance of JobException
</summary>
</member>
<member name="M:JobManagement.JobException.#ctor(System.Boolean)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="35">
<summary>
Initializes a new instance of JobException
</summary>
<param name="useCurrentWin32Error">true value will peek the current Win32 error.</param>
</member>
<member name="M:JobManagement.JobException.#ctor(System.Boolean,System.Exception)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="41">
<summary>
Initializes a new instance of JobException
</summary>
<param name="useCurrentWin32Error">true value will peek the current Win32 error.</param>
<param name="innerException">The exception that is the cause of the current exception, or a null reference
(Nothing in Visual Basic) if no inner exception is specified.
</param>
</member>
<member name="M:JobManagement.JobException.#ctor(System.UInt32)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="50">
<summary>
Initializes a new instance of JobException
</summary>
<param name="win32Error">The win32 error that is the cause of the current exception</param>
</member>
<member name="M:JobManagement.JobException.#ctor(System.UInt32,System.Exception)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="56">
<summary>
Initializes a new instance of JobException
</summary>
<param name="win32Error">The win32 error that is the cause of the current exception</param>
<param name="innerException">The exception that is the cause of the current exception, or a null reference
(Nothing in Visual Basic) if no inner exception is specified.
</param>
</member>
<member name="M:JobManagement.JobException.#ctor(System.String)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="65">
<summary>
Initializes a new instance of JobException
</summary>
<param name="message">The message that describes the error.</param>
</member>
<member name="M:JobManagement.JobException.#ctor(System.String,System.Exception)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="71">
<summary>
Initializes a new instance of JobException
</summary>
<param name="message">The message that describes the error.</param>
<param name="innerException">The exception that is the cause of the current exception, or a null reference
(Nothing in Visual Basic) if no inner exception is specified.
</param>
</member>
<member name="M:JobManagement.JobException.#ctor(System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="82">
<summary>
Initializes a new instance of JobException
</summary>
<param name="info">The <see cref="T:System.Runtime.Serialization.SerializationInfo"/> that holds the serialized
object data about the exception being thrown.</param>
<param name="context"> The <see cref="T:System.Runtime.Serialization.StreamingContext"/> that contains contextual
information about the source or destination.</param>
</member>
<member name="P:JobManagement.JobException.Win32LastError" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="93">
<summary>
The win32 error that is the cause of the current exception
</summary>
<returns></returns>
</member>
<member name="P:JobManagement.JobException.Win32ErrorMessage" decl="false" source="c:\users\alon\source\repos\jobobjectwrapper\jobmanagement\jobexception.h" line="98">
<summary>
The win32 error message describing the cause of the current exception.
</summary>
<returns>The win32 error message describing the cause of the current exception.</returns>
</member>
</members>
</doc>