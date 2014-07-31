#include "newtab.h"
#include "uartdialog.h"

UARTDialog::UARTDialog(int coreID, QWidget *parent) :
    QDialog(parent)
{
    maxFontSize = 96;
    minFontSize = 8;
    currentFontSize = 14;

    console = new QTextEdit;
    console->setReadOnly(true);
    //console->setText("UART test");
    console->setTextColor(Qt::black);
    console->setFont(QFont("Default", currentFontSize));

    fontSizeLabel = new QLabel(tr("Font Size: %1").arg(currentFontSize));

    QWidget *empty = new QWidget;
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setSingleStep(1);
    slider->setFixedWidth(100);
    slider->setRange(minFontSize, maxFontSize);
    slider->setValue(currentFontSize);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(console, 0, 0, 1, 0);
    mainLayout->addWidget(fontSizeLabel, 1, 0);
    mainLayout->addWidget(empty, 1, 2);
    mainLayout->addWidget(slider, 1, 3);
    setLayout(mainLayout);

    QDesktopWidget desktop;
    setFixedSize(desktop.width() * 0.4, desktop.height() * 0.4);
    setWindowTitle(tr("UART - core%1").arg(coreID));

    connectSignalsAndSlots();
}

void UARTDialog::connectSignalsAndSlots() {
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_slider_valueChanged()));
}

void UARTDialog::on_slider_valueChanged() {
    int value = slider->value();

    if(value > currentFontSize) {
        console->zoomIn(value - currentFontSize);
    } else {
        console->zoomOut(currentFontSize - value);
    }
    currentFontSize = value;
    fontSizeLabel->setText(tr("Font Size: %1").arg(currentFontSize));
}

void UARTDialog::setData(const QString &text) {
    console->moveCursor(QTextCursor::End);
    console->insertPlainText(text);
    QCoreApplication::processEvents();
}
