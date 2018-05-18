/*
 * psiotrclosure.h
 *
 * Off-the-Record Messaging plugin for Psi+
 * Copyright (C) 2007-2011  Timo Engel (timo-e@freenet.de)
 *               2011-2012  Florian Fieber
 *
 * This program was originally written as part of a diplom thesis
 * advised by Prof. Dr. Ruediger Weis (PST Labor)
 * at the Technical University of Applied Sciences Berlin.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSIOTRCLOSURE_H_
#define PSIOTRCLOSURE_H_

#include "otrmessaging.h"

#include <QObject>
#include <QDialog>
#include <QMessageBox>

class QAction;
class QMenu;
class QComboBox;
class QLineEdit;
class QProgressBar;
class QPushButton;

namespace psiotr
{

//-----------------------------------------------------------------------------

class AuthenticationDialog : public QDialog
{
    Q_OBJECT
public:
    AuthenticationDialog(OtrMessaging* otrc,
                         const QString& account, const QString& contact,
                         const QString& question, bool sender,
                         QWidget* parent = 0);
    ~AuthenticationDialog();

    void reset();
    bool finished();
    void updateSMP(int progress);
    void notify(const QMessageBox::Icon icon, const QString& message);

public slots:
    void reject();

private:
    enum AuthState {AUTH_READY, AUTH_IN_PROGRESS, AUTH_FINISHED};
    enum Method {METHOD_QUESTION, METHOD_SHARED_SECRET, METHOD_FINGERPRINT};

    OtrMessaging* m_otr;
    Method        m_method;
    QString       m_account;
    QString       m_contact;
    QString       m_contactName;
    bool          m_isSender;
    AuthState     m_state;
    Fingerprint   m_fpr;

    QWidget*      m_methodWidget[3];
    QComboBox*    m_methodBox;
    QLineEdit*    m_questionEdit;
    QLineEdit*    m_answerEdit;
    QLineEdit*    m_sharedSecretEdit;
    QProgressBar* m_progressBar;
    QPushButton*  m_cancelButton;
    QPushButton*  m_startButton;

private slots:
    void changeMethod(int index);
    void checkRequirements();
    void startAuthentication();
};

//-----------------------------------------------------------------------------

} // namespace psiotr

#endif
