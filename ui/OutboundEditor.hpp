#pragma once

#include "interface/QvGUIPluginInterface.hpp"
#include "interface/QvPluginProcessor.hpp"
#include "ui_OutboundEditor.h"

class OutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::OutboundEditor
{
    Q_OBJECT

  public:
    explicit OutboundEditor(QWidget *parent = nullptr);
    void SetHostAddress(const QString &address, int port) override
    {
        root["host"] = address;
        root["port"] = port;
    }
    QPair<QString, int> GetHostAddress() const override
    {
        return { root["host"].toString(), root["port"].toInt() };
    }
    //
    void SetContent(const QJsonObject &) override;
    const QJsonObject GetContent() const override
    {
        return root;
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_comboProtocol_currentTextChanged(const QString &arg1);
    void on_textAlpn_textEdited(const QString &arg1);
    void on_textAuthStr_textEdited(const QString &arg1);
    void on_textSNI_textEdited(const QString &arg1);
    void on_textUpMbps_textEdited(const QString &arg1);
    void on_textDownMbps_textEdited(const QString &arg1);
    void on_textObfs_textEdited(const QString &arg1);
    void on_checkInsecure_stateChanged(int arg1);
    void on_textCaPath_textEdited(const QString &arg1);
    void on_buttonBrowseCa_clicked();

  private:
    QJsonObject root;
};
