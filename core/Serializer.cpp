#include "Serializer.hpp"

#include <QUrlQuery>

const QString HysteriaOutboundHandler::SerializeOutbound(const QString &, const QString &alias, const QString &, const QJsonObject &object,
                                                           const QJsonObject &) const
{
    QUrl url;
    QUrlQuery query;
    url.setScheme("hysteria");
    url.setHost(object["host"].toString());
    if (const auto port = object["port"].toInt(443); port <= 0 || port >= 65536)
        url.setPort(443);
    else
        url.setPort(port);

    const QStringList protocols{"udp", "wetchat-video", "faketcp"};
    if (const auto protocol = object["protocol"].toString(); protocols.contains(protocol)) {
        query.addQueryItem("protocol", protocol);
    }

    if (const auto auth_str = object["auth_str"].toString(); !auth_str.isEmpty()) {
        query.addQueryItem("auth", auth_str);
    }

    if (const auto sni = object["sni"].toString(); !sni.isEmpty()) {
        query.addQueryItem("peer", sni);
    }

    if (const auto insecure = object["insecure"].toBool(false); insecure) {
        query.addQueryItem("insecure", object["insecure"].toString());
    }

    if (const auto upMbps = object["up_mbps"].toString(); !upMbps.isEmpty()) {
        query.addQueryItem("upmbps", upMbps);
    }

    if (const auto downMbps = object["down_mbps"].toString(); !downMbps.isEmpty()) {
        query.addQueryItem("downmbps", downMbps);
    }

    if (const auto alpn = object["alpn"].toString(); !alpn.isEmpty()) {
        query.addQueryItem("alpn", alpn);
    }

    if (const auto obfs = object["obfs"].toString(); !obfs.isEmpty() && obfs == "xplus") {
        query.addQueryItem("obfs", obfs);
    }

    if (const auto obfsStr = object["obfs_str"].toString(); !obfsStr.isEmpty()) {
        query.addQueryItem("obfsParam", obfsStr);
    }

    url.setQuery(query);

    url.setFragment(alias);

    return url.toString();
}

const void HysteriaOutboundHandler::SetOutboundInfo(const QString &protocol, const Qv2rayPlugin::OutboundInfoObject &info,
                                                      QJsonObject &outbound) const
{
    if (protocol != "naive")
        return;
    if (info.contains(Qv2rayPlugin::INFO_SERVER))
        outbound["host"] = info[Qv2rayPlugin::INFO_SERVER].toString();
    if (info.contains(Qv2rayPlugin::INFO_SERVER))
        outbound["port"] = info[Qv2rayPlugin::INFO_PORT].toInt();
    if (info.contains(Qv2rayPlugin::INFO_SNI))
        outbound["sni"] = info[Qv2rayPlugin::INFO_SNI].toString();
}

const QPair<QString, QJsonObject> HysteriaOutboundHandler::DeserializeOutbound(const QString &link, QString *alias,
                                                                                 QString *errorMessage) const
{
    const QString prefix = "hysteria://";
    if (!link.startsWith(prefix))
    {
        *errorMessage = ("Invalid Hysteria URI");
        return {};
    }

    const auto trueList = QStringList{ "true", "1", "yes", "y" };
    const QUrl hysteriaUrl(link.trimmed());
    const QUrlQuery query(hysteriaUrl.query());
    QJsonObject result;

    auto getQueryValue = [&](const QString &key) {
        return query.queryItemValue(key, QUrl::FullyDecoded);
    };

    if (!hysteriaUrl.isValid())
    {
        *errorMessage = hysteriaUrl.errorString();
        return {};
    }


    const auto description = hysteriaUrl.fragment(QUrl::FullyDecoded);
    if (!description.isEmpty())
    {
        *alias = description;
    }
    else
    {
        *alias = QString("[%1]-%2:%3").arg(hysteriaUrl.scheme()).arg(hysteriaUrl.host()).arg(hysteriaUrl.port());
    }

    if (const auto insecure = trueList.contains(query.queryItemValue("insecure").toLower()); insecure) {
        result["insecure"] =  insecure; // insecure, optional
    }
    result["host"] = hysteriaUrl.host();
    result["port"] = hysteriaUrl.port(443);
    result["protocol"] = getQueryValue("protocol").isEmpty() ? "udp" : getQueryValue("protocol"); // protocol, optional, default "udp"
    result["auth_str"] = getQueryValue("auth"); // authentication payload (string) (optional)

    // process sni (and also "peer")
    if (query.hasQueryItem("sni"))
    {
        result["sni"] = getQueryValue("sni");
    }
    else if (query.hasQueryItem("peer"))
    {
        // This is evil and may be removed in a future version.
        qWarning() << "use of 'peer' in trojan url is deprecated";
        result["sni"] = getQueryValue("peer");
    }

    if (const auto up_mbps = getQueryValue("upmbps"); !up_mbps.isEmpty()) {
        result["up_mbps"] = up_mbps;
    } else {
        result["up_mbps"] = "100";
    }

    if (const auto down_mbps = getQueryValue("downmbps"); !down_mbps.isEmpty()) {
        result["down_mbps"] = down_mbps;
    } else {
        result["down_mbps"] = "300";
    }

    if (const auto alpn =  getQueryValue("alpn"); !alpn.isEmpty()) {
        result["alpn"] = alpn; // alpn: QUIC ALPN (optional)
    }
    if (const auto obfsMode = getQueryValue("obfs"); !obfsMode.isEmpty()) {
        qWarning() << "Non-empty obfs mode is not well supported. Official document missing.";
    }
    if (const auto obfs = getQueryValue("obfsParam").trimmed(); !obfs.isEmpty()) {
        result["obfs"] = obfs; // obfsParam: Obfuscation password (optional)
    }

    return { "hysteria", result};
}

const Qv2rayPlugin::OutboundInfoObject HysteriaOutboundHandler::GetOutboundInfo(const QString &protocol, const QJsonObject &outbound) const
{
    return { //
             { Qv2rayPlugin::INFO_PROTOCOL, protocol },
             { Qv2rayPlugin::INFO_SERVER, outbound["host"].toString() },
             { Qv2rayPlugin::INFO_PORT, outbound["port"].toInt() },
             { Qv2rayPlugin::INFO_SNI, outbound["sni"].toString() }
    };
}
