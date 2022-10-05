/*
 * Copyright (C) 2022 The V-Gears Team
 *
 * This file is part of V-Gears
 *
 * V-Gears is free software: you can redistribute it and/or modify it under
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.0 (GPLv3) of the License.
 *
 * V-Gears is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */


#include <iostream>
#include <QtCore/QProcess>
#include <QtWidgets/QFileDialog>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include "DataInstaller.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

/**
 * Indicates if an installer has already been created.
 */
static bool installer_created = false;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), main_window_(new Ui::MainWindow){
    main_window_->setupUi(this);
    // Goto the data tab by default.
    main_window_->tabWidget->setCurrentIndex(0);
    // Turn off the launcher tab for now since it dosen't actually work
    //main_window_->tabWidget->setTabEnabled(0, false);
    InitSettings();
    main_window_->btn_vgears_config->setText(settings_->value("ConfigDir").toString());
    main_window_->line_data_dst->setText(settings_->value("DataDir").toString());
    main_window_->line_vgears_exe->setText(settings_->value("VGearsEXE").toString());
#ifdef _DEBUG
    // Hard coded prebaked paths for debugging to save time
    //main_window_->line_data_src->setText("C:\\Games\\FF7\\data");
    //main_window_->line_data_dst->setText(
    //  "C:\\Users\\paul\\Desktop\\v-gears\\output\\data"
    //);
#endif
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(DoProgress()));
}

MainWindow::~MainWindow(){delete main_window_;}

void MainWindow::InitSettings(void){
    bool win = false;
#ifdef Q_OS_WIN
    settings_ = new QSettings(
      QCoreApplication::applicationDirPath() + "/" + "launcherSettings.ini",
      QSettings::IniFormat
    );
    win = true;
#else
    settings_ = new QSettings(
      QSettings::NativeFormat,QSettings::UserScope, "v-gears", "launcher", 0
    );
#endif
    //Check settings.
    if (settings_->value("ConfigDir").isNull()){
        if (win) settings_->setValue("ConfigDir", QString("%1/v-gears").arg(QDir::homePath()));
        else settings_->setValue("ConfigDir", QString("%1/.v-gears").arg(QDir::homePath()));
    }

    if (settings_->value("DataDir").isNull()){
        if (win) settings_->setValue("DataDir",QString("%1/v-gears/data").arg(QDir::homePath()));
        else settings_->setValue("DataDir",QString("%1/.v-gears/data").arg(QDir::homePath()));
    }

    if (settings_->value("VGearsEXE").isNull()){
        if (win){
            settings_->setValue(
              "VGearsEXE",QString("%1/v-gears.exe").arg(QCoreApplication::applicationDirPath())
            );
        }
        else settings_->setValue("VGearsEXE", QString("/usr/games/v-gears"));
    }
}

void MainWindow::on_line_vgears_config_editingFinished(){
    settings_->setValue("ConfigDir",main_window_->btn_vgears_config->text());
}

void MainWindow::on_btn_vgears_config_clicked(){
    QString temp = QFileDialog::getExistingDirectory(
      this, tr("Select Location of VGears Configuration Data,"),
      settings_->value("ConfigDir").toString()
    );
    if (!temp.isNull()){
        settings_->setValue("ConfigDir", temp);
        main_window_->btn_vgears_config->setText(temp);
    }
}

void MainWindow::on_line_vgears_exe_editingFinished(){
    settings_->setValue("VGearsEXE",main_window_->line_vgears_exe->text());
}

void MainWindow::on_btn_vgears_exe_clicked(){
    QString temp = QFileDialog::getOpenFileName(
      this, tr("Location of VGears Executable,"), QDir::rootPath()
    );
    if (!temp.isNull()){
        settings_->setValue("VGearsEXE", temp);
        main_window_->line_vgears_exe->setText(temp);
    }
}

void MainWindow::on_btn_vgears_run_clicked(){
    QString configDir(main_window_->btn_vgears_config->text());
    QString exe(main_window_->line_vgears_exe->text());
    QStringList args;
    args.append(
      QString("--resources-file=%1/resources.cfg").arg(main_window_->btn_vgears_config->text())
    );
    args.append(
      QString("--config-file=%1/v-gears.cfg").arg(main_window_->btn_vgears_config->text())
    );
    args.append(
      QString("--plugins-file=/%1/plugins.cfg").arg(main_window_->btn_vgears_config->text())
    );
    // Check that the config dir is set up correctly.
    QProcess vGears;
    vGears.startDetached(exe,args,configDir);
}

void MainWindow::on_btn_data_src_clicked(){
    QString temp = QFileDialog::getExistingDirectory(
      this, tr("Location of Game Data),"),QDir::homePath()
    );
    main_window_->line_data_src->setText(temp);
}

void MainWindow::on_line_data_dst_editingFinished(){
    settings_->setValue("DataDir",main_window_->line_data_dst->text());
}

void MainWindow::on_btn_data_dst_clicked(){
    QString temp = QFileDialog::getExistingDirectory(
      this, tr("Location of VGears Data),"), settings_->value("DataDir").toString()
    );
    if (!temp.isNull()){
        settings_->setValue("DataDir",temp);
        main_window_->line_data_dst->setText(temp);
    }
}

void MainWindow::on_btn_data_run_clicked(){
    if (main_window_->line_data_src->text().isEmpty()){
        // TODO IVV: Default path, remove
        main_window_->line_data_src->setText("/home/ivalentin/data/");
    }
    //{
    //    QMessageBox::critical(
    //      this, tr("Input error"),
    //      tr("No input to installed FF7 PC data provided")
    //    );
    //}
    else if (main_window_->line_data_dst->text().isEmpty())
        QMessageBox::critical(this, tr("Output error"), tr("No output path provided"));
    else{
        // Normalize the paths so its in / format separators.
        QString input = QDir::fromNativeSeparators(main_window_->line_data_src->text());
        if (!input.endsWith("/")) input += "/";
        QString output = QDir::fromNativeSeparators(main_window_->line_data_dst->text());
        if (!output.endsWith("/")) output += "/";
        // TODO: Enumerate files or find some better way to do this.
        const std::vector<std::string> required_files = {"field/char.lgp", "field/flevel.lgp"};
        // Ensure required files are in the input dir
        for (auto& file : required_files){
            QString full_path = input + QString::fromStdString(file);
            if (!QFile::exists(full_path)){
                QMessageBox::critical(
                  this, tr("Missing input file"), tr("File not found: ") + full_path);
                return;
            }
        }
        if (installer_created){
            QMessageBox::critical(
              this,
              tr("Error"),
              tr(
                "Due to use of singletons install function can only be used "
                "once, please restart the application."
              )
            );
            return;
        }
        // Start data conversion
        try{
            installer_created = true;
            installer_ = std::make_unique<DataInstaller>(
              QDir::toNativeSeparators(input).toStdString(),
              QDir::toNativeSeparators(output).toStdString(),
              [this](const std::string outputLine){
                main_window_->data_log->append(outputLine.c_str());
              }
            );
            OnInstallStarted();
        }
        catch (const std::exception& ex){OnInstallStopped();}
    }
}

void MainWindow::EnableUi(bool enable){
    main_window_->btn_data_run->setEnabled(enable);
    main_window_->line_data_src->setEnabled(enable);
    main_window_->btn_data_src->setEnabled(enable);
    main_window_->line_data_dst->setEnabled(enable);
    main_window_->btn_data_dst->setEnabled(enable);
    main_window_->data_progress_bar->setValue(0);
    if (!enable) timer_->start(0);
    else timer_->stop();
}

void MainWindow::OnInstallStarted(){EnableUi(false);}

void MainWindow::OnInstallStopped(){EnableUi(true);}

void MainWindow::DoProgress(){
    try{
        const int progress = installer_->Progress();
        main_window_->data_progress_bar->setValue(progress);
        if (progress >= 100) OnInstallStopped();
    }
    catch (const std::exception& ex){
        OnInstallStopped();
        QMessageBox::critical(this, tr("Data conversion exception"), ex.what());
    }
}
