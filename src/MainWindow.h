#ifndef MainWindow_h__
#define MainWindow_h__

#include <QWidget>
#include <QSlider>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow( QWidget* parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  void onCreateView(QString fileName);

private slots:
  void open();

};

#endif
