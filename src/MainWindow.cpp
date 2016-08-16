#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "OSGWidget.h"

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/Node>

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QtWidgets>

#include "SchemaValidator.h"
#include "UIHelper.h"

MainWindow::MainWindow( QWidget* parent)
    : QMainWindow( parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showMaximized();

    //register for keyevents
    this->setFocusPolicy( Qt::StrongFocus );
    this->setMouseTracking(true);

    osgDB::Registry::instance()->addFileExtensionAlias("gml", "citygml");

    UIHelperSingleton* uIHelperSingleton = UIHelperSingleton::getInstance();
    std::vector<QString> vector= uIHelperSingleton->getLabels();
    for(int i=0;i<vector.size();i++)
    {
        ui->elementCB->addItem(vector.at(i));

        ui->elementSelectCB->addItem(vector.at(i));
    }

    //TEST TBR
    //SchemaValidator schemaV;
    //schemaV.validate();
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
                QDir::homePath(),
                tr("3D Mesh (*.obj *.osg *.ive)") );
    if( !filename.isEmpty() )
    {
        onCreateView(filename);
    }
}

void MainWindow::save(){
    QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Save City Object"),
                QDir::homePath(),
                tr("CityGML (*.gml)") );
    if( !filename.isEmpty() )
    {
        if(!filename.endsWith(".gml"))
        {
            filename.append(".gml");
        }

        ui->osgWidget->saveObject2File(filename);
    }
}

void MainWindow::on_commandLinkButton_clicked()
{
    open();
}

void MainWindow::on_commandLinkButton_2_clicked()
{
     save();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    ui->osgWidget->resizeWidget(this->width(),this->height());
}

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if( event->key() == Qt::Key_G )
    {
        bool visible = ui->main_widget->isVisible();
        ui->main_widget->setVisible(!visible);
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

void MainWindow::on_normalDistanceSB_valueChanged(double value)
{
    ui->osgWidget->setNormalsDistance(value);
}

void MainWindow::on_flatShadingRb_toggled(bool checked)
{
    if(checked)
        this->ui->osgWidget->renderOriginal();
    else
        this->ui->osgWidget->renderEditable();
}

void MainWindow::on_AddBtn_clicked()
{
    QString selected = ui->elementCB->itemText(ui->elementCB->currentIndex());
    QStringList selectedList = selected.split(":");
    QString name_space = selectedList.at(0);
    QString element_name = selectedList.at(1);
    ui->osgWidget->tagSelectedItems(name_space,element_name);
}

void MainWindow::on_pushButton_toggled(bool checked)
{
    ui->osgWidget->selectMode = checked;
}

void MainWindow::on_elemSelBtn_clicked()
{
    QString selected = ui->elementSelectCB->itemText(ui->elementSelectCB->currentIndex());
    QStringList selectedList = selected.split(":");
    QString name_space = selectedList.at(0);
    QString element_name = selectedList.at(1);
    ui->osgWidget->selectElementItems(name_space,element_name);
}
