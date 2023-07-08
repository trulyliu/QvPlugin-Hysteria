#include "HysteriaPlugin.hpp"

#include "core/EventHandler.hpp"
#include "core/Kernel.hpp"
#include "core/Serializer.hpp"
#include "ui/UIInterface.hpp"

#include <QDateTime>
#include <QLabel>
#include <QMetaEnum>

bool HysteriaPlugin::InitializePlugin(const QString &, const QJsonObject &_settings)
{
    this->settings = _settings;
    HysteriaPluginInstance = this;
    emit PluginLog("Initializing Hysteria plugin.");
    this->outboundHandler = std::make_unique<HysteriaOutboundHandler>();
    this->eventHandler = std::make_unique<NaiveEventHandler>();
    this->kernelInterface = std::make_shared<NaiveKernelInterface>();
    this->guiInterface = new NaiveUIInterface();
    return true;
}
