#include "InventoryScreen.h"
#include "ReportsScreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFont>
#include <QFrame>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

// AddProductDialog Implementation
AddProductDialog::AddProductDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Add New Product");
    setModal(true);
    resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QGridLayout *formLayout = new QGridLayout;
    
    // Product Name
    formLayout->addWidget(new QLabel("Product Name:"), 0, 0);
    nameEdit = new QLineEdit;
    nameEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; }");
    formLayout->addWidget(nameEdit, 0, 1);
    
    // Category
    formLayout->addWidget(new QLabel("Category:"), 1, 0);
    categoryEdit = new QLineEdit;
    categoryEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; }");
    formLayout->addWidget(categoryEdit, 1, 1);
    
    // Price
    formLayout->addWidget(new QLabel("Price ($):"), 2, 0);
    priceSpinBox = new QDoubleSpinBox;
    priceSpinBox->setMinimum(0.01);
    priceSpinBox->setMaximum(9999.99);
    priceSpinBox->setDecimals(2);
    priceSpinBox->setValue(0.00);
    priceSpinBox->setStyleSheet("QDoubleSpinBox { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; }");
    formLayout->addWidget(priceSpinBox, 2, 1);
    
    // Quantity
    formLayout->addWidget(new QLabel("Quantity:"), 3, 0);
    quantitySpinBox = new QSpinBox;
    quantitySpinBox->setMinimum(0);
    quantitySpinBox->setMaximum(9999);
    quantitySpinBox->setValue(0);
    quantitySpinBox->setStyleSheet("QSpinBox { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; }");
    formLayout->addWidget(quantitySpinBox, 3, 1);
    
    // Minimum Stock
    formLayout->addWidget(new QLabel("Min Stock:"), 4, 0);
    minStockSpinBox = new QSpinBox;
    minStockSpinBox->setMinimum(0);
    minStockSpinBox->setMaximum(999);
    minStockSpinBox->setValue(5);
    minStockSpinBox->setStyleSheet("QSpinBox { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; }");
    formLayout->addWidget(minStockSpinBox, 4, 1);
    
    // Description
    formLayout->addWidget(new QLabel("Description:"), 5, 0);
    descriptionEdit = new QLineEdit;
    descriptionEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; }");
    formLayout->addWidget(descriptionEdit, 5, 1);
    
    layout->addLayout(formLayout);
    
    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->setStyleSheet("QPushButton { padding: 8px 16px; border: none; border-radius: 6px; background: #2196F3; color: white; } QPushButton:hover { background: #1976D2; } QPushButton[text='Cancel'] { background: #666; } QPushButton[text='Cancel']:hover { background: #555; }");
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    layout->addWidget(buttonBox);
}

Product AddProductDialog::getProduct() const {
    Product product;
    product.name = nameEdit->text();
    product.category = categoryEdit->text();
    product.price = priceSpinBox->value();
    product.quantity = quantitySpinBox->value();
    product.minStock = minStockSpinBox->value();
    product.description = descriptionEdit->text();
    return product;
}

// InventoryScreen Implementation
QList<Product> InventoryScreen::getProducts() const {
    return products;
}

void InventoryScreen::loadProductsFromDatabase() {
    products.clear();
    QSqlQuery query("SELECT id, name, category, price, quantity, min_stock, description FROM products");
    while (query.next()) {
        Product p;
        p.id = query.value(0).toInt();
        p.name = query.value(1).toString();
        p.category = query.value(2).toString();
        p.price = query.value(3).toDouble();
        p.quantity = query.value(4).toInt();
        p.minStock = query.value(5).toInt();
        p.description = query.value(6).toString();
        products.append(p);
    }
    refreshInventory();
}

InventoryScreen::InventoryScreen(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Header
    QLabel *headerLabel = new QLabel("Inventory Management");
    QFont headerFont = headerLabel->font();
    headerFont.setPointSize(24);
    headerFont.setBold(true);
    headerLabel->setFont(headerFont);
    headerLabel->setStyleSheet("color: #ffffff; margin-bottom: 10px;");
    mainLayout->addWidget(headerLabel);

    // Search and Actions Bar
    QHBoxLayout *searchLayout = new QHBoxLayout;
    
    QLabel *searchLabel = new QLabel("Search:");
    searchLabel->setStyleSheet("color: #ffffff; font-size: 14px;");
    searchBox = new QLineEdit;
    searchBox->setPlaceholderText("Search products...");
    searchBox->setStyleSheet("QLineEdit { padding: 10px; border: 2px solid #444; border-radius: 8px; background: #2d313a; color: white; font-size: 14px; } QLineEdit:focus { border-color: #2196F3; }");
    connect(searchBox, &QLineEdit::textChanged, this, &InventoryScreen::searchProducts);
    
    addButton = new QPushButton("Add Product");
    addButton->setStyleSheet("QPushButton { background: #4CAF50; color: white; border: none; border-radius: 8px; padding: 10px; font-size: 14px; font-weight: bold; } QPushButton:hover { background: #45a049; }");
    connect(addButton, &QPushButton::clicked, this, &InventoryScreen::addProduct);
    
    editButton = new QPushButton("Edit Product");
    editButton->setStyleSheet("QPushButton { background: #2196F3; color: white; border: none; border-radius: 8px; padding: 10px; font-size: 14px; font-weight: bold; } QPushButton:hover { background: #1976D2; }");
    connect(editButton, &QPushButton::clicked, this, &InventoryScreen::editProduct);
    
    deleteButton = new QPushButton("Delete Product");
    deleteButton->setStyleSheet("QPushButton { background: #F44336; color: white; border: none; border-radius: 8px; padding: 10px; font-size: 14px; font-weight: bold; } QPushButton:hover { background: #d32f2f; }");
    connect(deleteButton, &QPushButton::clicked, this, &InventoryScreen::deleteProduct);
    
    refreshButton = new QPushButton("Refresh");
    refreshButton->setStyleSheet("QPushButton { background: #FF9800; color: white; border: none; border-radius: 8px; padding: 10px; font-size: 14px; font-weight: bold; } QPushButton:hover { background: #F57C00; }");
    connect(refreshButton, &QPushButton::clicked, this, &InventoryScreen::refreshInventory);
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchBox);
    searchLayout->addStretch();
    searchLayout->addWidget(addButton);
    searchLayout->addWidget(editButton);
    searchLayout->addWidget(deleteButton);
    searchLayout->addWidget(refreshButton);
    
    mainLayout->addLayout(searchLayout);

    // Statistics Bar
    QHBoxLayout *statsLayout = new QHBoxLayout;
    
    totalProductsLabel = new QLabel("Total Products: 0");
    totalProductsLabel->setStyleSheet("color: #4CAF50; font-size: 16px; font-weight: bold;");
    
    lowStockLabel = new QLabel("Low Stock Items: 0");
    lowStockLabel->setStyleSheet("color: #F44336; font-size: 16px; font-weight: bold;");
    
    statsLayout->addWidget(totalProductsLabel);
    statsLayout->addStretch();
    statsLayout->addWidget(lowStockLabel);
    
    mainLayout->addLayout(statsLayout);

    // Inventory Table
    inventoryTable = new QTableWidget;
    inventoryTable->setColumnCount(6);
    inventoryTable->setHorizontalHeaderLabels({"Name", "Category", "Price", "Quantity", "Min Stock", "Status"});
    inventoryTable->setStyleSheet("QTableWidget { background: #2d313a; border: 2px solid #444; border-radius: 8px; color: white; gridline-color: #444; } QHeaderView::section { background: #3a3f4b; color: white; padding: 8px; border: none; } QTableWidget::item { padding: 8px; }");
    inventoryTable->horizontalHeader()->setStretchLastSection(true);
    inventoryTable->setAlternatingRowColors(true);
    inventoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    mainLayout->addWidget(inventoryTable);

    setupInventoryTable();
    updateLowStockIndicator();
    loadProductsFromDatabase();
}

void InventoryScreen::setupInventoryTable() {
    // Set column widths
    inventoryTable->setColumnWidth(0, 200); // Name
    inventoryTable->setColumnWidth(1, 120); // Category
    inventoryTable->setColumnWidth(2, 100); // Price
    inventoryTable->setColumnWidth(3, 100); // Quantity
    inventoryTable->setColumnWidth(4, 100); // Min Stock
    inventoryTable->setColumnWidth(5, 120); // Status
}

void InventoryScreen::loadSampleData() {
    // This method is no longer used - all data comes from user input
    products.clear();
    refreshInventory();
}

void InventoryScreen::setUsername(const QString& uname) {
    username = uname;
}

void InventoryScreen::addProduct() {
    AddProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Product newProduct = dialog.getProduct();
        if (newProduct.name.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Product name cannot be empty!");
            return;
        }
        // Check if product already exists in DB
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM products WHERE name = ?");
        checkQuery.addBindValue(newProduct.name);
        if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() > 0) {
            QMessageBox::warning(this, "Warning", "A product with this name already exists!");
            return;
        }
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO products (name, category, price, quantity, min_stock, description) VALUES (?, ?, ?, ?, ?, ?)");
        insertQuery.addBindValue(newProduct.name);
        insertQuery.addBindValue(newProduct.category);
        insertQuery.addBindValue(newProduct.price);
        insertQuery.addBindValue(newProduct.quantity);
        insertQuery.addBindValue(newProduct.minStock);
        insertQuery.addBindValue(newProduct.description);
        if (!insertQuery.exec()) {
            QMessageBox::critical(this, "Error", "Failed to add product: " + insertQuery.lastError().text());
            return;
        }
        loadProductsFromDatabase();
        emit inventoryChanged();
        QMessageBox::information(this, "Success", "Product added successfully!");
        // Log add
        ReportsScreen::logActivity(username, "Add Product", newProduct.name);
    }
}

void InventoryScreen::editProduct() {
    int currentRow = inventoryTable->currentRow();
    if (currentRow < 0 || currentRow >= products.size()) {
        QMessageBox::warning(this, "Warning", "Please select a product to edit!");
        return;
    }
    Product &prod = products[currentRow];
    bool ok;
    int newQuantity = QInputDialog::getInt(this, "Edit Quantity", "Enter new quantity:", prod.quantity, 0, 9999, 1, &ok);
    if (ok) {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE products SET quantity = ? WHERE id = ?");
        updateQuery.addBindValue(newQuantity);
        updateQuery.addBindValue(prod.id);
        if (!updateQuery.exec()) {
            QMessageBox::critical(this, "Error", "Failed to update product: " + updateQuery.lastError().text());
            return;
        }
        loadProductsFromDatabase();
        emit inventoryChanged();
        QMessageBox::information(this, "Success", "Product updated successfully!");
        // Log edit
        ReportsScreen::logActivity(username, "Edit Product", prod.name + ", Qty: " + QString::number(newQuantity));
    }
}

void InventoryScreen::deleteProduct() {
    int currentRow = inventoryTable->currentRow();
    if (currentRow < 0 || currentRow >= products.size()) {
        QMessageBox::warning(this, "Warning", "Please select a product to delete!");
        return;
    }
    Product prod = products[currentRow];
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", QString("Are you sure you want to delete '%1'?").arg(prod.name), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery delQuery;
        delQuery.prepare("DELETE FROM products WHERE id = ?");
        delQuery.addBindValue(prod.id);
        if (!delQuery.exec()) {
            QMessageBox::critical(this, "Error", "Failed to delete product: " + delQuery.lastError().text());
            return;
        }
        loadProductsFromDatabase();
        emit inventoryChanged();
        QMessageBox::information(this, "Success", "Product deleted successfully!");
        // Log delete
        ReportsScreen::logActivity(username, "Delete Product", prod.name);
    }
}

void InventoryScreen::refreshInventory() {
    inventoryTable->setRowCount(products.size());
    for (int i = 0; i < products.size(); ++i) {
        const Product &product = products[i];
        inventoryTable->setItem(i, 0, new QTableWidgetItem(product.name));
        inventoryTable->setItem(i, 1, new QTableWidgetItem(product.category));
        inventoryTable->setItem(i, 2, new QTableWidgetItem(QString("$%1").arg(product.price, 0, 'f', 2)));
        inventoryTable->setItem(i, 3, new QTableWidgetItem(QString::number(product.quantity)));
        inventoryTable->setItem(i, 4, new QTableWidgetItem(QString::number(product.minStock)));
        QString status;
        QString statusColor;
        QColor rowColor;
        if (product.quantity == 0) {
            status = "Out of Stock";
            statusColor = "#F44336";
            rowColor = QColor("#FFCDD2"); // light red
        } else if (product.quantity <= product.minStock) {
            status = "Low Stock";
            statusColor = "#FF9800";
            rowColor = QColor("#FFF9C4"); // light yellow
        } else {
            status = "In Stock";
            statusColor = "#4CAF50";
            rowColor = QColor(); // default
        }
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        statusItem->setForeground(QColor(statusColor));
        inventoryTable->setItem(i, 5, statusItem);
        // Highlight the row if low/out of stock
        if (rowColor.isValid()) {
            for (int col = 0; col < inventoryTable->columnCount(); ++col) {
                QTableWidgetItem *item = inventoryTable->item(i, col);
                if (item) item->setBackground(rowColor);
            }
        }
    }
    totalProductsLabel->setText(QString("Total Products: %1").arg(products.size()));
    updateLowStockIndicator();
}

void InventoryScreen::updateLowStockIndicator() {
    int lowStockCount = 0;
    for (const Product &product : products) {
        if (product.quantity <= product.minStock) {
            lowStockCount++;
        }
    }
    lowStockLabel->setText(QString("Low Stock Items: %1").arg(lowStockCount));
}

void InventoryScreen::searchProducts() {
    QString searchText = searchBox->text().toLower();
    
    for (int i = 0; i < inventoryTable->rowCount(); ++i) {
        bool visible = false;
        for (int j = 0; j < inventoryTable->columnCount(); ++j) {
            QTableWidgetItem *item = inventoryTable->item(i, j);
            if (item && item->text().toLower().contains(searchText)) {
                visible = true;
                break;
            }
        }
        inventoryTable->setRowHidden(i, !visible);
    }
} 