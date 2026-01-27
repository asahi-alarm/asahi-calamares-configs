/* SPDX-FileCopyrightText: no
 * SPDX-License-Identifier: CC0-1.0
 */

#include "NetworkSetupViewStep.h"
#include "NetworkSetupPage.h"

#include "utils/Logger.h"

CALAMARES_PLUGIN_FACTORY_DEFINITION(NetworkSetupViewStepFactory, registerPlugin<NetworkSetupViewStep>();)

NetworkSetupViewStep::NetworkSetupViewStep(QObject* parent)
    : Calamares::ViewStep(parent)
    , m_widget(new NetworkSetupPage())
{
    cDebug() << "NetworkSetup viewstep created";

    connect(m_widget, &NetworkSetupPage::connectionStateChanged,
            this, &NetworkSetupViewStep::onConnectionStateChanged);

    // Check initial connection state
    m_isConnected = m_widget->isConnected();
}

NetworkSetupViewStep::~NetworkSetupViewStep()
{
    if (m_widget && m_widget->parent() == nullptr)
    {
        m_widget->deleteLater();
    }
}

QString
NetworkSetupViewStep::prettyName() const
{
    return tr("Network", "@title");
}

QWidget*
NetworkSetupViewStep::widget()
{
    return m_widget;
}

bool
NetworkSetupViewStep::isNextEnabled() const
{
    return m_isConnected;
}

bool
NetworkSetupViewStep::isBackEnabled() const
{
    return true;
}

bool
NetworkSetupViewStep::isAtBeginning() const
{
    return true;
}

bool
NetworkSetupViewStep::isAtEnd() const
{
    return true;
}

Calamares::JobList
NetworkSetupViewStep::jobs() const
{
    // No jobs - network connection happens immediately in the UI
    return Calamares::JobList();
}

void
NetworkSetupViewStep::setConfigurationMap(const QVariantMap& configurationMap)
{
    Q_UNUSED(configurationMap)
}

void
NetworkSetupViewStep::onConnectionStateChanged(bool connected)
{
    m_isConnected = connected;
    emit nextStatusChanged(connected);
}
