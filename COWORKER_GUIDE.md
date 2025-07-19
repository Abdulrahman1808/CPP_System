# 🏪 POS System - Coworker Access Guide

## 📋 Table of Contents

1. [Quick Start](#quick-start)
2. [Data Access Methods](#data-access-methods)
3. [Development Setup](#development-setup)
4. [API Documentation](#api-documentation)
5. [Database Access](#database-access)
6. [Troubleshooting](#troubleshooting)

---

## 🚀 Quick Start

### For Data Access Only (No Installation Required)

1. **Open your web browser**
2. **Go to:** `http://192.168.1.36:8080/api/sales`
3. **You'll see real-time sales data in JSON format**

### For Full Application Access

1. **Download the application** from the shared network folder
2. **Run:** `POSApp.exe`
3. **Login with:**
   - Admin: `admin` / `admin123`
   - Cashier: `cashier` / `cashier123`

---

## 📊 Data Access Methods

### Method 1: HTTP API (Recommended)

**Base URL:** `http://192.168.1.36:8080`

#### Available Endpoints:

- **Sales Data:** `GET /api/sales`
- **Inventory:** `GET /api/inventory`
- **Activity Log:** `GET /api/activity-log`
- **Summary:** `GET /api/summary`

#### Example Usage:

```bash
# Get all sales
curl "http://192.168.1.36:8080/api/sales"

# Get inventory status
curl "http://192.168.1.36:8080/api/inventory"

# Get activity log
curl "http://192.168.1.36:8080/api/activity-log"
```

#### Mobile/Tablet Access:

- **Open browser on your phone**
- **Go to:** `http://192.168.1.36:8080/api/inventory`
- **View real-time data anywhere!**

### Method 2: Direct Database Access

**Connection Details:**

- **Host:** `localhost` (or machine IP)
- **Port:** `5432`
- **Database:** `MonsterDB`
- **Username:** `postgres`
- **Password:** `Monsterxd2005@#@#`

#### Tools You Can Use:

- **pgAdmin** (Windows GUI)
- **DBeaver** (Cross-platform)
- **Any SQL client**

### Method 3: Shared Network Folder

1. **Navigate to:** `\\network\path\build\`
2. **Run:** `POSApp.exe`
3. **Login and start working!**

---

## 💻 Development Setup

### Prerequisites:

- **Qt 6.9.1** with MinGW
- **PostgreSQL** (local installation)
- **Git** (for version control)

### Step-by-Step Setup:

#### 1. Clone the Repository

```bash
git clone https://github.com/Abdulrahman1808/CPP_System.git
cd CPP_System
```

#### 2. Setup Database

1. **Open pgAdmin**
2. **Create database:** `MonsterDB`
3. **Run SQL script:** `pgadmin_setup.sql`
4. **Verify users are created**

#### 3. Build Application

```bash
# Windows
build_qt.bat

# Or manually
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

#### 4. Deploy Dependencies

```bash
C:\Qt\6.9.1\mingw_64\bin\windeployqt.exe build\POSApp.exe
```

---

## 🔌 API Documentation

### Authentication

**API Key:** `pos_api_key_2024` (for future use)

### Endpoints

#### GET /api/sales

Returns all sales data.

**Response:**

```json
{
	"sales": [
		{
			"id": 1,
			"cashier": "admin",
			"sale_time": "2024-01-15T10:30:00",
			"total": 25.5,
			"payment_method": "cash"
		}
	]
}
```

#### GET /api/inventory

Returns current inventory status.

**Response:**

```json
{
	"products": [
		{
			"id": 1,
			"name": "Coca Cola",
			"category": "Beverages",
			"price": 2.5,
			"quantity": 100,
			"min_stock": 10
		}
	]
}
```

#### GET /api/activity-log

Returns system activity log.

**Response:**

```json
{
	"activities": [
		{
			"id": 1,
			"username": "admin",
			"action": "Login",
			"details": "User logged in",
			"timestamp": "2024-01-15T10:30:00"
		}
	]
}
```

#### GET /api/summary

Returns system summary statistics.

**Response:**

```json
{
	"total_sales": 1500.75,
	"total_products": 25,
	"low_stock_items": 3,
	"today_sales": 125.5
}
```

---

## 🗄️ Database Access

### Schema Overview

#### Tables:

- **users** - User accounts and roles
- **products** - Inventory items
- **sales** - Sales transactions
- **sales_items** - Individual items in sales
- **activity_log** - System activity tracking

#### Key Queries:

**Get all sales:**

```sql
SELECT * FROM sales ORDER BY sale_time DESC;
```

**Get low stock items:**

```sql
SELECT * FROM products WHERE quantity <= min_stock;
```

**Get today's sales:**

```sql
SELECT * FROM sales WHERE DATE(sale_time) = CURRENT_DATE;
```

**Get user activity:**

```sql
SELECT * FROM activity_log ORDER BY timestamp DESC;
```

---

## 🔧 Troubleshooting

### Common Issues:

#### 1. "Connection Failed"

- **Check if the application is running**
- **Verify IP address:** `192.168.1.36:8080`
- **Check firewall settings**

#### 2. "Database Connection Error"

- **Ensure PostgreSQL is running**
- **Verify credentials in main.cpp**
- **Check if MonsterDB exists**

#### 3. "Login Failed"

- **Verify users exist in database**
- **Check password hashing**
- **Run pgadmin_setup.sql again**

#### 4. "Build Failed"

- **Install Qt 6.9.1 with MinGW**
- **Check CMake installation**
- **Verify all dependencies**

### Network Issues:

- **Ensure port 8080 is open**
- **Check Windows Firewall**
- **Verify network connectivity**

### Database Issues:

- **Restart PostgreSQL service**
- **Check pg_hba.conf for remote access**
- **Verify database exists**

---

## 📞 Support

### For Technical Issues:

- **Check the logs** in the application
- **Review database connection** settings
- **Verify network connectivity**

### For Data Access:

- **Use HTTP API** for real-time data
- **Connect directly to database** for detailed queries
- **Use shared network folder** for full application access

### Repository:

- **GitHub:** https://github.com/Abdulrahman1808/CPP_System.git
- **Latest updates** are always pushed here
- **Pull regularly** for new features

---

## 🎯 Quick Reference

### URLs:

- **API Base:** `http://192.168.1.36:8080`
- **Sales:** `http://192.168.1.36:8080/api/sales`
- **Inventory:** `http://192.168.1.36:8080/api/inventory`

### Credentials:

- **Admin:** `admin` / `admin123`
- **Cashier:** `cashier` / `cashier123`
- **Database:** `postgres` / `Monsterxd2005@#@#`

### Build Commands:

```bash
build_qt.bat                    # Build application
windeployqt.exe build\POSApp.exe # Deploy dependencies
```

---

**Last Updated:** January 2024  
**Version:** 1.0  
**Maintainer:** Abdulrahman
