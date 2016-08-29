#ifndef MainWindow_h__
#define MainWindow_h__

#include <QWidget>
#include <QSlider>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include "UIHelper.h"

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
  virtual void resizeEvent(QResizeEvent* event);
  virtual void keyPressEvent( QKeyEvent* event );
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  void showMenu();

private slots:
  void open();
  void save();
  void on_normalDistanceSB_valueChanged(double arg1);
  void on_originalRenderRb_toggled(bool checked);
  void on_AddBtn_clicked();
  void on_elemSelBtn_clicked();
  void on_open_btn_clicked();
  void on_save_btn_clicked();
  void on_selectBtn_toggled(bool checked);
  void on_clearSelBtn_clicked();
  void on_selectAllBtn_clicked();
  void on_showMenuCB_toggled(bool checked);
  void on_normalsBtn_toggled(bool checked);
  void on_normalGylphCB_toggled(bool checked);
  void on_spatialBtn_toggled(bool checked);
  void on_regenerate_graph_btn_clicked();
};

#endif
