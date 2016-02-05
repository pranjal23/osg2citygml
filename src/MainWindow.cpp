#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "OSGWidget.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QtWidgets>

MainWindow::MainWindow( QWidget* parent)
    : QMainWindow( parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCreateView(QString fileName)
{
    ui->osgWidget->setFile(fileName);
    ui->osgWidget->resizeWidget(this->width(),this->height());
}

void MainWindow::open(){
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open 3D Mesh"),
                QDir::currentPath(),
                tr("3D Mesh (*.obj *.osg *.ive)") );
    if( !filename.isEmpty() )
    {
        onCreateView(filename);
    }
}

void MainWindow::on_commandLinkButton_clicked()
{
    open();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    ui->osgWidget->resizeWidget(this->width(),this->height());
}
