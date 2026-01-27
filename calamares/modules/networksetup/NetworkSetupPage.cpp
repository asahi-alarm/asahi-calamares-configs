/* SPDX-FileCopyrightText: no
 * SPDX-License-Identifier: CC0-1.0
 */

#include "NetworkSetupPage.h"

#include "utils/Logger.h"

#include <algorithm>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSet>
#include <QTimer>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusArgument>
#include <QDBusMetaType>

// NM device types
constexpr uint NM_DEVICE_TYPE_WIFI = 2;

// NM device state
constexpr uint NM_DEVICE_STATE_ACTIVATED = 100;

// NM 802.11 AP flags
constexpr uint NM_802_11_AP_FLAGS_PRIVACY = 0x1;

// Password dialog for WiFi networks
class PasswordDialog : public QDialog
{
public:
    explicit PasswordDialog(const QString& ssid, QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle(tr("WiFi Password"));
        setMinimumWidth(300);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel(QStringLiteral("<b>%1</b>").arg(ssid)));

        m_passwordEdit = new QLineEdit();
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        m_passwordEdit->setPlaceholderText(tr("Enter password"));
        layout->addWidget(m_passwordEdit);

        auto* showPassword = new QCheckBox(tr("Show password"));
        connect(showPassword, &QCheckBox::toggled, this, [this](bool checked) {
            m_passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        });
        layout->addWidget(showPassword);

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addWidget(buttons);

        connect(m_passwordEdit, &QLineEdit::returnPressed, this, &QDialog::accept);
    }

    QString password() const { return m_passwordEdit->text(); }

private:
    QLineEdit* m_passwordEdit;
};


NetworkSetupPage::NetworkSetupPage(QWidget* parent)
    : QWidget(parent)
    , m_bus(QDBusConnection::systemBus())
{
    qDBusRegisterMetaType<QList<QDBusObjectPath>>();

    setupUi();
    findWirelessDevice();
    checkConnection();  // Check initial connection state (wired or existing WiFi)
    QTimer::singleShot(500, this, &NetworkSetupPage::scan);

    // Periodically check connection state (detects ethernet plug-in, etc.)
    m_connectionCheckTimer = new QTimer(this);
    connect(m_connectionCheckTimer, &QTimer::timeout, this, &NetworkSetupPage::checkConnection);
    m_connectionCheckTimer->start(2000);  // Check every 2 seconds
}

void
NetworkSetupPage::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto* title = new QLabel(tr("Network Setup"));
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    // Subtitle
    auto* subtitle = new QLabel(tr("A network connection is required to download packages during setup."));
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    layout->addSpacing(10);

    // Connection status
    auto* statusLayout = new QHBoxLayout();
    m_statusDot = new QLabel(QStringLiteral("\u2B24"));
    m_statusDot->setStyleSheet(QStringLiteral("color: #9E9E9E;"));
    statusLayout->addWidget(m_statusDot);
    m_statusLabel = new QLabel(tr("Not connected"));
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    layout->addLayout(statusLayout);

    layout->addSpacing(10);

    layout->addWidget(new QLabel(tr("Available networks:")));

    // Network list
    m_networkList = new QListWidget();
    m_networkList->setMinimumHeight(200);
    connect(m_networkList, &QListWidget::itemDoubleClicked,
            this, &NetworkSetupPage::onItemDoubleClicked);
    layout->addWidget(m_networkList);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    m_scanBtn = new QPushButton(tr("Scan"));
    connect(m_scanBtn, &QPushButton::clicked, this, &NetworkSetupPage::scan);
    btnLayout->addWidget(m_scanBtn);

    m_connectBtn = new QPushButton(tr("Connect"));
    connect(m_connectBtn, &QPushButton::clicked, this, &NetworkSetupPage::onConnect);
    btnLayout->addWidget(m_connectBtn);

    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    layout->addStretch();
}

void
NetworkSetupPage::findWirelessDevice()
{
    if (!m_bus.isConnected())
    {
        cWarning() << "NetworkSetup: D-Bus system bus not connected";
        return;
    }

    QDBusInterface nm(NM_SERVICE, NM_PATH, NM_IFACE, m_bus);
    if (!nm.isValid())
    {
        cWarning() << "NetworkSetup: NetworkManager service not available";
        return;
    }

    // Get all devices
    QDBusReply<QList<QDBusObjectPath>> reply = nm.call(QStringLiteral("GetDevices"));
    if (!reply.isValid())
    {
        cWarning() << "NetworkSetup: GetDevices failed:" << reply.error().message();
        return;
    }

    for (const auto& devicePath : reply.value())
    {
        QDBusInterface device(NM_SERVICE, devicePath.path(),
                              QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);

        QDBusReply<QVariant> typeReply = device.call(
            QStringLiteral("Get"), NM_DEVICE_IFACE, QStringLiteral("DeviceType"));

        if (typeReply.isValid() && typeReply.value().toUInt() == NM_DEVICE_TYPE_WIFI)
        {
            m_wirelessDevice = devicePath;
            cDebug() << "NetworkSetup: found wireless device at" << devicePath.path();
            return;
        }
    }

    cWarning() << "NetworkSetup: no WiFi device found";
}

void
NetworkSetupPage::scan()
{
    if (m_wirelessDevice.path().isEmpty())
    {
        m_statusLabel->setText(tr("No WiFi adapter found"));
        return;
    }

    m_scanBtn->setEnabled(false);
    m_scanBtn->setText(tr("Scanning..."));

    QDBusInterface wireless(NM_SERVICE, m_wirelessDevice.path(), NM_WIRELESS_IFACE, m_bus);
    if (wireless.isValid())
    {
        // RequestScan takes a dict of options (empty for default scan)
        QVariantMap options;
        wireless.call(QDBus::NoBlock, QStringLiteral("RequestScan"), options);
        QTimer::singleShot(3000, this, &NetworkSetupPage::loadAccessPoints);
    }
    else
    {
        m_scanBtn->setEnabled(true);
        m_scanBtn->setText(tr("Scan"));
        cWarning() << "NetworkSetup: wireless interface invalid";
    }
}

void
NetworkSetupPage::loadAccessPoints()
{
    m_scanBtn->setEnabled(true);
    m_scanBtn->setText(tr("Scan"));

    if (m_wirelessDevice.path().isEmpty())
        return;

    QDBusInterface wireless(NM_SERVICE, m_wirelessDevice.path(), NM_WIRELESS_IFACE, m_bus);
    if (!wireless.isValid())
        return;

    m_accessPoints.clear();

    // Get access points
    QDBusReply<QList<QDBusObjectPath>> reply = wireless.call(QStringLiteral("GetAccessPoints"));
    if (!reply.isValid())
    {
        cWarning() << "NetworkSetup: GetAccessPoints failed:" << reply.error().message();
        return;
    }

    for (const auto& apPath : reply.value())
    {
        QDBusInterface ap(NM_SERVICE, apPath.path(),
                          QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);

        QDBusReply<QVariant> ssidReply = ap.call(
            QStringLiteral("Get"), NM_AP_IFACE, QStringLiteral("Ssid"));
        QDBusReply<QVariant> strengthReply = ap.call(
            QStringLiteral("Get"), NM_AP_IFACE, QStringLiteral("Strength"));
        QDBusReply<QVariant> flagsReply = ap.call(
            QStringLiteral("Get"), NM_AP_IFACE, QStringLiteral("Flags"));
        QDBusReply<QVariant> wpaFlagsReply = ap.call(
            QStringLiteral("Get"), NM_AP_IFACE, QStringLiteral("WpaFlags"));
        QDBusReply<QVariant> rsnFlagsReply = ap.call(
            QStringLiteral("Get"), NM_AP_IFACE, QStringLiteral("RsnFlags"));

        if (!ssidReply.isValid())
            continue;

        QByteArray ssidBytes = ssidReply.value().toByteArray();
        QString ssid = QString::fromUtf8(ssidBytes);

        if (ssid.isEmpty())
            continue;

        AccessPointInfo info;
        info.path = apPath;
        info.ssid = ssid;
        info.strength = strengthReply.isValid() ? strengthReply.value().toUInt() : 0;
        info.flags = flagsReply.isValid() ? flagsReply.value().toUInt() : 0;
        info.wpaFlags = wpaFlagsReply.isValid() ? wpaFlagsReply.value().toUInt() : 0;
        info.rsnFlags = rsnFlagsReply.isValid() ? rsnFlagsReply.value().toUInt() : 0;

        // Network is secured if privacy flag is set or WPA/RSN flags are non-zero
        info.secured = (info.flags & NM_802_11_AP_FLAGS_PRIVACY) ||
                       info.wpaFlags != 0 || info.rsnFlags != 0;

        m_accessPoints.append(info);
    }

    // Sort by signal strength (descending)
    std::sort(m_accessPoints.begin(), m_accessPoints.end(),
              [](const AccessPointInfo& a, const AccessPointInfo& b) {
                  return a.strength > b.strength;
              });

    updateList();
    checkConnection();
}

void
NetworkSetupPage::updateList()
{
    m_networkList->clear();

    // Track seen SSIDs to avoid duplicates
    QSet<QString> seenSsids;

    for (const auto& ap : m_accessPoints)
    {
        if (seenSsids.contains(ap.ssid))
            continue;
        seenSsids.insert(ap.ssid);

        QString secType = ap.secured ? QStringLiteral("Secured") : QStringLiteral("Open");
        QString strengthStr;
        if (ap.strength > 75)
            strengthStr = QStringLiteral("\u2582\u2584\u2586\u2588");
        else if (ap.strength > 50)
            strengthStr = QStringLiteral("\u2582\u2584\u2586");
        else if (ap.strength > 25)
            strengthStr = QStringLiteral("\u2582\u2584");
        else
            strengthStr = QStringLiteral("\u2582");

        QString text = QStringLiteral("%1  %2  (%3)").arg(strengthStr, ap.ssid, secType);

        auto* item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, QVariant::fromValue(ap.path.path()));
        item->setData(Qt::UserRole + 1, QVariant::fromValue(ap.ssid));
        item->setData(Qt::UserRole + 2, QVariant::fromValue(ap.secured));

        m_networkList->addItem(item);
    }
}

void
NetworkSetupPage::checkConnection()
{
    bool wasConnected = m_isConnected;
    m_isConnected = false;
    QString connectionName;

    // First check NetworkManager's global connectivity state
    // This catches wired connections and any existing network access
    QDBusInterface nm(NM_SERVICE, NM_PATH,
                      QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);
    if (nm.isValid())
    {
        QDBusReply<QVariant> connReply = nm.call(
            QStringLiteral("Get"), NM_IFACE, QStringLiteral("Connectivity"));
        // NM_CONNECTIVITY_FULL = 4
        if (connReply.isValid() && connReply.value().toUInt() == 4)
        {
            m_isConnected = true;

            // Try to get the primary connection name
            QDBusReply<QVariant> primaryReply = nm.call(
                QStringLiteral("Get"), NM_IFACE, QStringLiteral("PrimaryConnection"));
            if (primaryReply.isValid())
            {
                QDBusObjectPath connPath = primaryReply.value().value<QDBusObjectPath>();
                if (!connPath.path().isEmpty() && connPath.path() != QStringLiteral("/"))
                {
                    QDBusInterface conn(NM_SERVICE, connPath.path(),
                                        QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);
                    QDBusReply<QVariant> idReply = conn.call(
                        QStringLiteral("Get"),
                        QStringLiteral("org.freedesktop.NetworkManager.Connection.Active"),
                        QStringLiteral("Id"));
                    if (idReply.isValid())
                        connectionName = idReply.value().toString();
                }
            }
        }
    }

    // Also check WiFi device state (for display purposes)
    if (!m_isConnected && !m_wirelessDevice.path().isEmpty())
    {
        QDBusInterface device(NM_SERVICE, m_wirelessDevice.path(),
                              QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);

        QDBusReply<QVariant> stateReply = device.call(
            QStringLiteral("Get"), NM_DEVICE_IFACE, QStringLiteral("State"));

        if (stateReply.isValid() && stateReply.value().toUInt() == NM_DEVICE_STATE_ACTIVATED)
        {
            m_isConnected = true;

            // Get active access point name
            QDBusInterface wireless(NM_SERVICE, m_wirelessDevice.path(),
                                    QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);
            QDBusReply<QVariant> apReply = wireless.call(
                QStringLiteral("Get"), NM_WIRELESS_IFACE, QStringLiteral("ActiveAccessPoint"));

            if (apReply.isValid())
            {
                QDBusObjectPath apPath = apReply.value().value<QDBusObjectPath>();
                if (!apPath.path().isEmpty() && apPath.path() != QStringLiteral("/"))
                {
                    QDBusInterface ap(NM_SERVICE, apPath.path(),
                                      QStringLiteral("org.freedesktop.DBus.Properties"), m_bus);
                    QDBusReply<QVariant> ssidReply = ap.call(
                        QStringLiteral("Get"), NM_AP_IFACE, QStringLiteral("Ssid"));
                    if (ssidReply.isValid())
                        connectionName = QString::fromUtf8(ssidReply.value().toByteArray());
                }
            }
        }
    }

    // Update UI
    if (m_isConnected)
    {
        m_statusDot->setStyleSheet(QStringLiteral("color: #4CAF50;"));
        if (!connectionName.isEmpty())
            m_statusLabel->setText(tr("Connected: %1").arg(connectionName));
        else
            m_statusLabel->setText(tr("Connected"));
    }
    else
    {
        m_statusDot->setStyleSheet(QStringLiteral("color: #9E9E9E;"));
        m_statusLabel->setText(tr("Not connected"));
    }

    if (wasConnected != m_isConnected)
        emit connectionStateChanged(m_isConnected);
}

void
NetworkSetupPage::onItemDoubleClicked(QListWidgetItem* item)
{
    Q_UNUSED(item)
    onConnect();
}

void
NetworkSetupPage::onConnect()
{
    auto* item = m_networkList->currentItem();
    if (!item)
        return;

    QString pathStr = item->data(Qt::UserRole).toString();
    QString ssid = item->data(Qt::UserRole + 1).toString();
    bool secured = item->data(Qt::UserRole + 2).toBool();

    QDBusObjectPath apPath(pathStr);

    if (!secured)
    {
        doConnect(apPath, ssid, false, QString());
    }
    else
    {
        PasswordDialog dlg(ssid, this);
        if (dlg.exec() == QDialog::Accepted)
        {
            doConnect(apPath, ssid, true, dlg.password());
        }
    }
}

// Type alias for NM connection settings: a{sa{sv}}
using NMVariantMapMap = QMap<QString, QVariantMap>;

void
NetworkSetupPage::doConnect(const QDBusObjectPath& apPath, const QString& ssid,
                            bool secured, const QString& password)
{
    m_statusDot->setStyleSheet(QStringLiteral("color: #FFC107;"));
    m_statusLabel->setText(tr("Connecting..."));

    // Build connection settings as a{sa{sv}}
    NMVariantMapMap settings;

    // Connection section
    QVariantMap connection;
    connection[QStringLiteral("type")] = QStringLiteral("802-11-wireless");
    connection[QStringLiteral("id")] = ssid;
    settings[QStringLiteral("connection")] = connection;

    // Wireless section
    QVariantMap wireless;
    wireless[QStringLiteral("ssid")] = ssid.toUtf8();
    wireless[QStringLiteral("mode")] = QStringLiteral("infrastructure");
    settings[QStringLiteral("802-11-wireless")] = wireless;

    // Security section (if needed)
    if (secured && !password.isEmpty())
    {
        // Also need to reference security in wireless section
        wireless[QStringLiteral("security")] = QStringLiteral("802-11-wireless-security");
        settings[QStringLiteral("802-11-wireless")] = wireless;

        QVariantMap security;
        security[QStringLiteral("key-mgmt")] = QStringLiteral("wpa-psk");
        security[QStringLiteral("psk")] = password;
        settings[QStringLiteral("802-11-wireless-security")] = security;
    }

    // IPv4 section
    QVariantMap ipv4;
    ipv4[QStringLiteral("method")] = QStringLiteral("auto");
    settings[QStringLiteral("ipv4")] = ipv4;

    // IPv6 section
    QVariantMap ipv6;
    ipv6[QStringLiteral("method")] = QStringLiteral("auto");
    settings[QStringLiteral("ipv6")] = ipv6;

    // Use AddAndActivateConnection
    QDBusInterface nm(NM_SERVICE, NM_PATH, NM_IFACE, m_bus);
    if (!nm.isValid())
    {
        QMessageBox::warning(this, tr("Error"), tr("NetworkManager not available"));
        return;
    }

    // Build the D-Bus message manually for correct typing
    QDBusMessage msg = QDBusMessage::createMethodCall(
        NM_SERVICE, NM_PATH, NM_IFACE, QStringLiteral("AddAndActivateConnection"));

    // Serialize settings as a{sa{sv}}
    QDBusArgument arg;
    arg.beginMap(QMetaType::fromType<QString>(), QMetaType::fromType<QVariantMap>());
    for (auto it = settings.constBegin(); it != settings.constEnd(); ++it)
    {
        arg.beginMapEntry();
        arg << it.key() << it.value();
        arg.endMapEntry();
    }
    arg.endMap();

    msg << QVariant::fromValue(arg);
    msg << QVariant::fromValue(m_wirelessDevice);
    msg << QVariant::fromValue(apPath);

    QDBusMessage reply = m_bus.call(msg);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        cWarning() << "NetworkSetup: AddAndActivateConnection failed:" << reply.errorMessage();
        QMessageBox::warning(this, tr("Error"), reply.errorMessage());
        m_statusDot->setStyleSheet(QStringLiteral("color: #9E9E9E;"));
        m_statusLabel->setText(tr("Connection failed"));
        return;
    }

    cDebug() << "NetworkSetup: connection activated";
    QTimer::singleShot(3000, this, &NetworkSetupPage::loadAccessPoints);
}
