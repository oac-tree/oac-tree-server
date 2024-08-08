# Todo

* Fix todos
* Move all server stuff in a separate folder and only expose the absolute necessary (for main.cpp) in the public API
* Add/reactivate and fix unit tests
* Implement AutomationClient, AutomationProtocolClient
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS

```c++
class AutomationClient;
class IJobManager;
class IJobInfoIO;
class IJobInfoIORegistry;
class ListenerFactoryFunction;

IJobManager job_manager;
IJobInfoIO job_info_io;
IJobInfoIORegistry job_info_io_registry;
ListenerFactoryFunction fact_func;

AutomationClient client{job_manager, fact_func};
bool success;

// connect only single job
// will fail when job_idx does not exist or job was already connected
success = client.connect(job_idx, job_info_io);

// connect all jobs
// what to do when some jobs were already connected?
success = client.ConnectAll(job_info_io_registry);
```
