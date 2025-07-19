#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QFrame>
#include <QTabWidget>

struct SalesReport {
    QString date;
    double totalSales;
    int totalOrders;
    double averageOrderValue;
    QString topProduct;
    int topProductQuantity;
};

struct InventoryReport {
    QString category;
    int totalItems;
    int lowStockItems;
    double totalValue;
    QString mostExpensiveItem;
};

class ReportsScreen : public QWidget {
    Q_OBJECT
public:
    explicit ReportsScreen(QWidget *parent = nullptr);
    void setUserRole(const QString& role);
    void setUsername(const QString& username); // Set current user
    static void logActivity(const QString& username, const QString& action, const QString& details);

private slots:
    void generateSalesReport();
    void generateInventoryReport();
    void exportReport();
    void printReport();
    void refreshReports();
    void backupDatabase(); // Slot for backup button
    void refreshActivityLog(); // Slot for activity log tab

private:
    void setupSalesReport();
    void setupInventoryReport();
    void setupSummaryCards();
    void loadSampleData();
    void updateCashierFilter();
    
    QTabWidget *tabWidget;
    
    // Sales Report Components
    QTableWidget *salesTable;
    QComboBox *salesPeriodCombo;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QComboBox *cashierCombo;
    QPushButton *generateSalesBtn;
    QPushButton *exportSalesBtn;
    QPushButton *printSalesBtn;
    
    // Inventory Report Components
    QTableWidget *inventoryTable;
    QComboBox *inventoryCategoryCombo;
    QPushButton *generateInventoryBtn;
    QPushButton *exportInventoryBtn;
    QPushButton *printInventoryBtn;
    
    // Summary Cards
    QLabel *totalRevenueLabel;
    QLabel *totalOrdersLabel;
    QLabel *averageOrderLabel;
    QLabel *topProductLabel;
    QLabel *lowStockCountLabel;
    QLabel *totalInventoryValueLabel;
    
    QList<SalesReport> salesData;
    QList<InventoryReport> inventoryData;
    QString userRole;
    QString username; // Current user
    QPushButton *backupBtn; // Backup button
    QTableWidget *activityLogTable; // Activity log table
}; 