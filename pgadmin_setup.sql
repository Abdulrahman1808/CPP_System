-- POS System Database Setup for pgAdmin
-- Run this in pgAdmin SQL tab to fix user/password issues
-- Enable pgcrypto for password hashing
CREATE EXTENSION IF NOT EXISTS pgcrypto;
-- Drop existing tables if they exist (for clean setup)
DROP TABLE IF EXISTS activity_log CASCADE;
DROP TABLE IF EXISTS sales_items CASCADE;
DROP TABLE IF EXISTS sales CASCADE;
DROP TABLE IF EXISTS products CASCADE;
DROP TABLE IF EXISTS users CASCADE;
-- Users table
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL
);
-- Products table
CREATE TABLE products (
    id SERIAL PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    category TEXT,
    price NUMERIC(10, 2) NOT NULL,
    quantity INTEGER NOT NULL,
    min_stock INTEGER NOT NULL,
    description TEXT
);
-- Sales table
CREATE TABLE sales (
    id SERIAL PRIMARY KEY,
    cashier TEXT REFERENCES users(username),
    sale_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    total NUMERIC(10, 2) NOT NULL,
    payment_method TEXT NOT NULL
);
-- Sales Items table
CREATE TABLE sales_items (
    id SERIAL PRIMARY KEY,
    sale_id INTEGER REFERENCES sales(id) ON DELETE CASCADE,
    product_name TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    price NUMERIC(10, 2) NOT NULL
);
-- Activity Log table
CREATE TABLE activity_log (
    id SERIAL PRIMARY KEY,
    username VARCHAR(64),
    action VARCHAR(255),
    details TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- Create admin user with password 'admin123'
INSERT INTO users (username, password_hash, role)
VALUES (
        'admin',
        crypt('admin123', gen_salt('bf')),
        'admin'
    );
-- Create cashier user with password 'cashier123'
INSERT INTO users (username, password_hash, role)
VALUES (
        'cashier',
        crypt('cashier123', gen_salt('bf')),
        'cashier'
    );
-- Verify the setup
SELECT 'Database setup completed successfully!' as status;
SELECT 'Users created:' as info;
SELECT username,
    role
FROM users;