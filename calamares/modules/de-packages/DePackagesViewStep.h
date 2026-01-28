/* SPDX-FileCopyrightText: no
 * SPDX-License-Identifier: CC0-1.0
 *
 * Calamares viewstep that maps DE selections to package operations.
 */

#ifndef DEPACKAGESVIEWSTEP_H
#define DEPACKAGESVIEWSTEP_H

#include "DllMacro.h"
#include "utils/PluginFactory.h"
#include "viewpages/ViewStep.h"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QColor>
#include <QPixmap>

class QWidget;
class QVBoxLayout;
class QButtonGroup;
class QFrame;
class QRadioButton;
class QLabel;

struct DesktopChoice
{
    QString id;
    QString name;
    QString description;
    QString screenshot;
};

class PLUGINDLLEXPORT DePackagesViewStep : public Calamares::ViewStep
{
    Q_OBJECT

public:
    explicit DePackagesViewStep( QObject* parent = nullptr );
    ~DePackagesViewStep() override;

    QString prettyName() const override;
    QString prettyStatus() const override;

    QWidget* widget() override;

    bool isNextEnabled() const override;
    bool isBackEnabled() const override;
    bool isAtBeginning() const override;
    bool isAtEnd() const override;

    Calamares::JobList jobs() const override;

    void setConfigurationMap( const QVariantMap& configurationMap ) override;
    void onActivate() override;

private:
    void ensureWidget();
    void updateSelection();
    void setStatusMessage( const QString& message, bool isError );
    void buildChoicesUi( QWidget* page, QVBoxLayout* layout );
    void handleSelectionChanged( const QString& selectionId );
    bool applySelection( const QString& selection );
    void selectButtonForId( const QString& selection );
    void updateFrameHighlights( const QString& selection );
    QVector< DesktopChoice > availableChoices() const;
    QString frameStyleSheet( bool selected ) const;
    void setCanProceed( bool enabled );
    QPixmap loadScreenshot( const QString& path ) const;
    QPixmap placeholderPixmap( const QString& label ) const;

    struct OptionWidget
    {
        QFrame* frame = nullptr;
        QRadioButton* button = nullptr;
    };

    QWidget* m_widget = nullptr;
    QLabel* m_statusLabel = nullptr;
    QButtonGroup* m_choiceGroup = nullptr;
    QVector< DesktopChoice > m_choices;
    QHash< QString, OptionWidget > m_optionWidgets;
    QString m_lastSelection;
    QString m_statusMessage;
    bool m_statusIsError = false;
    bool m_canProceed = false;
    QColor m_frameBorderColor;
    QColor m_frameHighlightColor;
    QColor m_frameHighlightBackground;
    QColor m_mutedTextColor;
};

CALAMARES_PLUGIN_FACTORY_DECLARATION( DePackagesViewStepFactory )

#endif  // DEPACKAGESVIEWSTEP_H
