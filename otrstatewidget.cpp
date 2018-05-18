#include "otrstatewidget.h"

#include "definitions/menuicons.h"
#include <definitions/resources.h>
#include <utils/iconstorage.h>
#include <utils/menu.h>

namespace psiotr
{

OtrStateWidget::OtrStateWidget(OtrMessaging* otrc, IMessageWindow *AWindow,
	                           const QString &account, const QString &contact, QWidget *AParent)
	: QToolButton(AParent),
      m_otr(otrc),
      m_account(account),
      m_contact(contact),
      m_authDialog(0)
{
	FWindow = AWindow;

	FMenu = new Menu(this);
	QActionGroup *actionGroup = new QActionGroup(FMenu);
	setMenu(FMenu);

	m_startSessionAction = new Action(FMenu);
    connect(m_startSessionAction, SIGNAL(triggered(bool)),
            this, SLOT(initiateSession(bool)));
	m_startSessionAction->setActionGroup(actionGroup);
	FMenu->addAction(m_startSessionAction);

	m_endSessionAction = new Action(FMenu);
	m_endSessionAction->setText(tr("&End private conversation"));
    connect(m_endSessionAction, SIGNAL(triggered(bool)),
            this, SLOT(endSession(bool)));
	m_endSessionAction->setActionGroup(actionGroup);
	FMenu->addAction(m_endSessionAction);

	FMenu->insertSeparator(NULL);

	m_authenticateAction = new Action(FMenu);
	m_authenticateAction->setText(tr("&Authenticate contact"));
    connect(m_authenticateAction, SIGNAL(triggered(bool)),
            this, SLOT(authenticateContact(bool)));
	m_authenticateAction->setActionGroup(actionGroup);
	FMenu->addAction(m_authenticateAction);

	m_sessionIdAction = new Action(FMenu);
	m_sessionIdAction->setText(tr("Show secure session &ID"));
    connect(m_sessionIdAction, SIGNAL(triggered(bool)),
            this, SLOT(sessionID(bool)));
	m_sessionIdAction->setActionGroup(actionGroup);
	FMenu->addAction(m_sessionIdAction);

	m_fingerprintAction = new Action(FMenu);
	m_fingerprintAction->setText(tr("Show own &fingerprint"));
    connect(m_fingerprintAction, SIGNAL(triggered(bool)),
            this, SLOT(fingerprint(bool)));
	m_fingerprintAction->setActionGroup(actionGroup);
	FMenu->addAction(m_fingerprintAction);

    setToolTip(tr("OTR Messaging"));

	connect(FWindow->address()->instance(),SIGNAL(addressChanged(const Jid &, const Jid &)),SLOT(onWindowAddressChanged(const Jid &, const Jid &)));

	updateMessageState()
}

OtrStateWidget::~OtrStateWidget()
{

}

void OtrStateWidget::onWindowAddressChanged(const Jid &AStreamBefore, const Jid &AContactBefore)
{
	Q_UNUSED(AStreamBefore); Q_UNUSED(AContactBefore);
	updateMessageState();
}

void OtrStateWidget::updateMessageState()
{
    QString iconKey;
    OtrMessageState state = m_otr->getMessageState(m_account, m_contact);

    QString stateString(m_otr->getMessageStateString(m_account,
                                                     m_contact));

    if (state == OTR_MESSAGESTATE_ENCRYPTED)
    {
        if (m_otr->isVerified(m_account, m_contact))
        {
            iconKey = MNI_OTR_ENCRYPTED;
        }
        else
        {
            iconKey = MNI_OTR_UNVERFIFIED;
            stateString += ", " + tr("unverified");
        }
    }
    else
    {
        iconKey = MNI_OTR_NO;
    }

    setText(tr("OTR Messaging [%1]").arg(stateString));
    IconStorage::staticStorage(RSR_STORAGE_MENUICONS)->insertAutoIcon(this,iconKey);

    if (state == OTR_MESSAGESTATE_ENCRYPTED)
    {
        m_startSessionAction->setText(tr("Refre&sh private conversation"));
        m_authenticateAction->setEnabled(true);
        m_sessionIdAction->setEnabled(true);
        m_endSessionAction->setEnabled(true);
    }
    else
    {
        m_startSessionAction->setText(tr("&Start private conversation"));
        if (state == OTR_MESSAGESTATE_PLAINTEXT)
        {
            m_authenticateAction->setEnabled(false);
            m_sessionIdAction->setEnabled(false);
            m_endSessionAction->setEnabled(false);
        }
        else // finished, unknown
        {
            m_endSessionAction->setEnabled(true);
            m_authenticateAction->setEnabled(false);
            m_sessionIdAction->setEnabled(false);
        }
    }

    if (m_otr->getPolicy() < OTR_POLICY_ENABLED)
    {
        m_startSessionAction->setEnabled(false);
        m_endSessionAction->setEnabled(false);
    }
}

//-----------------------------------------------------------------------------

void OtrStateWidget::initiateSession(bool b)
{
    Q_UNUSED(b);
    m_otr->startSession(m_account, m_contact);
}

//-----------------------------------------------------------------------------

void OtrStateWidget::authenticateContact(bool)
{
    if (m_authDialog || !encrypted())
    {
        return;
    }

    m_authDialog = new AuthenticationDialog(m_otr,
                                            m_account, m_contact,
                                            QString(), true);

    connect(m_authDialog, SIGNAL(destroyed()),
            this, SLOT(finishAuth()));

    m_authDialog->show();
}

//-----------------------------------------------------------------------------

void OtrStateWidget::receivedSMP(const QString& question)
{
    if ((m_authDialog && !m_authDialog->finished()) || !encrypted())
    {
        m_otr->abortSMP(m_account, m_contact);
        return;
    }
    if (m_authDialog)
    {
        disconnect(m_authDialog, SIGNAL(destroyed()),
                   this, SLOT(finishAuth()));
        finishAuth();
    }

    m_authDialog = new AuthenticationDialog(m_otr, m_account, m_contact, question, false);

    connect(m_authDialog, SIGNAL(destroyed()),
            this, SLOT(finishAuth()));

    m_authDialog->show();
}

//-----------------------------------------------------------------------------

void OtrStateWidget::updateSMP(int progress)
{
    if (m_authDialog)
    {
        m_authDialog->updateSMP(progress);
        m_authDialog->show();
    }
}

//-----------------------------------------------------------------------------

void OtrStateWidget::finishAuth()
{
    m_authDialog = 0;

    updateMessageState();
}

//-----------------------------------------------------------------------------

void OtrStateWidget::sessionID(bool)
{
    QString sId = m_otr->getSessionId(m_account, m_contact);
    QString msg;

    if (sId.isEmpty())
    {
        msg = tr("No active encrypted session");
    }
    else
    {
        msg = tr("Session ID between account \"%1\" and %2: %3")
                .arg(m_otr->humanAccount(m_account))
                .arg(m_contact)
                .arg(sId);
    }

    m_otr->displayOtrMessage(m_account, m_contact, msg);
}

//-----------------------------------------------------------------------------

void OtrStateWidget::endSession(bool b)
{
    Q_UNUSED(b);
    m_otr->endSession(m_account, m_contact);
    updateMessageState();
}

//-----------------------------------------------------------------------------

void OtrStateWidget::fingerprint(bool)
{
    QString fingerprint = m_otr->getPrivateKeys()
                                    .value(m_account,
                                           tr("No private key for account \"%1\"")
                                             .arg(m_otr->humanAccount(m_account)));

    QString msg(tr("Fingerprint for account \"%1\": %2")
                   .arg(m_otr->humanAccount(m_account))
                   .arg(fingerprint));

    m_otr->displayOtrMessage(m_account, m_contact, msg);
}

//-----------------------------------------------------------------------------

bool OtrStateWidget::encrypted() const
{
    return m_otr->getMessageState(m_account, m_contact) ==
           OTR_MESSAGESTATE_ENCRYPTED;
}
//-----------------------------------------------------------------------------

} // namespace