/* SPDX-FileCopyrightText: no
 * SPDX-License-Identifier: CC0-1.0
 *
 * Calamares network setup viewmodule using iwd via D-Bus
 */

#ifndef NETWORKSETUPVIEWSTEP_H
#define NETWORKSETUPVIEWSTEP_H

#include "DllMacro.h"
#include "utils/PluginFactory.h"
#include "viewpages/ViewStep.h"

#include <QObject>

class NetworkSetupPage;

class PLUGINDLLEXPORT NetworkSetupViewStep : public Calamares::ViewStep
{
    Q_OBJECT

public:
    explicit NetworkSetupViewStep(QObject* parent = nullptr);
    ~NetworkSetupViewStep() override;

    QString prettyName() const override;
    QWidget* widget() override;

    bool isNextEnabled() const override;
    bool isBackEnabled() const override;
    bool isAtBeginning() const override;
    bool isAtEnd() const override;

    Calamares::JobList jobs() const override;

    void setConfigurationMap(const QVariantMap& configurationMap) override;

private slots:
    void onConnectionStateChanged(bool connected);

private:
    NetworkSetupPage* m_widget;
    bool m_isConnected = false;
};

CALAMARES_PLUGIN_FACTORY_DECLARATION(NetworkSetupViewStepFactory)

#endif // NETWORKSETUPVIEWSTEP_H
