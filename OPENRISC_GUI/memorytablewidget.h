#ifndef MEMORYTABLEWIDGET_H
#define MEMORYTABLEWIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>

class MemoryTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MemoryTableWidget(QWidget *parent = 0);
    
    void setItem(int row, int column, QTableWidgetItem *item);
    void setItem(int row, int column, QString str, int textAlignment);
    void setRowBackgroundColor(int step, int row, Qt::GlobalColor color);
    void setRowTextColor(int row, Qt::GlobalColor color);
    void setHeader();

    bool compareContent(int row);
signals:
    
public slots:
    
};

#endif // MEMORYTABLEWIDGET_H
