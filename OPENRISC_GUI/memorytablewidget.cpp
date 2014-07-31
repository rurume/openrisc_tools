#include "memorytablewidget.h"

MemoryTableWidget::MemoryTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);
    setColumnCount(5);
    setRowCount(2);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setHeader();
}


void MemoryTableWidget::setItem(int row, int column, QTableWidgetItem *item)
{
    QTableWidget::setItem(row, column, item);
}

void MemoryTableWidget::setItem(int row, int column, QString str, int textAlignment)
{
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setTextAlignment(textAlignment);
    item->setText(str);

    setItem(row, column, item);
}

void MemoryTableWidget::setRowBackgroundColor(int step, int row, Qt::GlobalColor color) {
    if(step == 0) {
        item(row, 0)->setBackgroundColor(color);
        item(row, 1)->setBackgroundColor(color);
    } else {
        item(row, 2)->setBackgroundColor(color);
        item(row, 3)->setBackgroundColor(color);
        item(row, 4)->setBackgroundColor(color);
    }
}

void MemoryTableWidget::setRowTextColor(int row, Qt::GlobalColor color) {
    for(int i = 0; i < 5; i++) {
        item(row, i)->setTextColor(color);
    }
}

void MemoryTableWidget::setHeader()
{
    setItem(0, 0, "Before", Qt::AlignCenter);
    setSpan(0, 0, 1, 2);
    setColumnWidth(2, 10);
    setItem(0, 3, "After", Qt::AlignCenter);
    setSpan(0, 3, 1, 2);

    setItem(1, 0, "Address", Qt::AlignCenter);
    setItem(1, 1, "Value", Qt::AlignCenter);
    setItem(1, 3, "Address", Qt::AlignCenter);
    setItem(1, 4, "Value", Qt::AlignCenter);
}

bool MemoryTableWidget::compareContent(int row) {
    QString str1 = item(row, 1)->text();
    QString str2 = item(row, 4)->text();

    if(QString::compare(str1, str2, Qt::CaseInsensitive) == 0) return true;
    else return false;
}
