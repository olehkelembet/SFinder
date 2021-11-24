#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QWidgetAction>
#include <iostream>

#ifdef Q_OS_LINUX
  #include <unistd.h>
#endif
#ifdef Q_OS_WINDOWS
  #include <fileapi.h>
  #include <windows.h>
#endif


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void open_file();
    void open_file(const QString& file);
    void save_to_file();
    void exit();
    void see_only_ascii();
    void see_only_unicode();

private:
    Ui::MainWindow *ui;
    QString m_last_path;
    QString m_file_path;
    QMenu* m_file_menu;
    QMenu* m_options_menu;
    QAction* m_editable_act;
/*    QAction* m_ascii_act;
    QAction* m_unicode_act;
*/
    QAction* m_min_string_length;
    QByteArray m_file_byte_array;
    QMap<uint32_t, QString> m_astrings;
    QMap<uint32_t, QString> m_ustrings;
    int m_min_string_len;
    int m_max_string_len;
    enum m_string_type {STYPE_ASCII=0, STYPE_UNICODE};
    bool m_first_run=true;

    void setup_menu_bar();
    void read_file(const QString&);
    void search_strings();
    void find_ascii_strings();
    void find_unicode_strings();
    void print_string(int file_offset, m_string_type type, const QString& str);
    void reopen_file(const QString& file);
    void load_settings();
    void setup_header();
    void setup_options_panel();
    bool file_editable();
};
#endif // MAINWINDOW_H
