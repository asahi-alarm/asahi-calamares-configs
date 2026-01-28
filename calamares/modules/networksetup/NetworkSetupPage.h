/* SPDX-FileCopyrightText: no
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef NETWORKSETUPPAGE_H
#define NETWORKSETUPPAGE_H

#include <QWidget>
#include <QDBusConnection>
#include <QDBusObjectPath>

class QLabel;
class QTimer;
class QListWidget;
class QListWidgetItem;
class QPushButton;

struct AccessPointInfo
{
    QDBusObjectPath path;
    QString ssid;
    uint strength;
    uint flags;      // NM_802_11_AP_FLAGS
    uint wpaFlags;   // NM_802_11_AP_SEC
    uint rsnFlags;   // NM_802_11_AP_SEC
    bool secured;
};

class NetworkSetupPage : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSetupPage(QWidget* parent = nullptr);
    ~NetworkSetupPage() override = default;

    bool isConnected() const { return m_isConnected; }

signals:
    void connectionStateChanged(bool connected);

private slots:
    void scan();
    void loadAccessPoints();
    void onConnect();
    void onItemDoubleClicked(QListWidgetItem* item);
    void checkConnection();

private:
    void setupUi();
    void findWirelessDevice();
    void updateList();
    void doConnect(const QDBusObjectPath& apPath, const QString& ssid, bool secured, const QString& password);

    QLabel* m_statusDot;
    QLabel* m_statusLabel;
    QListWidget* m_networkList;
    QPushButton* m_scanBtn;
    QPushButton* m_connectBtn;

    QDBusConnection m_bus;
    QDBusObjectPath m_wirelessDevice;
    QList<AccessPointInfo> m_accessPoints;
    bool m_isConnected = false;
    QTimer* m_connectionCheckTimer = nullptr;

    static constexpr const char* NM_SERVICE = "org.freedesktop.NetworkManager";
    static constexpr const char* NM_PATH = "/org/freedesktop/NetworkManager";
    static constexpr const char* NM_IFACE = "org.freedesktop.NetworkManager";
    static constexpr const char* NM_DEVICE_IFACE = "org.freedesktop.NetworkManager.Device";
    static constexpr const char* NM_WIRELESS_IFACE = "org.freedesktop.NetworkManager.Device.Wireless";
    static constexpr const char* NM_AP_IFACE = "org.freedesktop.NetworkManager.AccessPoint";
};

#endif // NETWORKSETUPPAGE_H
