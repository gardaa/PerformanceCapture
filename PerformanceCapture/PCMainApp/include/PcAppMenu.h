#ifndef PCAPPMENU_H
#define PCAPPMENU_H

#include <QDockWidget>

class QSlider;
class QGroupBox;
class QPushButton;

class PcAppMenu :
    public QDockWidget
{
    Q_OBJECT

signals:
    void calibrationClicked ();
    void synchronisationClicked ();

public slots:
    void onCalibrationClicked ();
    void onSynchronisationClicked ();

public:
    explicit PcAppMenu ( QWidget* iParent = 0 );
    ~PcAppMenu ();

private:
    // Main container
    QWidget*        m_mainWidget;

    // Group boxes
    QGroupBox*      m_settingsWidget;
    QGroupBox*      m_controlsWidget;

    // Buttons
    QPushButton*    m_calibrationButton;
    QPushButton*    m_synchronisationButton;

    // Components
    QSlider*        m_numColumns;
};

#endif // PCAPPMENU_H
