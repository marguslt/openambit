#ifndef MOVESCOUNTSETTINGS_H
#define MOVESCOUNTSETTINGS_H

#include <QObject>
#include <QList>

#include "custommode.h"
#include "custommodegroup.h"

class MovescountSettings : public QObject
{
    Q_OBJECT
public:
    explicit MovescountSettings(QObject *parent = 0);

    void parse(QVariantMap settingsMap);
    MovescountSettings(const MovescountSettings &other);

    MovescountSettings& operator=(MovescountSettings &rhs);

    void toAmbitData(ambit_device_settings_t *ambitSettings);
signals:

public slots:

private:
    QList<CustomModeGroup> customModeGroups;
    QList<CustomMode> customModes;
};

#endif // MOVESCOUNTSETTINGS_H
