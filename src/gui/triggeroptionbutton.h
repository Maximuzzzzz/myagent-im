#ifndef TRIGGEROPTIONBUTTON_H
#define TRIGGEROPTIONBUTTON_H

#include <QToolButton>

class TriggerOptionButton : public QToolButton
{
Q_OBJECT
public:
	TriggerOptionButton(const QString& iconPrefix, const QString& optionName, QWidget* parent = 0);

private slots:
	void updateSettings(bool enableOption);

private:
	QString m_optionName;
};

#endif // TRIGGEROPTIONBUTTON_H
