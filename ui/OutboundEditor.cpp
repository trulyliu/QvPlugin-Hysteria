#include <QDir>
#include <QFileDialog>
#include "OutboundEditor.hpp"

OutboundEditor::OutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    textUpMbps->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    textDownMbps->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
}

void OutboundEditor::SetContent(const QJsonObject &r)
{
    root = r;
    const auto protocol = root["protocol"].toString();
    comboProtocol->setCurrentText(protocol != "udp" && protocol != "wechat-video" && protocol != "faketcp" ? "udp" : root["protocol"].toString());
    textAlpn->setText(root["alpn"].toString());
    textAuthStr->setText(root["auth_str"].toString());
    textDownMbps->setText(root["down_mbps"].toString());
    textUpMbps->setText(root["up_mbps"].toString());
    textSNI->setText(root["sni"].toString());
    textObfs->setText(root["obfs"].toString());
    checkInsecure->setChecked(root["insecure"].toBool());
    textCaPath->setText(root["ca"].toString());
}

void OutboundEditor::changeEvent(QEvent *e)
{

    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void OutboundEditor::on_comboProtocol_currentTextChanged(const QString &arg1)
{
    root["protocol"] = arg1;
}

void OutboundEditor::on_textAlpn_textEdited(const QString &arg1)
{
    if (arg1.isEmpty()) {
        root.remove("alpn");
    } else {
        root["alpn"] = arg1;
    }
}

void OutboundEditor::on_textAuthStr_textEdited(const QString &arg1)
{
    if (arg1.isEmpty()) {
        root.remove("auth_str");
    } else {
        root["auth_str"] = arg1;
    }
}

void OutboundEditor::on_checkInsecure_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        root["insecure"] = arg1 == Qt::Checked;
    } else {
        root.remove("insecure");
    }
}

void OutboundEditor::on_textSNI_textEdited(const QString &arg1) {
    if (arg1.isEmpty()) {
        root.remove("sni");
    } else {
        root["sni"] = arg1;
    }
}

void OutboundEditor::on_textUpMbps_textEdited(const QString &arg1) {
    root["up_mbps"] = arg1;
}

void OutboundEditor::on_textDownMbps_textEdited(const QString &arg1) {
    root["down_mbps"] = arg1;
}

void OutboundEditor::on_textObfs_textEdited(const QString &arg1) {
    if (arg1.isEmpty()) {
        root.remove("obfs");
    } else {
        root["obfs"] = arg1;
    }
}

void OutboundEditor::on_textCaPath_textEdited(const QString &arg1)
{
    if (arg1.isEmpty()) {
        root.remove("ca");
    } else {
        root["ca"] = arg1;
    }
}

void OutboundEditor::on_buttonBrowseCa_clicked()
{
    const auto caPath = QFileDialog::getOpenFileName(this, tr("Open custom CA"), QDir::currentPath());

    if (!caPath.isEmpty())
    {
        textCaPath->setText(caPath);
        on_textCaPath_textEdited(caPath);
    }
}
