/* SPDX-FileCopyrightText: no
 * SPDX-License-Identifier: CC0-1.0
 */

#include "DePackagesViewStep.h"

#include "GlobalStorage.h"
#include "JobQueue.h"
#include "Branding.h"
#include "utils/Logger.h"

#include <algorithm>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QFile>
#include <QFont>
#include <QHash>
#include <QLabel>
#include <QRadioButton>
#include <QPixmap>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QTextStream>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QPalette>
#include <QFileInfo>
#include <QPainter>

namespace
{
struct DesktopConfig
{
    QStringList packages;
    QString displayManager;
};

const QHash< QString, DesktopConfig > s_desktops = {
    { QStringLiteral( "plasma" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "plasma-meta" ),
              QStringLiteral( "kde-applications-meta" ),
              QStringLiteral( "sddm" ),
              QStringLiteral( "konsole" ),
              QStringLiteral( "dolphin" ),
              QStringLiteral( "audacity" ),
              QStringLiteral( "qt6-multimedia-gstreamer" ),
          },
          QStringLiteral( "sddm" ),
      } },
    { QStringLiteral( "gnome" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "gnome" ),
              QStringLiteral( "gnome-tweaks" ),
              QStringLiteral( "gdm" ),
          },
          QStringLiteral( "gdm" ),
      } },
    { QStringLiteral( "cosmic" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "cosmic" ),
              QStringLiteral( "cosmic-greeter" ),
          },
          QStringLiteral( "cosmic-greeter" ),
      } },
    { QStringLiteral( "xfce" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "xfce4" ),
              QStringLiteral( "xfce4-goodies" ),
              QStringLiteral( "lightdm" ),
              QStringLiteral( "lightdm-gtk-greeter" ),
              QStringLiteral( "gvfs" ),
              QStringLiteral( "feh" ),
              QStringLiteral( "blueman" ),
              QStringLiteral( "network-manager-applet" ),
              QStringLiteral( "xfce4-terminal" ),
              QStringLiteral( "thunar" ),
          },
          QStringLiteral( "lightdm" ),
      } },
    { QStringLiteral( "lxqt" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "lxqt" ),
              QStringLiteral( "lightdm" ),
              QStringLiteral( "lightdm-gtk-greeter" ),
              QStringLiteral( "qterminal" ),
              QStringLiteral( "gvfs" ),
              QStringLiteral( "feh" ),
              QStringLiteral( "blueman" ),
              QStringLiteral( "xorg-xinit" ),
              QStringLiteral( "network-manager-applet" ),
              QStringLiteral( "pcmanfm-qt" ),
          },
          QStringLiteral( "lightdm" ),
      } },
    { QStringLiteral( "mate" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "mate" ),
              QStringLiteral( "mate-extra" ),
              QStringLiteral( "lightdm" ),
              QStringLiteral( "lightdm-gtk-greeter" ),
              QStringLiteral( "gvfs" ),
              QStringLiteral( "feh" ),
              QStringLiteral( "blueman" ),
              QStringLiteral( "system-config-printer" ),
              QStringLiteral( "xorg-xinit" ),
              QStringLiteral( "network-manager-applet" ),
          },
          QStringLiteral( "lightdm" ),
      } },
    { QStringLiteral( "hyprland" ),
      DesktopConfig{
          QStringList{
              QStringLiteral( "hyprland" ),
              QStringLiteral( "hyprcursor" ),
              QStringLiteral( "hyprgraphics" ),
              QStringLiteral( "hypridle" ),
              QStringLiteral( "hyprland-protocols" ),
              QStringLiteral( "hyprland-qt-support" ),
              QStringLiteral( "hyprland-guiutils" ),
              QStringLiteral( "hyprlang" ),
              QStringLiteral( "hyprlauncher" ),
              QStringLiteral( "hyprlock" ),
              QStringLiteral( "hyprpaper" ),
              QStringLiteral( "hyprpicker" ),
              QStringLiteral( "hyprpolkitagent" ),
              QStringLiteral( "hyprsunset" ),
              QStringLiteral( "hyprutils" ),
              QStringLiteral( "mako" ),
              QStringLiteral( "wl-clipboard" ),
              QStringLiteral( "cliphist" ),
              QStringLiteral( "nwg-displays" ),
              QStringLiteral( "nwg-dock-hyprland" ),
              QStringLiteral( "nwg-panel" ),
              QStringLiteral( "sddm" ),
              QStringLiteral( "uwsm" ),
              QStringLiteral( "kitty" ),
              QStringLiteral( "libnewt" ),
              QStringLiteral( "libnotify" ),
              QStringLiteral( "wmenu" ),
              QStringLiteral( "labwc" ),
              QStringLiteral( "dolphin" ),
              QStringLiteral( "xdg-desktop-portal" ),
              QStringLiteral( "xdg-desktop-portal-hyprland" ),
          },
          QStringLiteral( "sddm" ),
      } },
};

QString formatPackages( const QStringList& packages )
{
    return packages.join( QStringLiteral( ", " ) );
}
}  // namespace

CALAMARES_PLUGIN_FACTORY_DEFINITION( DePackagesViewStepFactory, registerPlugin< DePackagesViewStep >(); )

DePackagesViewStep::DePackagesViewStep( QObject* parent )
    : Calamares::ViewStep( parent )
{
    setCanProceed( false );
    setStatusMessage( tr( "Select a desktop to continue." ), true );
}

DePackagesViewStep::~DePackagesViewStep()
{
    if ( m_widget && !m_widget->parent() )
    {
        m_widget->deleteLater();
    }
}

QString
DePackagesViewStep::prettyName() const
{
    return tr( "Desktop Packages", "@title" );
}

QString
DePackagesViewStep::prettyStatus() const
{
    return m_statusMessage;
}

QWidget*
DePackagesViewStep::widget()
{
    ensureWidget();
    return m_widget;
}

bool
DePackagesViewStep::isNextEnabled() const
{
    return m_canProceed;
}

bool
DePackagesViewStep::isBackEnabled() const
{
    return true;
}

bool
DePackagesViewStep::isAtBeginning() const
{
    return true;
}

bool
DePackagesViewStep::isAtEnd() const
{
    return true;
}

Calamares::JobList
DePackagesViewStep::jobs() const
{
    return Calamares::JobList();
}

void
DePackagesViewStep::setConfigurationMap( const QVariantMap& configurationMap )
{
    m_choices.clear();

    const QVariantList items = configurationMap.value( QStringLiteral( "items" ) ).toList();
    for ( const QVariant& item : items )
    {
        const QVariantMap map = item.toMap();
        DesktopChoice choice;
        choice.id = map.value( QStringLiteral( "id" ) ).toString();
        if ( choice.id.isEmpty() )
        {
            continue;
        }
        choice.name = map.value( QStringLiteral( "name" ) ).toString();
        if ( choice.name.isEmpty() )
        {
            choice.name = choice.id;
        }
        choice.description = map.value( QStringLiteral( "description" ) ).toString();
        choice.screenshot = map.value( QStringLiteral( "screenshot" ) ).toString();
        m_choices.append( choice );
    }
}

void
DePackagesViewStep::onActivate()
{
    ensureWidget();
    updateSelection();
}

void
DePackagesViewStep::ensureWidget()
{
    if ( m_widget )
    {
        return;
    }

    auto* page = new QWidget();
    auto* layout = new QVBoxLayout( page );
    layout->setSpacing( 12 );
    layout->setContentsMargins( 20, 20, 20, 20 );

    const QPalette palette = page->palette();
    m_frameBorderColor = palette.color( QPalette::Mid );
    if ( !m_frameBorderColor.isValid() )
    {
        m_frameBorderColor = palette.color( QPalette::WindowText );
        m_frameBorderColor.setAlphaF( 0.3 );
    }
    m_frameHighlightColor = palette.color( QPalette::Highlight );
    if ( !m_frameHighlightColor.isValid() )
    {
        m_frameHighlightColor = QColor( 63, 81, 181 );
    }
    m_frameHighlightBackground = m_frameHighlightColor;
    m_frameHighlightBackground.setAlpha( 45 );
    m_mutedTextColor = palette.color( QPalette::PlaceholderText );
    if ( !m_mutedTextColor.isValid() )
    {
        m_mutedTextColor = palette.color( QPalette::Mid );
    }

    auto* title = new QLabel( tr( "Desktop Packages" ), page );
    QFont titleFont;
    titleFont.setPointSize( 18 );
    titleFont.setBold( true );
    title->setFont( titleFont );
    layout->addWidget( title );

    auto* subtitle = new QLabel(
        tr( "Your desktop selection determines which packages Calamares will install." ), page );
    subtitle->setWordWrap( true );
    layout->addWidget( subtitle );

    layout->addSpacing( 10 );

    m_statusLabel = new QLabel( tr( "Waiting for a desktop selection ..." ), page );
    m_statusLabel->setWordWrap( true );
    layout->addWidget( m_statusLabel );

    layout->addSpacing( 15 );

    auto* instruction = new QLabel( tr( "Select a desktop environment to install:" ), page );
    instruction->setWordWrap( true );
    layout->addWidget( instruction );

    buildChoicesUi( page, layout );

    layout->addSpacing( 10 );

    auto* info = new QLabel(
        tr( "This step automatically translates the selection from the package chooser." ), page );
    info->setWordWrap( true );
    if ( m_mutedTextColor.isValid() )
    {
        QPalette infoPalette = info->palette();
        infoPalette.setColor( QPalette::WindowText, m_mutedTextColor );
        info->setPalette( infoPalette );
    }
    layout->addWidget( info );

    m_widget = page;

    if ( !m_statusMessage.isEmpty() )
    {
        setStatusMessage( m_statusMessage, m_statusIsError );
    }
}

void
DePackagesViewStep::buildChoicesUi( QWidget* page, QVBoxLayout* layout )
{
    auto choices = availableChoices();

    m_choiceGroup = new QButtonGroup( page );
    m_choiceGroup->setExclusive( true );
    m_optionWidgets.clear();

    auto* scrollArea = new QScrollArea( page );
    scrollArea->setWidgetResizable( true );

    auto* container = new QWidget( scrollArea );
    auto* containerLayout = new QVBoxLayout( container );
    containerLayout->setSpacing( 10 );
    containerLayout->setContentsMargins( 0, 0, 0, 0 );

    int addedChoices = 0;

    for ( const DesktopChoice& choice : choices )
    {
        if ( !s_desktops.contains( choice.id ) )
        {
            cWarning() << "de-packages: configuration references unknown desktop" << choice.id;
            continue;
        }

        ++addedChoices;

        auto* frame = new QFrame( container );
        frame->setFrameShape( QFrame::StyledPanel );
        frame->setFrameShadow( QFrame::Plain );
        frame->setStyleSheet( frameStyleSheet( false ) );

        auto* rowLayout = new QHBoxLayout( frame );
        rowLayout->setContentsMargins( 14, 12, 14, 12 );
        rowLayout->setSpacing( 14 );

        QPixmap pixmap = loadScreenshot( choice.screenshot );
        if ( pixmap.isNull() )
        {
            pixmap = placeholderPixmap( choice.name );
        }

        if ( !pixmap.isNull() )
        {
            auto* preview = new QLabel( frame );
            preview->setPixmap( pixmap.scaled( 140, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
            preview->setAlignment( Qt::AlignCenter );
            preview->setMinimumSize( 150, 110 );
            preview->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
            rowLayout->addWidget( preview );
        }

        auto* optionLayout = new QVBoxLayout();
        optionLayout->setSpacing( 6 );

        auto* button = new QRadioButton( choice.name, frame );
        button->setProperty( "choiceId", choice.id );
        optionLayout->addWidget( button );

        m_choiceGroup->addButton( button );
        rowLayout->addLayout( optionLayout, 1 );

        connect(
            button, &QRadioButton::toggled, this, [this, button]( bool checked ) {
                if ( checked )
                {
                    handleSelectionChanged( button->property( "choiceId" ).toString() );
                }
            } );

        OptionWidget widget;
        widget.frame = frame;
        widget.button = button;
        m_optionWidgets.insert( choice.id, widget );

        containerLayout->addWidget( frame );
    }

    if ( addedChoices == 0 )
    {
        auto* warning = new QLabel( tr( "No desktop environments are available." ), container );
        warning->setWordWrap( true );
        containerLayout->addWidget( warning );
        setStatusMessage( tr( "No desktops are configured for installation." ), true );
        setCanProceed( false );
    }

    containerLayout->addStretch();

    scrollArea->setWidget( container );
    layout->addWidget( scrollArea, 1 );
}

void
DePackagesViewStep::handleSelectionChanged( const QString& selectionId )
{
    auto* gs = Calamares::JobQueue::instanceGlobalStorage();
    if ( !gs )
    {
        cWarning() << "de-packages: global storage unavailable";
        setStatusMessage( tr( "Unable to access installer storage." ), true );
        setCanProceed( false );
        return;
    }

    gs->insert( QStringLiteral( "packagechooser_packagechooser" ), selectionId );
    applySelection( selectionId );
}

bool
DePackagesViewStep::applySelection( const QString& selection )
{
    auto* gs = Calamares::JobQueue::instanceGlobalStorage();
    if ( !gs )
    {
        cWarning() << "de-packages: global storage unavailable";
        setStatusMessage( tr( "Unable to access installer storage." ), true );
        setCanProceed( false );
        return false;
    }

    const auto it = s_desktops.constFind( selection );
    if ( it == s_desktops.constEnd() )
    {
        cWarning() << "de-packages: unknown desktop selection" << selection;
        setStatusMessage( tr( "%1 is not a supported desktop choice." ).arg( selection ), true );
        setCanProceed( false );
        return false;
    }

    const DesktopConfig& config = it.value();

    QVariantMap installOperation;
    installOperation.insert( QStringLiteral( "install" ), config.packages );
    QVariantList packageOperations;
    packageOperations.append( installOperation );
    gs->insert( QStringLiteral( "packageOperations" ), packageOperations );

    QFile dmFile( QStringLiteral( "/tmp/calamares-dm" ) );
    if ( dmFile.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
    {
        QTextStream out( &dmFile );
        out << config.displayManager;
        dmFile.close();
    }
    else
    {
        cWarning() << "de-packages: could not write display manager selection" << dmFile.errorString();
    }

    QFile deFile( QStringLiteral( "/tmp/calamares-de" ) );
    if ( deFile.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
    {
        QTextStream out( &deFile );
        out << selection;
        deFile.close();
    }
    else
    {
        cWarning() << "de-packages: could not write desktop selection" << deFile.errorString();
    }

    const QString username = gs->value( QStringLiteral( "username" ) ).toString();
    if ( !username.isEmpty() )
    {
        QFile userFile( QStringLiteral( "/tmp/calamares-user" ) );
        if ( userFile.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
        {
            QTextStream out( &userFile );
            out << username;
            userFile.close();
        }
        else
        {
            cWarning() << "de-packages: could not write username" << userFile.errorString();
        }
    }

    m_lastSelection = selection;
    selectButtonForId( selection );

    cDebug() << "de-packages: selection" << selection;
    cDebug() << "de-packages: packages" << config.packages;
    cDebug() << "de-packages: display manager" << config.displayManager;

    setStatusMessage(
        tr( "%1 will install: %2." ).arg( selection, formatPackages( config.packages ) ),
        false );
    setCanProceed( true );
    return true;
}

void
DePackagesViewStep::selectButtonForId( const QString& selection )
{
    if ( !m_choiceGroup )
    {
        return;
    }

    const bool clearing = selection.isEmpty();
    if ( clearing )
    {
        m_choiceGroup->setExclusive( false );
    }

    const auto buttons = m_choiceGroup->buttons();
    for ( QAbstractButton* button : buttons )
    {
        QSignalBlocker blocker( button );
        const bool match = !clearing && ( button->property( "choiceId" ).toString() == selection );
        button->setChecked( match );
    }

    if ( clearing )
    {
        m_choiceGroup->setExclusive( true );
    }

    updateFrameHighlights( clearing ? QString() : selection );
}

QVector< DesktopChoice >
DePackagesViewStep::availableChoices() const
{
    if ( !m_choices.isEmpty() )
    {
        return m_choices;
    }

    QVector< DesktopChoice > defaults;
    QStringList keys = s_desktops.keys();
    std::sort( keys.begin(), keys.end() );
    defaults.reserve( keys.size() );
    for ( const QString& key : keys )
    {
        DesktopChoice choice;
        choice.id = key;
        choice.name = key;
        defaults.append( choice );
    }
    return defaults;
}

void
DePackagesViewStep::updateFrameHighlights( const QString& selection )
{
    const bool hasSelection = !selection.isEmpty();
    for ( auto it = m_optionWidgets.begin(); it != m_optionWidgets.end(); ++it )
    {
        const bool selected = hasSelection && ( it.key() == selection );
        if ( it.value().frame )
        {
            it.value().frame->setStyleSheet( frameStyleSheet( selected ) );
        }
    }
}

QString
DePackagesViewStep::frameStyleSheet( bool selected ) const
{
    const QColor borderColor = selected && m_frameHighlightColor.isValid() ? m_frameHighlightColor : m_frameBorderColor;
    const int borderWidth = selected ? 2 : 1;
    QString style = QStringLiteral( "QFrame { border-radius: 10px; border: %1px solid %2;" )
                        .arg( borderWidth )
                        .arg( borderColor.isValid() ? borderColor.name() : QStringLiteral( "#808080" ) );
    if ( selected && m_frameHighlightBackground.isValid() )
    {
        style.append(
            QStringLiteral( " background-color: %1;" ).arg( m_frameHighlightBackground.name( QColor::HexArgb ) ) );
    }
    style.append( QStringLiteral( " }" ) );
    return style;
}

void
DePackagesViewStep::setCanProceed( bool enabled )
{
    if ( m_canProceed == enabled )
    {
        return;
    }

    m_canProceed = enabled;
    emit nextStatusChanged( m_canProceed );
}

void
DePackagesViewStep::updateSelection()
{
    auto* gs = Calamares::JobQueue::instanceGlobalStorage();
    if ( !gs )
    {
        cWarning() << "de-packages: global storage unavailable";
        setStatusMessage( tr( "Unable to access installer storage." ), true );
        setCanProceed( false );
        return;
    }

    const QString selection = gs->value( QStringLiteral( "packagechooser_packagechooser" ) ).toString();
    if ( selection.isEmpty() )
    {
        setStatusMessage( tr( "Select a desktop to continue." ), true );
        setCanProceed( false );
        m_lastSelection.clear();
        selectButtonForId( QString() );
        return;
    }

    if ( ( selection == m_lastSelection ) && !m_statusIsError )
    {
        selectButtonForId( selection );
        setCanProceed( true );
        return;
    }

    applySelection( selection );
}

void
DePackagesViewStep::setStatusMessage( const QString& message, bool isError )
{
    m_statusMessage = message;
    m_statusIsError = isError;

    if ( m_statusLabel )
    {
        m_statusLabel->setText( message );
        m_statusLabel->setStyleSheet( isError ? QStringLiteral( "color: #c62828;" )
                                              : QStringLiteral( "color: #2e7d32;" ) );
    }
}

QPixmap
DePackagesViewStep::loadScreenshot( const QString& path ) const
{
    if ( path.isEmpty() )
    {
        return QPixmap();
    }

    const auto* branding = Calamares::Branding::instance();
    if ( branding )
    {
        QFileInfo localFile( branding->componentDirectory() + QLatin1Char( '/' ) + path );
        if ( localFile.exists() && localFile.isFile() )
        {
            QPixmap pixmap( localFile.absoluteFilePath() );
            if ( !pixmap.isNull() )
            {
                return pixmap;
            }
        }

        QPixmap brandedPix = branding->image( path, QSize( 160, 120 ) );
        if ( !brandedPix.isNull() )
        {
            return brandedPix;
        }
    }

    return QPixmap( path );
}

QPixmap
DePackagesViewStep::placeholderPixmap( const QString& label ) const
{
    QPixmap pixmap( 200, 120 );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );
    QColor frameColor = m_frameBorderColor.isValid() ? m_frameBorderColor : QColor( 180, 180, 180 );
    QColor textColor = m_mutedTextColor.isValid() ? m_mutedTextColor : QColor( 80, 80, 80 );

    QRectF rect( 0, 0, pixmap.width(), pixmap.height() );
    painter.setBrush( QColor( frameColor.red(), frameColor.green(), frameColor.blue(), 30 ) );
    painter.setPen( frameColor );
    painter.drawRoundedRect( rect.adjusted( 4, 4, -4, -4 ), 10, 10 );

    painter.setPen( textColor );
    QFont font = painter.font();
    font.setBold( true );
    font.setPointSize( 14 );
    painter.setFont( font );
    painter.drawText( rect, Qt::AlignCenter, label );

    painter.end();
    return pixmap;
}
