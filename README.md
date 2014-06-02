# Inventory

Application for inventory management.

Database is a collection of items.
Each item has item type, placement location, name and various details like inventorial number, responsible person, repair flag, note etc.
There are three reference tables for item types, places and persons. Each of them is modifiable (including main collection) - user can add/remove/edit records.
Views can be sorted and filtered by item type, placement and 'written off' flag.
History of editions for main table is stored.
Columns in the main view could be shown and hidden.
There is also 'print' view with some summary information.
Any view could be printed to CSV file for further export to Excel/OpenOffice/LibreOffice/etc.

Database is an SQLite file. It could be stored at any location. After first start, user will be prompted to choose that location or to create a new database, if there isn't such.

## Installation

Depends on Qt4 and SQLite driver.

Run `qmake && make` from main directory. Created executable will be placed in `bin/` directory. Fill free to run it or put it somewhere in the $PATH or whatever.
