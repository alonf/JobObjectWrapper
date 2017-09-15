# JobObjectWrapper
A job object allows a group of processes to be managed as a unit. Job objects are namable, securable, sharable objects that control attributes of and assign limits to the processes associated with them. Operations performed on the job object affect all processes associated with the job object.
JobObjectWrapper is a .NET abstraction over the Win32 Job Object. With this library you can create job objects, create and assign a process to the job, control process and job limits, and register for the various process- and job-related notification events.

