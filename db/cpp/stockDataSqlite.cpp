/*
*stockDataSqlite.cpp*/

#include <qdir.h>
#include <qtGlobal.h>
#include <qtextcodec.h>
#include <qsqldatabase.h>
#include <qvariant.h>
#include <qsqlquery.h>
#include <qstring.h>
#include "../include/stockDataSqlite.h"


BOOL CStockDataSqlite::Init(const char* dbDir, const char* pListName, const char* pKLineName, const char* pTraceName)
{
	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QString dirPath = pTextCode->toUnicode(dbDir);
	QDir dir(dirPath);

	QString listFile = dir.absoluteFilePath(pTextCode->toUnicode(pListName));
	QString klineFile = dir.absoluteFilePath(pTextCode->toUnicode(pKLineName));

	m_listDb = QSqlDatabase::addDatabase("QSQLITE", "stocklist");
	m_listDb.setDatabaseName(listFile);


	m_klineDb = QSqlDatabase::addDatabase("QSQLITE", "kline");
	m_klineDb.setDatabaseName(klineFile);

	m_traceLogDb = QSqlDatabase::addDatabase("QSQLITE", "tracelog");
	m_traceLogDb.setDatabaseName(pTraceName);

	m_listDb.open();
	m_klineDb.open();
	m_traceLogDb.open();
	return TRUE;
}

void CStockDataSqlite::Close()
{
	m_listDb.close();
	m_klineDb.close();
}

BOOL CStockDataSqlite::StockListIsOn()
{
	return m_listDb.isOpen();
}

int CStockDataSqlite::GetStockList(STOCK_CODE_NAME* pListBuf, int bufCounts)
{
	if (!m_listDb.isOpen())
		return -1;

	QSqlQuery sqlQuery(m_listDb);
	QString selSql("select symbol, name from code");
	if (!sqlQuery.exec(selSql))
	{
		return -1;
	}

	int stockCounts = 0;
	while (sqlQuery.next() && bufCounts--)
	{
		QString code= sqlQuery.value(0).toString();
		QString name = sqlQuery.value(1).toString();

		QString2Char(code, pListBuf->code);
		QString2Char(name, pListBuf->name);
		stockCounts++;
		pListBuf++;
	}

	return stockCounts;
}

BOOL CStockDataSqlite::TraceLogIsOn()
{
	return m_traceLogDb.isOpen();
}

int CStockDataSqlite::GetTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf, int bufCounts)
{
	if (!m_traceLogDb.isOpen())
		return -1;

	QSqlQuery sqlQuery(m_traceLogDb);
	QString selSql("select step, symbol, hightime, highval, buytime, buyval,  selltime, sellval, lasttime from trace");
	if (!sqlQuery.exec(selSql))
	{
		return -1;
	}

	int logCounts = 0;
	while (sqlQuery.next() && bufCounts--)
	{
		pTraceLogBuf->traceStep = sqlQuery.value(0).toInt();

		QString code = sqlQuery.value(1).toString();
		QString2Char(code, pTraceLogBuf->code);

		pTraceLogBuf->highTime = sqlQuery.value(2).toInt();
		pTraceLogBuf->fHighVal = sqlQuery.value(3).toFloat();

		pTraceLogBuf->buyTime = sqlQuery.value(4).toInt();
		pTraceLogBuf->fBuyVal = sqlQuery.value(5).toFloat();

		pTraceLogBuf->sellTime = sqlQuery.value(6).toInt();
		pTraceLogBuf->fSellVal = sqlQuery.value(7).toFloat();

		pTraceLogBuf->lastTime = sqlQuery.value(8).toInt();
		logCounts++;
		pTraceLogBuf++;
	}

	return logCounts;
}