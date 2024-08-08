# Todo

* Fix todos
* Move all server stuff in a separate folder and only expose the absolute necessary (for main.cpp) in the public API
* Add/reactivate and fix unit tests
* Look for bottlenecks: e.g. using std::map with string keys to identify the name server for EPICS
* Implement other JobInterface methods over the network

Should there be something like:

```c++
IJobInfoIORegistry job_info_io_registry;
AutomationClient client{job_manager, fact_func};
bool success;

// connect all jobs
// what to do when some jobs were already connected?
success = client.ConnectAll(job_info_io_registry);
```
