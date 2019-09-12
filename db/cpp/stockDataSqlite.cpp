/*
*stockDataSqlite.cpp*/

#include <qdir.h>
#include <qtGlobal.h>
#include <qtextcodec.h>
#include <qsqldatabase.h>
#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstring.h>
#include "../include/stockDataSqlite.h"


BOOL CStockDataSqlite::Init(const char* dbDir, const char* pListName, const char* pKLineName, const char* pTraceName)
{
	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QString dirPath = pTextCode->toUnicode(dbDir);
	QDir dir(dirPath);

	QString listFile = dir.absoluteFilePath(pTextCode->toUnicode(pListName));
	QString klineFile = dir.absoluteFilePath(pTextCode->toUnicode(pKLineName));
	QString traceFile = dir.absoluteFilePath(pTextCode->toUnicode(pTraceName));
	m_listDb = QSqlDatabase::addDatabase("QSQLITE", "stocklist");
	m_listDb.setDatabaseName(listFile);


	m_klineDb = QSqlDatabase::addDatabase("QSQLITE", "kline");
	m_klineDb.setDatabaseName(klineFile);

	m_traceLogDb = QSqlDatabase::addDatabase("QSQLITE", "tracelog");
	m_traceLogDb.setDatabaseName(traceFile);

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
	QString selSql("select step, code, hightime, highval, buytime, buyval,  selltime, sellval, histime, updatetime, realtime, raisebalances from trace");
	
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

		pTraceLogBuf->hisTime = sqlQuery.value(8).toInt();
		pTraceLogBuf->updateTime = sqlQuery.value(9).toInt();
		pTraceLogBuf->realTime = sqlQuery.value(10).toInt();
		pTraceLogBuf->raiseBalanceCheckTimes = sqlQuery.value(11).toInt();

		logCounts++;
		pTraceLogBuf++;
	}

	return logCounts;
}

int CStockDataSqlite::UpdateTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf)
{
	if (!m_traceLogDb.isOpen())
		return -1;
	QTextCodec* pTextCode = QTextCodec::codecForLocale();

	QSqlQuery sqlQuery(m_traceLogDb);

	QString selSql = QString("select code from trace where code =\'%1\'").arg(pTextCode->toUnicode(pTraceLogBuf->code));

	if (!sqlQuery.exec(selSql))
		return -1;

	if (!sqlQuery.next())
	{
		QString insertSql = QString("insert into trace values(?, ?, ?, ?, ?, ?,?, ?, ?, ?, ?, ?)");
		sqlQuery.prepare(insertSql);
	}
	else
	{
		QString updateSql = QString("update trace set step=?, code=?, hightime=?, highval=?, buytime=?, buyval=?,  selltime=?, sellval=?, histime=?, updatetime=?, realtime=?, raisebalances=? where code=\'%1\'").arg(pTextCode->toUnicode(pTraceLogBuf->code));
		sqlQuery.prepare(updateSql);
	}

	sqlQuery.bindValue(0, pTraceLogBuf->traceStep);
	sqlQuery.bindValue(1, pTextCode->toUnicode(pTraceLogBuf->code));
	sqlQuery.bindValue(2, pTraceLogBuf->highTime);
	sqlQuery.bindValue(3, pTraceLogBuf->fHighVal);
	sqlQuery.bindValue(4, pTraceLogBuf->buyTime);
	sqlQuery.bindValue(5, pTraceLogBuf->fBuyVal);
	sqlQuery.bindValue(6, pTraceLogBuf->sellTime);
	sqlQuery.bindValue(7, pTraceLogBuf->fSellVal);
	sqlQuery.bindValue(8, pTraceLogBuf->hisTime);
	sqlQuery.bindValue(9, pTraceLogBuf->updateTime);
	sqlQuery.bindValue(10, pTraceLogBuf->realTime);
	sqlQuery.bindValue(11, pTraceLogBuf->raiseBalanceCheckTimes);
	
	return sqlQuery.exec() ? 0 : -1;
}

int CStockDataSqlite::GetHisKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pHisKLineBuf, int bufCounts)
{
	if (!m_klineDb.isOpen())
		return -1;

	QSqlQuery sqlQuery(m_klineDb);
	QString selSql("select * from (select open, close, high, low, rsi, ma10, percent, timestamp from %1 order by timestamp desc limit %2) aa order by timestamp");
	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QString tab = pTextCode->toUnicode(pStockCode);

	selSql = selSql.arg(tab).arg(bufCounts);
	if (!sqlQuery.exec(selSql))
	{
		return -1;
	}

	int hisCounts = 0;
	while (sqlQuery.next() && bufCounts--)
	{
		pHisKLineBuf->fOpen = sqlQuery.value(0).toFloat();
		pHisKLineBuf->fClose = sqlQuery.value(1).toFloat();
		pHisKLineBuf->fHigh = sqlQuery.value(2).toFloat();
		pHisKLineBuf->fLow = sqlQuery.value(3).toFloat();
		pHisKLineBuf->fRsi7 = sqlQuery.value(4).toFloat();
		pHisKLineBuf->fMa10 = sqlQuery.value(5).toFloat();
		pHisKLineBuf->fPercent = sqlQuery.value(6).toFloat();
		pHisKLineBuf->timeVal = sqlQuery.value(7).toInt();
		QString code = sqlQuery.value(1).toString();
	

		hisCounts++;
		pHisKLineBuf++;
	}

	return hisCounts;
}