-- POS System Database Schema
-- Run this file in PostgreSQL to create the required tables
-- Enable pgcrypto for password hashing
CREATE EXTENSION IF NOT EXISTS pgcrypto;
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
    sale_time TIMESTAMP NOT NULL DEFAULT NOW(),
    total NUMERIC(10, 2) NOT NULL,
    payment_method TEXT NOT NULL
);
-- Sales Items table
CREATE TABLE sales_items (
    id SERIAL PRIMARY KEY,
    sale_id INTEGER REFERENCES sales(id) ON DELETE CASCADE,
    product_name TEXT REFERENCES products(name),
    quantity INTEGER NOT NULL,
    price NUMERIC(10, 2) NOT NULL
);
-- Activity Log table
CREATE TABLE activity_log (
    id SERIAL PRIMARY KEY,
    username TEXT REFERENCES users(username),
    action TEXT,
    details TEXT,
    timestamp TIMESTAMP NOT NULL DEFAULT NOW()
);
-- Example: Insert an admin user (replace password as needed)
-- INSERT INTO users (username, password_hash, role) VALUES ('admin', crypt('yourpassword', gen_salt('bf')), 'admin');
