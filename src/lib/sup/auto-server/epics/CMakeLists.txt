target_sources(sup-auto-server
  PRIVATE
  epics_job_pv_server.cpp
  epics_pv_handler.cpp
  job_pv_info.cpp
  pv_update_command.cpp
  pv_update_queue.cpp
)
