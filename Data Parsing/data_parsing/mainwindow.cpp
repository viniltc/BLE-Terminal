#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Data File"), "", tr("Text Files (*.txt);;All Files (*)"));

    std::optional<WalkingData> parsedData = parser.parse(fileName);

    if (parsedData) {
        walkingData = *parsedData;  // Extract the value from std::optional
        displayParsedData();        // Now it's safe to use walkingData
    } else {
        // Handle the error case, e.g., show a message box
        QMessageBox::warning(this, "Error", "Failed to parse walking data.");
    }
}

void MainWindow::displayParsedData()
{
    // For example, display some of the parsed data in the console or in a UI element
       qDebug() << "Start Date:" << walkingData.startDate.toString();
       qDebug() << "End Date:" << walkingData.endDate.toString();
       qDebug() << "Total Days:" << walkingData.totalDays;

       // You can also display the parsed data in a QTextEdit or QLabel in your UI
       QString displayText = QString("Start Date: %1\nEnd Date: %2\nTotal Days: %3\nCH1 Total Steps: %4\n")
                               .arg(walkingData.startDate.toString())
                               .arg(walkingData.endDate.toString())
                               .arg(walkingData.totalDays)
                               .arg(walkingData.ch1TotalSteps);

       // Assuming you have a QTextEdit widget named dataDisplayTextEdit in your UI
       ui->dataDisplayTextEdit->setPlainText(displayText);
}

