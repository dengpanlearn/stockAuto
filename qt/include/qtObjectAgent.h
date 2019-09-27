/*
*qtObjectAgent.h
*qt Object »ùÀà
*/

#include <dp.h>
#include <QObject>
#include <qthread.h>
#include <dpAtomic.h>


class CQtObjectAgent : public QObject
{
	Q_OBJECT

public:
	CQtObjectAgent(QObject *parent = NULL);
	virtual ~CQtObjectAgent();

	BOOL Create(QThread* pServerObj);
	void Close();

public:
	void Active();

protected:
	virtual BOOL OnInit();
	virtual void OnActive()=0;
	;

signals:
	void SignalActive();

private slots:
	void OnSignalActive();

protected:
	QThread*	m_pThreadObj;
};

class CQtExitAgent : public CQtObjectAgent
{
	Q_OBJECT

public:
	CQtExitAgent(QObject *parent=NULL);
	virtual ~CQtExitAgent();

	BOOL Create(QThread* pServerObj);
	void Close();

protected:
	virtual BOOL OnInit();
	virtual void OnActive();
	virtual void OnExit();
};

class QTimer;
class CQtTimeAgent : public CQtObjectAgent
{
	Q_OBJECT

public:
	CQtTimeAgent(QObject *parent = NULL);
	virtual ~CQtTimeAgent();

	BOOL Create(QThread* pServerObj, int timeoutMs);
	void Close();

protected:
	virtual BOOL OnInit();
	virtual void OnActive();
	virtual void OnTimeout();
	
private slots:
	void OnSignalTimeout();

private:
	QTimer*	m_pTimer;
	int		m_timeout;
};