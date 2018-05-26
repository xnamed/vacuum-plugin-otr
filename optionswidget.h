#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <definitions/optionvalues.h>
#include <interfaces/inotifications.h>
#include <interfaces/ioptionsmanager.h>
#include <utils/options.h>

class OptionsWidget :
	public QWidget,
	public IOptionsDialogWidget
{
	Q_OBJECT;
	Q_INTERFACES(IOptionsDialogWidget);
public:
	OptionsWidget(QWidget *optpage, QWidget *AParent);
	~OptionsWidget();
	virtual QWidget* instance() { return this; }
	public slots:
		virtual void apply();
		virtual void reset();
signals:
		void modified();
		void childApply();
		void childReset();
};

#endif // OPTIONSWIDGET_H
