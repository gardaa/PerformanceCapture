#include "PcAppMenu.h"

#include <QSlider>
#include <QLabel>
#include <QLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QButtonGroup>
#include <QPushButton>

#include "PcParameterHandler.h"

using namespace pcm;

PcAppMenu::PcAppMenu ( QWidget* iParent )
    :   QDockWidget ( "Parameters", iParent )
{
    PcParameterHandler& params = PcParameterHandler::Instance ();

    m_mainWidget = new QWidget ( this );
    m_settingsWidget = new QGroupBox ( "Settings", this );
    m_controlsWidget = new QGroupBox ( "Controls", this );
    
    m_calibrationButton = new QPushButton ( "Calibrate", m_controlsWidget );
    m_synchronisationButton = new QPushButton ( "Synchronise", m_controlsWidget );

    QVBoxLayout* mainLayout = new QVBoxLayout ( m_mainWidget );
    QFormLayout* settingsLayout = new QFormLayout ( m_settingsWidget );
    QHBoxLayout* controlsLayout = new QHBoxLayout ( m_controlsWidget );
    QSizePolicy sizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
    
    // Component instantiation
    m_numColumns = new QSlider ( Qt::Horizontal, this );

    // Label creation
    
    // Wiring
    connect (
        m_numColumns, SIGNAL ( valueChanged    ( int ) ),
             &params, SLOT   ( SetViewportCols ( int ) )
    );
    connect (
        m_calibrationButton, SIGNAL ( clicked              () ),
                       this, SLOT   ( onCalibrationClicked () )
    );

    // Component setup
    m_numColumns->setSliderPosition ( params.GetViewportCols () );
    m_numColumns->setMaximum ( 4 );
    m_numColumns->setMinimum ( 1 );
    m_numColumns->setSingleStep ( 1 );

    sizePolicy.setHorizontalStretch ( 0 );
    sizePolicy.setVerticalStretch ( 0 );
    m_settingsWidget->setSizePolicy ( sizePolicy );
    m_controlsWidget->setSizePolicy ( sizePolicy );

    settingsLayout->addRow ( tr("Columns"), m_numColumns );
    controlsLayout->addWidget ( m_calibrationButton );
    controlsLayout->addWidget ( m_synchronisationButton );

    mainLayout->addWidget ( m_settingsWidget );
    mainLayout->addWidget ( m_controlsWidget );
    mainLayout->addStretch ( 0 );

    setFeatures ( QDockWidget::NoDockWidgetFeatures );
    setWidget ( m_mainWidget );
}

void PcAppMenu::onCalibrationClicked ()
{
    emit calibrationClicked ();
}

void PcAppMenu::onSynchronisationClicked ()
{
    emit synchronisationClicked ();
}