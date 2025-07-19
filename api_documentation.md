# POS System API Documentation

Your POS system now includes an HTTP API server that allows remote access to sales, inventory, and activity data.

## Base URL

```
http://YOUR_SERVER_IP:8080
```

## Authentication

All API requests require a Bearer token in the Authorization header:

```
Authorization: Bearer pos_api_key_2024
```

## API Endpoints

### 1. Get Sales Data

**GET** `/api/sales`

Returns the latest 50 sales with details.

**Response:**

```json
{
	"success": true,
	"data": [
		{
			"id": 1,
			"cashier": "admin",
			"sale_time": "2024-01-15T10:30:00",
			"total": 25.5,
			"payment_method": "Cash",
			"item_count": 3
		}
	]
}
```

### 2. Get Inventory Status

**GET** `/api/inventory`

Returns all products with current stock levels.

**Response:**

```json
{
	"success": true,
	"data": [
		{
			"id": 1,
			"name": "Coffee",
			"category": "Beverages",
			"price": 3.5,
			"quantity": 10,
			"min_stock": 5,
			"description": "Hot coffee"
		}
	]
}
```

### 3. Get Activity Log

**GET** `/api/activity-log`

Returns the latest 100 user actions.

**Response:**

```json
{
	"success": true,
	"data": [
		{
			"username": "admin",
			"action": "Login",
			"details": "User logged in",
			"timestamp": "2024-01-15T10:30:00"
		}
	]
}
```

### 4. Get Summary Statistics

**GET** `/api/summary`

Returns business summary statistics.

**Response:**

```json
{
	"success": true,
	"data": {
		"total_sales": 1250.75,
		"total_orders": 45,
		"today_sales": 125.5,
		"today_orders": 5,
		"low_stock_items": 3,
		"total_products": 25
	}
}
```

### 5. Create New Sale (Admin Only)

**POST** `/api/sales`

Creates a new sale record.

**Request Body:**

```json
{
	"cashier": "admin",
	"total": 25.5,
	"payment_method": "Cash",
	"items": [
		{
			"product_name": "Coffee",
			"quantity": 2,
			"price": 3.5
		}
	]
}
```

**Response:**

```json
{
	"success": true,
	"data": {
		"sale_id": 123,
		"message": "Sale created successfully"
	}
}
```

## Error Responses

All endpoints return error responses in this format:

```json
{
	"success": false,
	"error": "Error message"
}
```

Common HTTP status codes:

- `200` - Success
- `400` - Bad Request (missing fields, invalid JSON)
- `401` - Unauthorized (invalid API key)
- `500` - Server Error (database issues)

## Example Usage

### Using curl:

```bash
# Get sales data
curl -H "Authorization: Bearer pos_api_key_2024" \
     http://YOUR_SERVER_IP:8080/api/sales

# Get inventory
curl -H "Authorization: Bearer pos_api_key_2024" \
     http://YOUR_SERVER_IP:8080/api/inventory

# Get summary
curl -H "Authorization: Bearer pos_api_key_2024" \
     http://YOUR_SERVER_IP:8080/api/summary
```

### Using JavaScript:

```javascript
const apiKey = 'pos_api_key_2024';
const baseUrl = 'http://YOUR_SERVER_IP:8080';

// Get sales data
fetch(`${baseUrl}/api/sales`, {
	headers: {
		Authorization: `Bearer ${apiKey}`,
	},
})
	.then((response) => response.json())
	.then((data) => console.log(data));
```

## Security Notes

1. **API Key**: The current API key is `pos_api_key_2024`. In production, use a more secure key.
2. **HTTPS**: For production use, enable HTTPS for secure communication.
3. **Rate Limiting**: Consider implementing rate limiting for API endpoints.
4. **IP Whitelisting**: Consider restricting access to specific IP addresses.

## Troubleshooting

1. **Connection Refused**: Make sure the POS app is running and the HTTP server started successfully.
2. **401 Unauthorized**: Check that the Authorization header includes the correct API key.
3. **500 Server Error**: Check the POS app console for database connection issues.

## Next Steps

1. **Find Your IP Address**: Run `ipconfig` (Windows) or `ifconfig` (Linux/Mac) to get your local IP.
2. **Port Forwarding**: Configure your router to forward port 8080 to your computer.
3. **Test Access**: Have coworkers test the API endpoints from their machines.
4. **Create Dashboard**: Build a web dashboard using these API endpoints.
