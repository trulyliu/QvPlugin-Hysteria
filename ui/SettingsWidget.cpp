#include "SettingsWidget.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

SettingsWidget::SettingsWidget(QWidget *parent) : Qv2rayPlugin::QvPluginSettingsWidget(parent)
{
    setupUi(this);
}

void SettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void SettingsWidget::on_textKernelPath_textEdited(const QString &arg1)
{
    root.insert("kernelPath", arg1);
}

void SettingsWidget::on_buttonBrowseKernel_clicked()
{
    const auto core = QFileDialog::getOpenFileName(this, tr("Open Hysteria Kernel"), QDir::currentPath());

    if (!core.isEmpty())
    {
        textKernelPath->setText(core);
        on_textKernelPath_textEdited(core);
    }
}

void SettingsWidget::on_buttonTestKernel_clicked()
{
    const auto path = this->textKernelPath->text();
    if (path.isEmpty())
    {
        QMessageBox::warning(this, tr("Invalid Configuration"), tr("Kernel path is empty."));
        return;
    }

    QProcess process;

#ifdef Q_OS_WIN32
    process.setProgram(path);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.setNativeArguments("--version");
    process.start();
#else
    process.start(path, { "--version" });
#endif
    process.waitForStarted();
    process.waitForFinished();
    const auto exitCode = process.exitCode();
    if (exitCode != 0)
    {
        QMessageBox::warning(this, tr("Hysteria Core Test Failed"), tr("Hysteria Core failed with exit code %1").arg(exitCode));
        return;
    }

    QString output = process.readAllStandardOutput();
    QMessageBox::information(this, tr("Hysteria Test Result"), tr("Hysteria: %1").arg(output));
}
