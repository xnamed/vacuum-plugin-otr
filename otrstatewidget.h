#ifndef OTRSATEWIDGET_H
#define OTRSATEWIDGET_H

#include <QLabel>
#include <QToolButton>
#include <interfaces/imessagewidgets.h>

#include "otrmessaging.h"
#include "psiotrclosure.h"

namespace psiotr
{

class AuthenticationDialog;

class OtrStateWidget :
	public QToolButton
{
	Q_OBJECT;
public:
	OtrStateWidget(OtrMessaging* otrc, IMessageWindow *AWindow,
		         const QString &account, const QString &contact, QWidget *AParent);
	~OtrStateWidget();
    bool encrypted() const;
    void receivedSMP(const QString& question);
    void updateSMP(int progress);
protected slots:
	void onWindowAddressChanged(const Jid &AStreamBefore, const Jid &AContactBefore);
	void updateMessageState();
private:
    OtrMessaging* m_otr;
    QString       m_account;
    QString       m_contact;
	IMessageWindow *FWindow;
private:
	Menu *FMenu;
    Action*       m_authenticateAction;
    Action*       m_sessionIdAction;
    Action*       m_fingerprintAction;
    Action*       m_startSessionAction;
    Action*       m_endSessionAction;
    AuthenticationDialog* m_authDialog;
public slots:
    void initiateSession(bool b);
    void endSession(bool b);
    void authenticateContact(bool b);
    void sessionID(bool b);
    void fingerprint(bool b);
    void finishAuth();

};

} // namespace psiotr

#endif // OTRSATEWIDGET_H
