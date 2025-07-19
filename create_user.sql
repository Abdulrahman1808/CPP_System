-- Create a default admin user for the POS system
-- Run this in your PostgreSQL database
-- Make sure pgcrypto is enabled
CREATE EXTENSION IF NOT EXISTS pgcrypto;
-- Insert admin user
INSERT INTO users (username, password_hash, role)
VALUES (
        'admin',
        crypt('admin123', gen_salt('bf')),
        'admin'
    );
-- Insert a cashier user (optional)
INSERT INTO users (username, password_hash, role)
VALUES (
        'cashier',
        crypt('cashier123', gen_salt('bf')),
        'cashier'
    );
-- Verify the users were created
SELECT username,
    role
FROM users;