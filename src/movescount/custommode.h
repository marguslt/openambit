#ifndef CUSTOMMODE_H
#define CUSTOMMODE_H

#include <QObject>
#include <QVariantMap>
#include <QMap>
#include <QList>

#include "libambit.h"

QMap<int, int> qmapInit();
static const QMap<int, int> movescount2ambitConverter = qmapInit();

class CustomModeDisplay : public QObject
{
    Q_OBJECT
public:
    explicit CustomModeDisplay(QVariantMap &displayMap, QObject *parent = 0);
    CustomModeDisplay(const CustomModeDisplay &other);

    CustomModeDisplay& operator=(const CustomModeDisplay &rhs);

    uint serializeDisplayLayout(u_int8_t *data);
    uint serializeRow(u_int16_t row, u_int8_t *data);
    uint serializeRowEntry(u_int16_t row_nbr, u_int8_t *data);

    QString toString();

private:
    u_int16_t ambitDisplayType();

    bool requiresHRBelt;
    int row1;
    int row2;
    int type;
    QList<int> views;


    static const QString REQUIRES_HR_BELT;
    static const QString ROW_1;
    static const QString ROW_2;
    static const QString TYPE;
    static const QString VIEWS;

    static const int HEADER_SIZE = 4;
};

class CustomMode : public QObject
{
    Q_OBJECT
public:
    explicit CustomMode(QVariantMap &customModeMap, QObject *parent = 0);
    CustomMode(const CustomMode &other);

    CustomMode& operator=(const CustomMode &rhs);

    uint getCustomModeId() const;

    uint serialize(u_int8_t *data);

    QString toString();
signals:

public slots:

private:
    uint serializeSettings(u_int8_t *data);
    void serializeName(ambit_custom_mode_settings_s *settings);
    void serializeHeader(u_int16_t header_nbr, u_int16_t length, u_int8_t *dataWrite);
    uint serializeDisplays(u_int8_t *data);
    uint serializeDisplay(CustomModeDisplay display, u_int8_t *data);
    u_int16_t hrbeltAndPods();

    uint activityId;
    uint altiBaroMode;
//    uint autoPause;
    uint autolapDistance;
    uint gpsInterval;
    uint hrLimitHigh;
    uint hrLimitLow;
    uint interval1distance;
    uint interval2distance;
    uint interval1time;
    uint interval2time;
    uint intervalRepetitions;
    QString activityName;
    uint recordingInterval;
//    uint tonesMode;
    bool useAccelerometer;
    bool useAutolap;
//    bool useAutomaticLogRecording;
    bool useBikePod;
    bool useCadencePod;
    bool useFootPod;
    bool usePowerPod;
    bool useHrBelt;
    bool useHrLimits;
    bool useIntervals;
    uint custommodeId;
    uint autoScrolingSpeed;
    float autoPauseSpeed;
    uint backlightMode;
    uint displayIsNegative;
    uint showNavigationSelection;
    QList<CustomModeDisplay> displays;

    QList<CustomModeDisplay> unknownDisplays;

public:
    static const QString ACTIVITY_ID;
    static const QString ALTI_BARO_MODE;
    static const QString AUTOLAP_DISTANCE;
    static const QString GPS_INTERVAL;
    static const QString HR_LIMIT_HIGH;
    static const QString HR_LIMIT_LOW;
    static const QString INTERVAL_1_DISTANCE;
    static const QString INTERVAL_2_DISTANCE;
    static const QString INTERVAL_1_TIME;
    static const QString INTERVAL_2_TIME;
    static const QString INTERVAL_REPETITIONS;
    static const QString ACTIVITY_NAME;
    static const QString RECORDING_INTERVAL;
    static const QString USE_ACCELEROMETER;
    static const QString USE_AUTOLAP;
    static const QString USE_BIKE_POD;
    static const QString USE_CADENS_POD;
    static const QString USE_FOOT_POD;
    static const QString USE_POWER_POD;
    static const QString USE_HR_BELT;
    static const QString USE_HR_LIMITS;
    static const QString USE_INTERVALS;
    static const QString CUSTOM_MODE_ID;
    static const QString AUTO_SCROLING_SPEED;
    static const QString AUTO_PAUSE_SPEED;
    static const QString BACKLIGHT_MODE;
    static const QString DISPLAY_IS_NEGATIVE;
    static const QString SHOW_NAVIGATION_SELECTION;

    static const int HEADER_SIZE = 4;
    static const int NAME_SIZE = 16;
    static const int SETTINGS_SIZE = 90;

    static const int START_HEADER = 0x0100;
    static const int CUSTOM_MODE_HEADER = 0x0101;
    static const int SETTINGS_HEADER = 0x0102;

    static const u_int8_t UNKNOWN_DISPLAYES[];
};

#endif // CUSTOMMODE_H
