#pragma once
#include "QvPluginProcessor.hpp"
#include "interface/utils/HttpProxy.hpp"

#include <QProcess>
#include <QTemporaryFile>

class HysteriaKernel : public Qv2rayPlugin::PluginKernel
{
  public:
    explicit HysteriaKernel();
    bool StartKernel() override;
    bool StopKernel() override;
    void SetConnectionSettings(const QMap<Qv2rayPlugin::KernelOptionFlags, QVariant> &options, const QJsonObject &settings) override;
    QString GetKernelName() const override
    {
        return "Hysteria";
    }

  private:
    QJsonObject hysteriaConf;
    QTemporaryFile hysteriaConfJson;

    QString listenIp;
    int port;
    QProcess process;
    //
    QString sni; // peer

  private:
    bool isStarted = false;
    int socksPort = 0;
    int httpPort = 0;
    Qv2rayPlugin::Utils::HttpProxy httpProxy;
};

class HysteriaKernelInterface : public Qv2rayPlugin::PluginKernelInterface
{
  public:
    virtual std::unique_ptr<Qv2rayPlugin::PluginKernel> CreateKernel() const override
    {
        return std::make_unique<HysteriaKernel>();
    }
    virtual QList<QString> GetKernelProtocols() const override
    {
        return { "hysteria" };
    }
};
