target_sources(sup-auto-server
  PRIVATE
  epics_instruction_pv_handler.cpp
  epics_job_pv_handler.cpp
  epics_job_pv_server.cpp
  pv_update_command.cpp
  pv_update_queue.cpp
)
