#ifndef PCMAINAPPMENU_H
#define PCMAINAPPMENU_H

#include <QDockWidget>

class QSlider;
class QGroupBox;
class QPushButton;

class PCMainAppMenu :
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
    explicit PCMainAppMenu ( QWidget* iParent = 0 );
    ~PCMainAppMenu ();

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

#endif // PCMAINAPPMENU_H
