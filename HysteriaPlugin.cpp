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
    this->eventHandler = std::make_unique<HysteriaEventHandler>();
    this->kernelInterface = std::make_shared<HysteriaKernelInterface>();
    this->guiInterface = new HysteriaUIInterface();
    return true;
}
