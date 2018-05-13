install(FILES
  event-summary.sql
  max-events.sql
  module-summary.sql
  DESTINATION "${${product}_data_dir}/tools/sqlite/timeTracker")
