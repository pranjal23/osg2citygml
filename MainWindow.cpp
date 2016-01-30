#include "MainWindow.h"
#include "OSGWidget.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QtWidgets>

MainWindow::MainWindow( QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags )
    , mdiArea_( new QMdiArea( this ) )
{
    setWindowState(Qt::WindowMaximized);
    QMenuBar* menuBar = this->menuBar();

    QMenu* menu = menuBar->addMenu( "File" );
    menu->addAction( "Select Model", this, SLOT( open() ) );

    this->setCentralWidget( mdiArea_ );
}

MainWindow::~MainWindow()
{
}

void MainWindow::onCreateView(QString fileName)
{
    OSGWidget* osgWidget     = new OSGWidget( this );
    osgWidget->setFile(fileName);
    QMdiSubWindow* subWindow = mdiArea_->addSubWindow( osgWidget );
    subWindow->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    subWindow->showMaximized();
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
