DROP TABLE IF EXISTS source CASCADE;
DROP TABLE IF EXISTS position_command CASCADE;
DROP TABLE IF EXISTS log_info CASCADE;
DROP TABLE IF EXISTS position_command CASCADE;
DROP TABLE IF EXISTS joint_demand CASCADE;
DROP TABLE IF EXISTS joint_report CASCADE;
DROP INDEX IF EXISTS joint_report_sourceid_logtime_idx;
DROP TABLE IF EXISTS joint_state CASCADE;
DROP TABLE IF EXISTS parameter_report CASCADE;
DROP INDEX IF EXISTS parameter_report_sourceid_parameter_logtime_idx;
DROP INDEX IF EXISTS emergency_stop_sourceid_logtime_idx;
DROP TABLE IF EXISTS emergency_stop CASCADE;
DROP TABLE IF EXISTS error CASCADE;
DROP INDEX IF EXISTS error_sourceid_logtime_idx;
DROP TABLE IF EXISTS joint_state_pid CASCADE;
DROP TABLE IF EXISTS imu CASCADE;
