install(FILES
  ./concurrent_queue.hpp
  ./dag.h
  ./dag_algorithms.h
  ./dag_edge.h
  ./dag_vertex.h
  ./https_session.h
  ./interruptible_task_thread.h
  ./logging.h
  ./stop_watch.h
  ./task.h
  ./task_scheduler.h
  ./task_stage.h
  ./uuid.h
  ./workflow_service.h
  DESTINATION include/dag_scheduler
)
