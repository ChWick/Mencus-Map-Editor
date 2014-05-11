#include "editexecutabledialog.h"
#include "ui_editexecutabledialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

EditExecutableDialog::EditExecutableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditExecutableDialog)
{
    ui->setupUi(this);

    QSettings settings("CWDevelopment", "MencusMapEditor");
    settings.beginGroup("executable");
    ui->executablePath->setText(settings.value("executable_path").toString());
    ui->workingDirectory->setText(settings.value("working_dir").toString());
    settings.endGroup();
}

EditExecutableDialog::~EditExecutableDialog()
{
    delete ui;
}

void EditExecutableDialog::accept() {
    QDialog::accept();

    QSettings settings("CWDevelopment", "MencusMapEditor");
    settings.beginGroup("executable");
    settings.setValue("executable_path", ui->executablePath->text());
    settings.setValue("working_dir", ui->workingDirectory->text());
    settings.endGroup();
}

void EditExecutableDialog::onSelectWorkingDirectory() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Working directory"));

    ui->workingDirectory->setText(path);

    QDir dir(path);
    if (QFileInfo(dir.filePath("Game")).isFile()) {
        ui->executablePath->setText(dir.filePath("Game"));
    }
    else if (QFileInfo(dir.filePath("bin/Game")).isFile()) {
        ui->executablePath->setText(dir.filePath("bin/Game"));
    }
}

void EditExecutableDialog::onSelectExecutablePath() {
    QString path = QFileDialog::getOpenFileName(this, tr("Executable"), "", tr("*"));

    ui->executablePath->setText(path);
}
