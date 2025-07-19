-- Complete POS System Database Setup
-- Run this file in your PostgreSQL database (pgAdmin, DBeaver, or Neon console)
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
-- Create admin user
INSERT INTO users (username, password_hash, role)
VALUES (
        'admin',
        crypt('admin123', gen_salt('bf')),
        'admin'
    );
-- Create cashier user
INSERT INTO users (username, password_hash, role)
VALUES (
        'cashier',
        crypt('cashier123', gen_salt('bf')),
        'cashier'
    );
-- Add some sample products
INSERT INTO products (
        name,
        category,
        price,
        quantity,
        min_stock,
        description
    )
VALUES (
        'Coca Cola',
        'Beverages',
        2.50,
        100,
        10,
        '330ml can'
    ),
    ('Pepsi', 'Beverages', 2.50, 80, 10, '330ml can'),
    ('Chips', 'Snacks', 1.99, 50, 5, 'Potato chips'),
    (
        'Chocolate Bar',
        'Candy',
        1.50,
        75,
        5,
        'Milk chocolate'
    ),
    ('Bread', 'Bakery', 3.99, 20, 5, 'Fresh bread'),
    ('Milk', 'Dairy', 4.50, 30, 5, '1L fresh milk'),
    ('Eggs', 'Dairy', 5.99, 40, 5, '12 eggs'),
    ('Apple', 'Fruits', 0.99, 100, 10, 'Fresh apple'),
    (
        'Banana',
        'Fruits',
        0.79,
        120,
        10,
        'Fresh banana'
    ),
    (
        'Water Bottle',
        'Beverages',
        1.50,
        60,
        10,
        '500ml water'
    );
-- Verify setup
SELECT 'Users created:' as info;
SELECT username,
    role
FROM users;
SELECT 'Products added:' as info;
SELECT name,
    category,
    price,
    quantity
FROM products
LIMIT 5;