#pragma once
#include "QvPluginProcessor.hpp"
#include "interface/utils/HttpProxy.hpp"

#include <QProcess>

class HysteriaKernel : public Qv2rayPlugin::PluginKernel
{
  public:
    explicit HysteriaKernel();
    bool StartKernel() override;
    bool StopKernel() override;
    void SetConnectionSettings(const QMap<Qv2rayPlugin::KernelOptionFlags, QVariant> &options, const QJsonObject &settings) override;
    QString GetKernelName() const override
    {
        return "Naive";
    }

  private:
    QString protocol;
    QString host;
    QString username;
    QString password;
    QString listenIp;
    bool padding;
    int port;
    QProcess process;
    //
    QString sni;

  private:
    bool isStarted = false;
    int socksPort = 0;
    int httpPort = 0;
    Qv2rayPlugin::Utils::HttpProxy httpProxy;
};

class NaiveKernelInterface : public Qv2rayPlugin::PluginKernelInterface
{
  public:
    virtual std::unique_ptr<Qv2rayPlugin::PluginKernel> CreateKernel() const override
    {
        return std::make_unique<HysteriaKernel>();
    }
    virtual QList<QString> GetKernelProtocols() const override
    {
        return { "naive" };
    }
};
