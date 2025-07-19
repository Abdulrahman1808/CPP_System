-- Activity Log Table Setup for POS System
-- Run this file in PostgreSQL to create the activity_log table for audit trail
-- Create the activity_log table for tracking user actions
CREATE TABLE IF NOT EXISTS activity_log (
    id SERIAL PRIMARY KEY,
    username VARCHAR(64),
    action VARCHAR(255),
    details TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- Add an index on timestamp for better query performance
CREATE INDEX IF NOT EXISTS idx_activity_log_timestamp ON activity_log(timestamp);
-- Add an index on username for filtering by user
CREATE INDEX IF NOT EXISTS idx_activity_log_username ON activity_log(username);
-- Example: Insert a test log entry (optional)
-- INSERT INTO activity_log (username, action, details) VALUES ('system', 'setup', 'Activity log table created');