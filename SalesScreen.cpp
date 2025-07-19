#include "SalesScreen.h"
#include "BillDialog.h"
#include "Bill.h"
#include "ReportsScreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

SalesScreen::SalesScreen(QWidget *parent) : QWidget(parent), subtotal(0), tax(0), finalTotal(0) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Left side - Product selection
    QVBoxLayout *leftLayout = new QVBoxLayout;
    
    // Search section
    QLabel *searchLabel = new QLabel("Search Products:");
    searchLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
    searchBox = new QLineEdit;
    searchBox->setPlaceholderText("Enter product name...");
    searchBox->setStyleSheet("QLineEdit { padding: 10px; border: 2px solid #444; border-radius: 8px; background: #2d313a; color: white; font-size: 14px; } QLineEdit:focus { border-color: #2196F3; }");
    connect(searchBox, &QLineEdit::textChanged, this, &SalesScreen::searchProducts);
    
    leftLayout->addWidget(searchLabel);
    leftLayout->addWidget(searchBox);

    // Product list
    QLabel *productsLabel = new QLabel("Available Products:");
    productsLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold; margin-top: 10px;");
    productList = new QListWidget;
    productList->setStyleSheet("QListWidget { background: #2d313a; border: 2px solid #444; border-radius: 8px; color: white; font-size: 14px; } QListWidget::item { padding: 10px; border-bottom: 1px solid #444; } QListWidget::item:selected { background: #2196F3; }");
    productList->setMinimumHeight(300);
    
    // Add a message when no products are available
    QListWidgetItem *noProductsItem = new QListWidgetItem("No products available. Add products in the Inventory section.");
    noProductsItem->setTextAlignment(Qt::AlignCenter);
    noProductsItem->setForeground(QColor("#888888"));
    productList->addItem(noProductsItem);
    
    leftLayout->addWidget(productsLabel);
    leftLayout->addWidget(productList);

    // Quantity and add button
    QHBoxLayout *quantityLayout = new QHBoxLayout;
    QLabel *quantityLabel = new QLabel("Quantity:");
    quantityLabel->setStyleSheet("color: #ffffff; font-size: 14px;");
    quantitySpinBox = new QSpinBox;
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(99);
    quantitySpinBox->setValue(1);
    quantitySpinBox->setStyleSheet("QSpinBox { padding: 8px; border: 2px solid #444; border-radius: 6px; background: #2d313a; color: white; font-size: 14px; }");
    
    addButton = new QPushButton("Add to Cart");
    addButton->setStyleSheet("QPushButton { background: #4CAF50; color: white; border: none; border-radius: 8px; padding: 10px; font-size: 14px; font-weight: bold; } QPushButton:hover { background: #45a049; }");
    connect(addButton, &QPushButton::clicked, this, &SalesScreen::addToCart);
    
    quantityLayout->addWidget(quantityLabel);
    quantityLayout->addWidget(quantitySpinBox);
    quantityLayout->addWidget(addButton);
    quantityLayout->addStretch();
    
    leftLayout->addLayout(quantityLayout);
    leftLayout->addStretch();

    // Right side - Cart and payment
    QVBoxLayout *rightLayout = new QVBoxLayout;
    
    // Cart section
    QLabel *cartLabel = new QLabel("Shopping Cart:");
    cartLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
    cartTable = new QTableWidget;
    cartTable->setColumnCount(4);
    cartTable->setHorizontalHeaderLabels({"Item", "Price", "Qty", "Total"});
    cartTable->setStyleSheet("QTableWidget { background: #2d313a; border: 2px solid #444; border-radius: 8px; color: white; gridline-color: #444; } QHeaderView::section { background: #3a3f4b; color: white; padding: 8px; border: none; }");
    cartTable->horizontalHeader()->setStretchLastSection(true);
    cartTable->setMinimumHeight(250);
    
    rightLayout->addWidget(cartLabel);
    rightLayout->addWidget(cartTable);

    // Cart actions
    QHBoxLayout *cartActionsLayout = new QHBoxLayout;
    removeButton = new QPushButton("Remove Item");
    removeButton->setStyleSheet("QPushButton { background: #F44336; color: white; border: none; border-radius: 8px; padding: 8px; font-size: 12px; } QPushButton:hover { background: #d32f2f; }");
    connect(removeButton, &QPushButton::clicked, this, &SalesScreen::removeFromCart);
    
    clearButton = new QPushButton("Clear Cart");
    clearButton->setStyleSheet("QPushButton { background: #FF9800; color: white; border: none; border-radius: 8px; padding: 8px; font-size: 12px; } QPushButton:hover { background: #F57C00; }");
    connect(clearButton, &QPushButton::clicked, this, &SalesScreen::clearCart);
    
    cartActionsLayout->addWidget(removeButton);
    cartActionsLayout->addWidget(clearButton);
    cartActionsLayout->addStretch();
    
    rightLayout->addLayout(cartActionsLayout);

    // Totals section
    QFrame *totalsFrame = new QFrame;
    totalsFrame->setStyleSheet("QFrame { background: #2d313a; border-radius: 10px; padding: 15px; }");
    QVBoxLayout *totalsLayout = new QVBoxLayout(totalsFrame);
    
    QHBoxLayout *subtotalLayout = new QHBoxLayout;
    QLabel *subtotalText = new QLabel("Subtotal:");
    subtotalText->setStyleSheet("color: #888888; font-size: 14px;");
    totalLabel = new QLabel("$0.00");
    totalLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
    subtotalLayout->addWidget(subtotalText);
    subtotalLayout->addStretch();
    subtotalLayout->addWidget(totalLabel);
    
    QHBoxLayout *taxLayout = new QHBoxLayout;
    QLabel *taxText = new QLabel("Tax (8.5%):");
    taxText->setStyleSheet("color: #888888; font-size: 14px;");
    taxLabel = new QLabel("$0.00");
    taxLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
    taxLayout->addWidget(taxText);
    taxLayout->addStretch();
    taxLayout->addWidget(taxLabel);
    
    QHBoxLayout *finalLayout = new QHBoxLayout;
    QLabel *finalText = new QLabel("Total:");
    finalText->setStyleSheet("color: #4CAF50; font-size: 18px; font-weight: bold;");
    finalTotalLabel = new QLabel("$0.00");
    finalTotalLabel->setStyleSheet("color: #4CAF50; font-size: 18px; font-weight: bold;");
    finalLayout->addWidget(finalText);
    finalLayout->addStretch();
    finalLayout->addWidget(finalTotalLabel);
    
    totalsLayout->addLayout(subtotalLayout);
    totalsLayout->addLayout(taxLayout);
    totalsLayout->addLayout(finalLayout);
    
    rightLayout->addWidget(totalsFrame);

    // Payment section
    QLabel *paymentLabel = new QLabel("Payment Method:");
    paymentLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold; margin-top: 10px;");
    paymentMethodCombo = new QComboBox;
    paymentMethodCombo->addItems({"Cash", "Credit Card", "Debit Card", "Mobile Payment"});
    paymentMethodCombo->setStyleSheet("QComboBox { padding: 10px; border: 2px solid #444; border-radius: 8px; background: #2d313a; color: white; font-size: 14px; } QComboBox::drop-down { border: none; } QComboBox::down-arrow { image: none; }");
    
    paymentButton = new QPushButton("Process Payment");
    paymentButton->setMinimumHeight(50);
    paymentButton->setStyleSheet("QPushButton { background: #4CAF50; color: white; border: none; border-radius: 10px; font-size: 16px; font-weight: bold; } QPushButton:hover { background: #45a049; } QPushButton:disabled { background: #666; }");
    connect(paymentButton, &QPushButton::clicked, this, &SalesScreen::processPayment);
    
    rightLayout->addWidget(paymentLabel);
    rightLayout->addWidget(paymentMethodCombo);
    rightLayout->addWidget(paymentButton);
    rightLayout->addStretch();

    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addLayout(rightLayout, 1);

    updateTotal();
    reloadProductsFromDatabase(); // Load products on startup
}

void SalesScreen::setupProductList() {
    // This method will be called when products are added from inventory
    // For now, the product list starts empty
}

void SalesScreen::addToCart() {
    if (productList->currentItem() == nullptr) {
        QMessageBox::warning(this, "Warning", "Please select a product first.");
        return;
    }

    QString itemText = productList->currentItem()->text();
    
    // Check if this is the "no products" message
    if (itemText.contains("No products available")) {
        QMessageBox::information(this, "No Products", "Please add products in the Inventory section first.");
        return;
    }
    
    // Check for low stock warning
    if (productList->currentItem()->foreground().color() == QColor("#FFD600")) {
        QMessageBox::warning(this, "Low Stock", "Warning: This product is low in stock!");
    }

    QStringList parts = itemText.split(" - $");
    if (parts.size() != 2) return;

    QString name = parts[0];
    double price = parts[1].toDouble();
    int quantity = quantitySpinBox->value();

    // Check if item already in cart
    for (int i = 0; i < cartItems.size(); ++i) {
        if (cartItems[i].name == name) {
            cartItems[i].quantity += quantity;
            cartItems[i].total = cartItems[i].price * cartItems[i].quantity;
            updateCartTable();
            updateTotal();
            return;
        }
    }

    // Add new item
    CartItem item;
    item.name = name;
    item.price = price;
    item.quantity = quantity;
    item.total = price * quantity;
    cartItems.append(item);

    updateCartTable();
    updateTotal();
}

void SalesScreen::removeFromCart() {
    int currentRow = cartTable->currentRow();
    if (currentRow >= 0 && currentRow < cartItems.size()) {
        cartItems.removeAt(currentRow);
        updateCartTable();
        updateTotal();
    }
}

void SalesScreen::clearCart() {
    cartItems.clear();
    updateCartTable();
    updateTotal();
}

void SalesScreen::updateCartTable() {
    cartTable->setRowCount(cartItems.size());
    
    for (int i = 0; i < cartItems.size(); ++i) {
        cartTable->setItem(i, 0, new QTableWidgetItem(cartItems[i].name));
        cartTable->setItem(i, 1, new QTableWidgetItem(QString("$%1").arg(cartItems[i].price, 0, 'f', 2)));
        cartTable->setItem(i, 2, new QTableWidgetItem(QString::number(cartItems[i].quantity)));
        cartTable->setItem(i, 3, new QTableWidgetItem(QString("$%1").arg(cartItems[i].total, 0, 'f', 2)));
    }
}

void SalesScreen::updateTotal() {
    subtotal = 0;
    for (const CartItem &item : cartItems) {
        subtotal += item.total;
    }
    
    tax = subtotal * 0.085; // 8.5% tax
    finalTotal = subtotal + tax;
    
    totalLabel->setText(QString("$%1").arg(subtotal, 0, 'f', 2));
    taxLabel->setText(QString("$%1").arg(tax, 0, 'f', 2));
    finalTotalLabel->setText(QString("$%1").arg(finalTotal, 0, 'f', 2));
    
    paymentButton->setEnabled(cartItems.size() > 0);
}

void SalesScreen::processPayment() {
    if (cartItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Cart is empty!");
        return;
    }
    // Gather payment info
    QString paymentMethod = paymentMethodCombo->currentText();
    double paidAmount = finalTotal; // For now, assume paid in full (can add input dialog later)
    double change = 0.0; // For now, assume no change (can add cash input later)
    // --- Save Sale to DB ---
    QSqlQuery saleQuery;
    saleQuery.prepare("INSERT INTO sales (cashier, sale_time, total, payment_method) VALUES (?, ?, ?, ?) RETURNING id");
    saleQuery.addBindValue(username);
    saleQuery.addBindValue(QDateTime::currentDateTime());
    saleQuery.addBindValue(finalTotal);
    saleQuery.addBindValue(paymentMethod);
    if (!saleQuery.exec() || !saleQuery.next()) {
        QMessageBox::critical(this, "Error", "Failed to save sale: " + saleQuery.lastError().text());
        return;
    }
    int saleId = saleQuery.value(0).toInt();
    // Save each item
    QSqlQuery itemQuery;
    for (const auto& item : cartItems) {
        itemQuery.prepare("INSERT INTO sales_items (sale_id, product_name, quantity, price) VALUES (?, ?, ?, ?)");
        itemQuery.addBindValue(saleId);
        itemQuery.addBindValue(item.name);
        itemQuery.addBindValue(item.quantity);
        itemQuery.addBindValue(item.price);
        if (!itemQuery.exec()) {
            QMessageBox::critical(this, "Error", "Failed to save sale item: " + itemQuery.lastError().text());
            // Optionally: rollback or mark sale as incomplete
        }
    }
    // --- Create Bill from real cart data ---
    Bill bill;
    bill.saleId = saleId;
    bill.cashier = username;
    bill.dateTime = QDateTime::currentDateTime();
    bill.taxRate = 0.085;
    bill.paid = paidAmount;
    bill.change = change;
    for (const auto& item : cartItems) {
        BillItem bitem;
        bitem.name = item.name;
        bitem.quantity = item.quantity;
        bitem.price = item.price;
        bill.items.append(bitem);
    }
    // Log sale
    QString details = QString("Total: $%1, Items: %2, Payment: %3, SaleID: %4").arg(finalTotal, 0, 'f', 2).arg(cartItems.size()).arg(paymentMethod).arg(saleId);
    ReportsScreen::logActivity(username, "Sale", details);
    // Show BillDialog
    BillDialog dlg(bill, this);
    dlg.exec();
    // Show confirmation and clear cart after dialog
    QMessageBox::information(this, "Payment Complete", QString("Payment processed successfully!\n\nTotal: $%1\nPayment Method: %2\n\nThank you for your purchase!")
        .arg(finalTotal, 0, 'f', 2)
        .arg(paymentMethod));
    clearCart();
}

void SalesScreen::searchProducts() {
    QString searchText = searchBox->text().toLower();
    
    for (int i = 0; i < productList->count(); ++i) {
        QListWidgetItem *item = productList->item(i);
        bool visible = item->text().toLower().contains(searchText);
        item->setHidden(!visible);
    }
}

// Method to add products from inventory
void SalesScreen::addProductToSales(const QString &name, double price, int quantity) {
    // Remove the "no products" message if it exists
    for (int i = 0; i < productList->count(); ++i) {
        QListWidgetItem *item = productList->item(i);
        if (item->text().contains("No products available")) {
            delete productList->takeItem(i);
            break;
        }
    }
    
    // Add the new product
    QString productText = QString("%1 - $%2").arg(name).arg(price, 0, 'f', 2);
    productList->addItem(productText);
} 

void SalesScreen::reloadProductsFromDatabase() {
    productList->clear();
    QSqlQuery query("SELECT name, price, quantity FROM products WHERE quantity > 0");
    int count = 0;
    const int lowStockThreshold = 5;
    while (query.next()) {
        QString name = query.value(0).toString();
        double price = query.value(1).toDouble();
        int quantity = query.value(2).toInt();
        QString productText = QString("%1 - $%2").arg(name).arg(price, 0, 'f', 2);
        QListWidgetItem* item = new QListWidgetItem(productText);
        if (quantity <= lowStockThreshold) {
            item->setForeground(QColor("#FFD600")); // yellow for low stock
            item->setToolTip("Low stock: " + QString::number(quantity));
        }
        productList->addItem(item);
        count++;
    }
    if (count == 0) {
        QListWidgetItem *noProductsItem = new QListWidgetItem("No products available. Add products in the Inventory section.");
        noProductsItem->setTextAlignment(Qt::AlignCenter);
        noProductsItem->setForeground(QColor("#888888"));
        productList->addItem(noProductsItem);
    }
}

void SalesScreen::onInventoryChanged() {
    reloadProductsFromDatabase();
} 

void SalesScreen::setUsername(const QString& uname) {
    username = uname;
} 