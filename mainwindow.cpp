#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_menu_bar();
    this->setGeometry(100, 100, 800, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup_menu_bar()
{
    m_file_menu	= new QMenu("File");
    m_options_menu = new QMenu("Options");

    m_file_menu->addAction("Open file",  this, SLOT(open_file()), Qt::CTRL + Qt::Key_O);
    m_file_menu->addAction("Save file",   this, SLOT(save_file()), Qt::CTRL + Qt::Key_S);
    m_file_menu->addAction("Save strings to file",   this, SLOT(save_to_file()), Qt::CTRL + Qt::Key_F);
    m_file_menu->addAction("Exit",   this, SLOT(exit()), Qt::CTRL + Qt::Key_Q);

    m_editable_act = m_options_menu->addAction("Editable",  this, SLOT(file_editable()), Qt::CTRL + Qt::Key_E);
    m_options_menu->addSeparator();
    m_ascii_act = m_options_menu->addAction("ASCII",  this, SLOT(see_only_ascii()), Qt::CTRL + Qt::Key_A);
    m_unicode_act = m_options_menu->addAction("UNICODE",  this, SLOT(see_only_unicode()), Qt::CTRL + Qt::Key_U);
    m_options_menu->addSeparator();
    m_min_string_length = m_options_menu->addAction("Minimum search length", this, SLOT(set_minimum_search_lenght()), Qt::CTRL + Qt::Key_M);
    m_editable_act->setCheckable(true);
    m_editable_act->setChecked(false);
    m_ascii_act->setCheckable(true);
    m_ascii_act->setChecked(true);
    m_unicode_act->setCheckable(true);
    m_unicode_act->setChecked(true);

    ui->menubar->addMenu(m_file_menu);
    ui->menubar->addMenu(m_options_menu);
}

void MainWindow::on_pushButton_clicked()
{
    open_file();
}

void MainWindow::open_file()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file for search"),
                                                m_last_path.isEmpty() ? QDir::homePath() : m_last_path);
    if( !QFile::exists(file_name) ) {
        qDebug() << "Selected file" << file_name << "does not exist.";
        return;
    }
    if(file_name==m_file_path)
    {
        if(first_run)
        {
            first_run = false;
        } else
        {
            reopen_file(file_name);
        }
    } else if(m_file_path.size())
    {
        if(first_run)
        {
           first_run = false;
        }
    }
    m_last_path = QFileInfo(file_name).absolutePath();
    m_file_path = file_name;
    ui->lineEdit->setText(file_name);
    open_file(file_name);
}

void MainWindow::open_file(const QString& file)
{
    if(!first_run)
    {
        //ui->tableWidget->clear();
        QAbstractItemModel* const mdl = ui->tableWidget->model();
        mdl->removeRows(0,mdl->rowCount());
    }
    read_file(file);
    search_strings();
}

void MainWindow::reopen_file(const QString& file)
{
    QMessageBox::information(this,tr("You reopening the file !"),tr("The file will be reopened."));
    open_file(file);
}

void MainWindow::save_file()
{

}

void MainWindow::save_to_file()
{

}

void MainWindow::exit()
{
    QApplication::quit();
}

void MainWindow::file_editable()
{

}

void MainWindow::see_only_ascii()
{

}

void MainWindow::see_only_unicode()
{

}

void MainWindow::read_file(const QString& file)
{
    if(!m_file_byte_array.isNull())
    {
        m_file_byte_array.clear();
    }

    QFile file_io(file);
    if( !file_io.open(QIODevice::ReadOnly) )
    {
        qDebug() << "Can't open file: " << file;
    }
    m_file_byte_array = file_io.readAll();
}

void MainWindow::search_strings()
{
    if(!first_run)
    {
        ui->tableWidget->clear();
    }
    find_ascii_strings();
    find_unicode_strings();
}

void MainWindow::set_minimum_search_lenght()
{

}

void MainWindow::set_maximum_search_lenght()
{

}

void MainWindow::find_ascii_strings()
{
    int row{0};
    bool b_in_string{false};
    QString current_str;
    m_min_string_len = 4;

    for( int file_offset=0, counter=0;
         counter < m_file_byte_array.size();
         ++counter )
    {
        if( std::isprint(m_file_byte_array.at(counter)) )
        {
            if(!b_in_string)
            {
                b_in_string = true;
                file_offset = counter;
            }
            current_str += m_file_byte_array.at(counter);
            continue;
        } else if( current_str.size() >= m_min_string_len )
        {
          m_astrings.insert(file_offset, current_str);
          print_string(file_offset, STYPE_ASCII, current_str);
          b_in_string=false;
          current_str.clear();
        } else
        {
            current_str.clear();
            b_in_string=false;
        }
    }
}

void MainWindow::find_unicode_strings()
{
    int row{0};
    bool b_in_string{false};
    QString current_str;
    m_min_string_len = 4;

    for( int file_offset=0, counter=0;
         counter < (m_file_byte_array.size()-1);
         counter +=2 )
    {
        if( std::isprint(m_file_byte_array.at(counter))  && (m_file_byte_array.at(counter+1) == 0) )
        {
            if(!b_in_string)
            {
                b_in_string = true;
                file_offset = counter;
                current_str += m_file_byte_array.at(counter);
            }
            else
            {
                current_str += m_file_byte_array.at(counter);
                continue;
            }
        }
        else if( current_str.size() >= m_min_string_len )
        {
          m_ustrings.insert(file_offset, current_str);
          print_string(file_offset, STYPE_UNICODE, current_str);
          b_in_string=false;
          current_str.clear();
        }
        else
        {
            current_str.clear();
            b_in_string=false;
        }
    }
}

void MainWindow::print_string(int file_offset, string_type type, const QString& str)
{
    int row = ui->tableWidget->rowCount();

    ui->tableWidget->setRowCount(row + 1);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem((std::to_string(file_offset)).c_str()));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem((type)?"U":"A"));
    ui->tableWidget->setItem(row,2,new QTableWidgetItem(str));

}
