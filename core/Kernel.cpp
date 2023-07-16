#include "Kernel.hpp"

#include "HysteriaPlugin.hpp"
#include "interface/QvPluginInterface.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QTemporaryDir>
#include <QTemporaryFile>

HysteriaKernel::HysteriaKernel() : Qv2rayPlugin::PluginKernel(), hysteriaConfJson(QDir::temp().absoluteFilePath(".qv2ray-hyteria-XXXXXX.json"))
{
    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, &QProcess::readyRead, [this]() { emit this->OnKernelLogAvailable(process.readAll()); });
    connect(&process, &QProcess::stateChanged, [this]() {
        if (this->isStarted && process.state() == QProcess::ProcessState::NotRunning)
        {
            this->isStarted = false;
            StopKernel();
            emit OnKernelCrashed(tr("Hysteria kernel crashed with code %1").arg(process.exitCode()));
        }
    });
}

bool HysteriaKernel::StartKernel()
{
    const auto executablePath = HysteriaPluginInstance->GetSettngs()["kernelPath"].toString();
    if (!QFile::exists(executablePath))
    {
        HysteriaPluginInstance->PluginErrorMessageBox(
            tr("Hysteria!"), tr("We cannot find your Hysteria kernel. Please configure it in the plugin settings."));
        return false;
    }

    QJsonDocument strJson = QJsonDocument(hysteriaConf);
    hysteriaConfJson.setAutoRemove(true);
    hysteriaConfJson.open();
    HysteriaPluginInstance->PluginLog("Hysteria config file " + hysteriaConfJson.fileName());
    hysteriaConfJson.open();
    hysteriaConfJson.write(strJson.toJson());
    hysteriaConfJson.close();

    QStringList arguments{ "client" };
    arguments << "--config";
    arguments << hysteriaConfJson.fileName();
    arguments << "--log-level";
    arguments << "info";

    // listen http
    if (this->httpPort)
    {
        httpProxy.httpListen(QHostAddress(listenIp), httpPort, socksPort);
    }

    // launch
    this->process.start(executablePath, arguments);
    isStarted = true;
    return true;
}

void HysteriaKernel::SetConnectionSettings(const QMap<Qv2rayPlugin::KernelOptionFlags, QVariant> &options, const QJsonObject &settings)
{
    sni.clear();
    this->listenIp = options[KERNEL_LISTEN_ADDRESS].toString();
    this->socksPort = options[KERNEL_SOCKS_ENABLED].toBool() ? options[KERNEL_SOCKS_PORT].toInt() : 0;
    this->httpPort = options[KERNEL_HTTP_ENABLED].toBool() ? options[KERNEL_HTTP_PORT].toInt() : 0;

    hysteriaConf["server"] =  QString("%1:%2").arg(settings["host"].toString()).arg(settings["port"].toInt(443));

    QJsonObject socks5;
    socks5["listen"] = QString("%1:%2").arg(this->listenIp).arg(this->socksPort);
    socks5["disable_udp"] = false;
    hysteriaConf["socks5"] = socks5;

    hysteriaConf["retry"] = -1;
    hysteriaConf["retry_interval"] = 5;
    hysteriaConf["insecure"] = settings["insecure"].toBool(false);

    if (const auto caPath = settings["ca"].toString().trimmed(); !caPath.isEmpty()) {
        hysteriaConf["ca"] = caPath;
    } else {
        hysteriaConf.remove("ca");
    }

    if (const auto alpn = settings["alpn"].toString().trimmed(); !alpn.isEmpty()) {
        hysteriaConf["alpn"] = alpn;
    } else {
        hysteriaConf.remove("alpn");
    }

    if (const auto protocol = settings["protocol"].toString().trimmed(); !protocol.isEmpty()) {
        hysteriaConf["protocol"] = protocol;
    } else {
        emit OnKernelLogAvailable("warning: outbound protocol falled back to udp");
        hysteriaConf["protocol"] = "udp";
    }

    if (const auto obfs = settings["obfs"].toString().trimmed(); !obfs.isEmpty()) {
        hysteriaConf["obfs"] = obfs;
    } else {
        hysteriaConf.remove("obfs");
    }

    if (const auto authStr = settings["auth_str"].toString().trimmed(); !authStr.isEmpty()) {
        hysteriaConf["auth_str"] = authStr;
    } else {
        hysteriaConf.remove("authStr");
    }

    hysteriaConf["down_mbps"] = settings["down_mbps"].toInt(300);
    hysteriaConf["up_mbps"] = settings["up_mbps"].toInt(100);

    if (const auto serverName = settings["sni"].toString().trimmed(); !serverName.isEmpty()) {
        hysteriaConf["server_name"] = serverName;
    } else {
        hysteriaConf.remove("server_name");
    }
}

bool HysteriaKernel::StopKernel()
{
    if (this->httpPort)
    {
        httpProxy.close();
    }
    hysteriaConfJson.remove();
    isStarted = false;
    this->process.close();
    this->process.waitForFinished();
    return true;
}
