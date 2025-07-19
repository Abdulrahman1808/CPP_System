cmake_minimum_required(VERSION 3.16)
project(POSApp)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Widgets Sql PrintSupport HttpServer REQUIRED)

# Enable Qt's MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add source files
set(SOURCES
    main.cpp
    MainWindow.cpp
    DashboardScreen.cpp
    SalesScreen.cpp
    InventoryScreen.cpp
    ReportsScreen.cpp
    LoginDialog.cpp
    BillDialog.cpp
    HttpServer.cpp
)

set(HEADERS
    MainWindow.h
    DashboardScreen.h
    SalesScreen.h
    InventoryScreen.h
    ReportsScreen.h
    LoginDialog.h
    BillDialog.h
    Bill.h
    HttpServer.h
)

# Create executable
add_executable(POSApp ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(POSApp Qt6::Widgets Qt6::Sql Qt6::PrintSupport Qt6::HttpServer)