/*
* qtObjectAgent.cpp
*/
#include <qtimer.h>
#include "../include/qtObjectAgent.h"

CQtObjectAgent::CQtObjectAgent(QObject *parent)
	: QObject(parent)
{

}

CQtObjectAgent::~CQtObjectAgent()
{
	Close();
}

BOOL CQtObjectAgent::Create(QThread* pServerObj)
{
	m_pThreadObj = pServerObj;

	return OnInit();
}

BOOL CQtObjectAgent::OnInit()
{
	moveToThread(m_pThreadObj);
	connect(this, SIGNAL(SignalActive()), this, SLOT(OnSignalActive()));
	
	return TRUE;
}

void CQtObjectAgent::Close()
{

}

void CQtObjectAgent::Active()
{
	emit SignalActive();
}


void CQtObjectAgent::OnSignalActive()
{
	OnActive();
}

CQtExitAgent::CQtExitAgent(QObject *parent):CQtObjectAgent(parent)
{
}

CQtExitAgent::~CQtExitAgent()
{
	Close();
}


BOOL CQtExitAgent::Create(QThread* pServerObj)
{
	return CQtObjectAgent::Create(pServerObj);
}

void CQtExitAgent::Close()
{
	CQtObjectAgent::Close();
}

BOOL CQtExitAgent::OnInit()
{
	return CQtObjectAgent::OnInit();
}

void CQtExitAgent::OnActive()
{
	OnExit();
	m_pThreadObj->quit();
}

void CQtExitAgent::OnExit()
{

}

CQtTimeAgent::CQtTimeAgent(QObject *parent):CQtObjectAgent(parent)
{
	m_pTimer = NULL;
}

CQtTimeAgent::~CQtTimeAgent()
{
	Close();
}

BOOL CQtTimeAgent::Create(QThread* pServerObj, int timeoutMs)
{
	m_timeout = timeoutMs;
	return CQtObjectAgent::Create(pServerObj);
}

void CQtTimeAgent::Close()
{

}

void CQtTimeAgent::InActive()
{
	emit SignalInactive();
}

BOOL CQtTimeAgent::OnInit()
{
	if (!CQtObjectAgent::OnInit())
		return FALSE;

	
	return TRUE;
}

void CQtTimeAgent::OnActive()
{
	m_pTimer = new QTimer(this);

	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnSignalTimeout()));
	connect(this, SIGNAL(SignalInactive()), this, SLOT(OnSignalInActive()));
	m_pTimer->setInterval(m_timeout);
	m_pTimer->start();
}

void CQtTimeAgent::OnTimeout()
{

}

void CQtTimeAgent::OnSignalTimeout()
{
	OnTimeout();
}


void CQtTimeAgent::OnSignalInActive()
{
	OnInActive();
}

void CQtTimeAgent::OnInActive()
{
	m_pTimer->stop();
}
