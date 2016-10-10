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

    //osgDB::Registry::instance()->addFileExtensionAlias("gml", "citygml");

    UIHelperSingleton* uIHelperSingleton = UIHelperSingleton::getInstance();
    std::vector<QString> vector= uIHelperSingleton->getLabels();
    for(int i=0;i<vector.size();i++)
    {
        ui->elementCB->addItem(vector.at(i));

        ui->elementSelectCB->addItem(vector.at(i));
    }

   ui->osgWidget->up_vector = new osg::Vec3f(ui->xUpSB->value(),ui->yUpSB->value(),ui->zUpSB->value());
   ui->shapeGrp->setEnabled(false);

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

void MainWindow::resizeEvent(QResizeEvent* event)
{
    ui->osgWidget->resizeWidget(this->width(),this->height());
}

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if( event->key() == Qt::Key_Q )
    {
        ui->originalRenderRb->toggle();
        return;
    }

    if( event->key() == Qt::Key_W )
    {
        ui->editableRenderRb->toggle();
        return;
    }

    if( event->key() == Qt::Key_M )
    {
        ui->showMenuCB->toggle();
        return;
    }

    QCoreApplication::sendEvent(this->ui->osgWidget,event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QCoreApplication::sendEvent(this->ui->osgWidget,event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    QCoreApplication::sendEvent(this->ui->osgWidget,event);
}

void MainWindow::showMenu()
{
    bool visible = ui->main_widget->isVisible();
    ui->main_widget->setVisible(!visible);
}

void MainWindow::on_normalDistanceSB_valueChanged(double value)
{
    ui->osgWidget->setNormalsDistance(value);
}

void MainWindow::on_AddBtn_clicked()
{
    QString selected = ui->elementCB->itemText(ui->elementCB->currentIndex());
    QStringList selectedList = selected.split(":");
    QString name_space = selectedList.at(0);
    QString element_name = selectedList.at(1);
    ui->osgWidget->tagSelectedItems(name_space,element_name);
}

void MainWindow::on_elemSelBtn_clicked()
{
    QString selected = ui->elementSelectCB->itemText(ui->elementSelectCB->currentIndex());
    QStringList selectedList = selected.split(":");
    QString name_space = selectedList.at(0);
    QString element_name = selectedList.at(1);
    ui->osgWidget->selectElementItems(name_space,element_name, false);
}

void MainWindow::on_open_btn_clicked()
{
    open();
}

void MainWindow::on_save_btn_clicked()
{
    save();
}

void MainWindow::on_selectBtn_toggled(bool checked)
{
    ui->osgWidget->selectMode = checked;
    if(checked)
    {
        ui->selectBtn->setText("Select Mode");
    }
    else
    {
        ui->selectBtn->setText("Deselect Mode");
    }
}

void MainWindow::on_clearSelBtn_clicked()
{
    ui->osgWidget->clearAllSelection();
}

void MainWindow::on_selectAllBtn_clicked()
{
    ui->osgWidget->selectAllPolygons();
}

void MainWindow::on_showMenuCB_toggled(bool checked)
{
    showMenu();
}

void MainWindow::on_normalsBtn_toggled(bool checked)
{
    ui->osgWidget->setNormalsBasedSegmentation(checked);
}

void MainWindow::on_normalGylphCB_toggled(bool checked)
{
    ui->osgWidget->showNormalGlyph = checked;

    ui->osgWidget->renderModel();
    ui->osgWidget->repaint();
}

void MainWindow::on_originalRenderRb_toggled(bool checked)
{
    if(checked)
        this->ui->osgWidget->renderEditableMode = false;
    else
        this->ui->osgWidget->renderEditableMode = true;

     ui->osgWidget->renderModel();
     ui->osgWidget->repaint();
}

void MainWindow::on_spatialBtn_toggled(bool checked)
{
    ui->osgWidget->setLocationBasedSegmentation(checked);
}

void MainWindow::on_regenerate_graph_btn_clicked()
{
    ui->osgWidget->generateMoreGraphLinks();
}

void MainWindow::on_shapeBtn_toggled(bool checked)
{
    ui->osgWidget->setShapeBasedSegmentation(checked);
    ui->shapeGrp->setEnabled(checked);
}

void MainWindow::on_plainRb_toggled(bool checked)
{
    if(checked)
        ui->osgWidget->shape_to_segment = Shape::Plain;
}

void MainWindow::on_curvedSurfaceRb_toggled(bool checked)
{
    if(checked)
        ui->osgWidget->shape_to_segment = Shape::Curved;
}

void MainWindow::on_cylindricalRb_toggled(bool checked)
{
    if(checked)
        ui->osgWidget->shape_to_segment = Shape::Cylindrical;
}

void MainWindow::on_boxRB_toggled(bool checked)
{
    if(checked)
        ui->osgWidget->shape_to_segment = Shape::Box;
}

void MainWindow::on_wireFrameModeBtn_toggled(bool checked)
{
    ui->osgWidget->renderWireFrame = checked;

    ui->osgWidget->renderModel();
    ui->osgWidget->repaint();
}

void MainWindow::on_link_vert_dist_sb_valueChanged(double arg1)
{
    ui->osgWidget->link_precision = arg1;
}

void MainWindow::saveOSGT(){
    QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Save City Object"),
                QDir::homePath(),
                tr("OSG (*.osg)") );
    if( !filename.isEmpty() )
    {
        if(!filename.endsWith(".osg"))
        {
            filename.append(".osg");
        }

        ui->osgWidget->saveOSG2File(filename);
    }
}

void MainWindow::on_save_osgt_btn_clicked()
{
    saveOSGT();
}

void MainWindow::on_radioButton_toggled(bool checked)
{
    if(checked)
        ui->paintToolBtn->setChecked(!checked);

    ui->osgWidget->setCameraTranslateMode(checked);
}

void MainWindow::on_recalUpBtn_clicked()
{
    ui->osgWidget->recalculateUpVector();
    ui->xUpSB->setValue(ui->osgWidget->up_vector->x());
    ui->yUpSB->setValue(ui->osgWidget->up_vector->y());
    ui->zUpSB->setValue(ui->osgWidget->up_vector->z());
}

void MainWindow::on_paintToolBtn_toggled(bool checked)
{
    ui->osgWidget->paintSelectionMode = checked;
}

void MainWindow::on_elemSelBtn_2_clicked()
{
    QString selected = ui->elementSelectCB->itemText(ui->elementSelectCB->currentIndex());
    QStringList selectedList = selected.split(":");
    QString name_space = selectedList.at(0);
    QString element_name = selectedList.at(1);
    ui->osgWidget->selectElementItems(name_space,element_name, true);
}
