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

    //register for keyevents
    this->setFocusPolicy( Qt::StrongFocus );
    this->setMouseTracking(true);
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

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if( event->key() == Qt::Key_G )
    {
        bool visiblity = this->ui->controlsGroup->isVisible();
        this->ui->controlsGroup->setVisible(!visiblity);
        return;
    }

    QCoreApplication::sendEvent(this->ui->osgWidget,event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    qDebug() << " MouseEvent in main window 1 ";
    QCoreApplication::sendEvent(this->ui->osgWidget,event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    qDebug() << " MouseEvent in main window 2 ";
    QCoreApplication::sendEvent(this->ui->osgWidget,event);
}

void MainWindow::on_enableLightBtn_toggled(bool checked)
{
    ui->osgWidget->setLocationBasedSegmentation(checked);
}

void MainWindow::on_multilightsBtn_toggled(bool checked)
{
    ui->osgWidget->setNormalsBasedSegmentation(checked);
}
