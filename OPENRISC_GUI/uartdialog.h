#ifndef UARTDIALOG_H
#define UARTDIALOG_H

#include "headers.h"

class UARTDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UARTDialog(int CoreID, QWidget *parent = 0);

    void setData(const QString &);

    int currentFontSize;

private:
    void connectSignalsAndSlots();

    QTextEdit *console;
    QLabel *fontSizeLabel;
    QSlider *slider;
    int maxFontSize;
    int minFontSize;
    
signals:
    
public slots:
    void on_slider_valueChanged();
};

#endif // UARTDIALOG_H
