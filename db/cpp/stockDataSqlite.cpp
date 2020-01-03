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


BOOL CStockDataSqlite::Init(const char* dbDir, const char* pListName, const char* pKLineName, const char* pTraceName, const char* pRecordName)
{
	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QString dirPath = pTextCode->toUnicode(dbDir);
	QDir dir(dirPath);

	QString listFile = dir.absoluteFilePath(pTextCode->toUnicode(pListName));
	QString klineFile = dir.absoluteFilePath(pTextCode->toUnicode(pKLineName));
	QString traceFile = dir.absoluteFilePath(pTextCode->toUnicode(pTraceName));
	QString traceRecordFile = dir.absoluteFilePath(pTextCode->toUnicode(pRecordName));
	m_listDb = QSqlDatabase::addDatabase("QSQLITE", "stocklist");
	m_listDb.setDatabaseName(listFile);


	m_klineDb = QSqlDatabase::addDatabase("QSQLITE", "kline");
	m_klineDb.setDatabaseName(klineFile);

	m_traceLogDb = QSqlDatabase::addDatabase("QSQLITE", "tracelog");
	m_traceLogDb.setDatabaseName(traceFile);

	m_traceRecordDb = QSqlDatabase::addDatabase("QSQLITE", "traceRecord");
	m_traceRecordDb.setDatabaseName(traceRecordFile);

	m_listDb.open();
	m_klineDb.open();
	m_traceLogDb.open();
	m_traceRecordDb.open();
	return TRUE;
}

void CStockDataSqlite::Close()
{
	m_listDb.close();
	m_klineDb.close();
	m_traceLogDb.close();
	m_traceRecordDb.close();
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
	if (!m_traceLogDb.isOpen())
		return FALSE;

	if (m_traceLogDb.tables().contains("trace"))
		return TRUE;

	QSqlQuery sqlQuery(m_traceLogDb);
	QString createTab = QString("create table trace (step int, code varchar(9), hightime int, highval decimal, buytime int, buyval decimal, selltime int, sellval decimal, histime int, updatetime int, realtime int, raisebalances int, toptime int, topval decimal)");
	return sqlQuery.exec(createTab);
}

BOOL CStockDataSqlite::TraceRecordIsOn()
{
	if (!m_traceRecordDb.isOpen())
		return FALSE;

	if (m_traceRecordDb.tables().contains("traceRecord"))
		return TRUE;

	QSqlQuery sqlQuery(m_traceRecordDb);
	QString createTab = QString("create table traceRecord (code varchar(9), hightime int, highval decimal, buytime int, buyval decimal, selltime int, sellval decimal)");
	return sqlQuery.exec(createTab);
}

int CStockDataSqlite::GetTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf, int bufCounts)
{
	if (!m_traceLogDb.isOpen())
		return -1;

	QSqlQuery sqlQuery(m_traceLogDb);
	QString selSql("select step, code, hightime, highval, buytime, buyval,  selltime, sellval, histime, updatetime, realtime, raisebalances, toptime, topval from trace");
	
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
		pTraceLogBuf->topTime = sqlQuery.value(12).toInt();
		pTraceLogBuf->fTopVal = sqlQuery.value(13).toFloat();
		logCounts++;
		pTraceLogBuf++;
	}

	return logCounts;
}

int CStockDataSqlite::ClearTraceLogHisTime()
{
	if (!m_traceLogDb.isOpen())
		return -1;


	QSqlQuery sqlQuery(m_traceLogDb);

	QString updateSql = QString("update trace set step=1, histime = 0 where step < 4;");

	return sqlQuery.exec(updateSql) ? 0 : -1;
}

int CStockDataSqlite::UpdateTraceLog(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf)
{
	if (!m_traceLogDb.isOpen())
		return -1;
	QTextCodec* pTextCode = QTextCodec::codecForLocale();

	QSqlQuery sqlQuery(m_traceLogDb);

	QString selSql = QString("select code from trace where code =\'%1\'").arg(pTextCode->toUnicode(pTraceLogBuf->code));

	if (!sqlQuery.exec(selSql))
	{
		if (m_traceLogDb.tables().contains("trace"))
			return -1;

		QString createTab = QString("create table trace (step int, code varchar(9), hightime int, highval decimal, buytime int, buyval decimal, selltime int, sellval decimal, histime int, updatetime int, realtime int, raisebalances int, toptime int, topval decimal)");
		if (!sqlQuery.exec(createTab))
			return -1;
	}
		

	if (!sqlQuery.next())
	{
		QString insertSql = QString("insert into trace values(?, ?, ?, ?, ?, ?,?, ?, ?, ?, ?, ?, ?, ?)");
		sqlQuery.prepare(insertSql);
	}
	else
	{
		QString updateSql = QString("update trace set step=?, code=?, hightime=?, highval=?, buytime=?, buyval=?,  selltime=?, sellval=?, histime=?, updatetime=?, realtime=?, raisebalances=?, toptime=?, topval=? where code=\'%1\'").arg(pTextCode->toUnicode(pTraceLogBuf->code));
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
	sqlQuery.bindValue(12, pTraceLogBuf->topTime);
	sqlQuery.bindValue(13, pTraceLogBuf->fTopVal);
	return sqlQuery.exec() ? 0 : -1;
}

int CStockDataSqlite::GetHisKLine(char const* pStockCode, STOCK_CALC_TRACE_KLINE* pHisKLineBuf, int bufCounts)
{
	if (!m_klineDb.isOpen())
		return -1;

	QSqlQuery sqlQuery(m_klineDb);
	QString selSql("select * from (select open, close, high, low, rsi, ma10, percent, timestamp ,volume from %1 order by timestamp desc limit %2) aa order by timestamp");
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
		pHisKLineBuf->volume = sqlQuery.value(8).toInt();
		hisCounts++;
		pHisKLineBuf++;
	}

	return hisCounts;
}

int CStockDataSqlite::GetTraceLog(char const* pStockCode, STOCK_MANAGER_TRACE_LOG* pTrace)
{
	if (!m_traceLogDb.isOpen())
		return -1;

	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QSqlQuery sqlQuery(m_traceLogDb);
	QString selSql = QString("select  step, code, hightime, highval, buytime, buyval,  selltime, sellval, histime, updatetime, realtime, raisebalances, toptime, topval from trace where code =\'%1\'").arg(pTextCode->toUnicode(pStockCode));

	if (!sqlQuery.exec(selSql))
	{
		return -1;
	}

	if (sqlQuery.next())
	{
		pTrace->traceStep = sqlQuery.value(0).toInt();

		QString code = sqlQuery.value(1).toString();
		QString2Char(code, pTrace->code);

		pTrace->highTime = sqlQuery.value(2).toInt();
		pTrace->fHighVal = sqlQuery.value(3).toFloat();

		pTrace->buyTime = sqlQuery.value(4).toInt();
		pTrace->fBuyVal = sqlQuery.value(5).toFloat();

		pTrace->sellTime = sqlQuery.value(6).toInt();
		pTrace->fSellVal = sqlQuery.value(7).toFloat();

		pTrace->hisTime = sqlQuery.value(8).toInt();
		pTrace->updateTime = sqlQuery.value(9).toInt();
		pTrace->realTime = sqlQuery.value(10).toInt();
		pTrace->raiseBalanceCheckTimes = sqlQuery.value(11).toInt();
		pTrace->topTime = sqlQuery.value(12).toInt();
		pTrace->fTopVal = sqlQuery.value(13).toFloat();
	}

	return 0;
}

int CStockDataSqlite::InsertTraceRecord(STOCK_MANAGER_TRACE_LOG* pTraceLogBuf)
{
	QTextCodec* pTextCode = QTextCodec::codecForLocale();
	QSqlQuery sqlQuery(m_traceRecordDb);

	QString sqlString;
	if (pTraceLogBuf->traceStep == CALC_STOCK_TRADE_STEP_WAIT_BUYING)
		sqlString = QString("insert into traceRecord values(?, ?, ?, ?, ?, ?,?)");
	else
		sqlString = QString("update traceRecord set code=?, hightime=?, highval=?, buytime=?, buyval=?,  selltime=?, sellval=? where code=\'%1\' and buytime= %2").arg(pTextCode->toUnicode(pTraceLogBuf->code)).arg(pTraceLogBuf->buyTime);
	sqlQuery.prepare(sqlString);

	sqlQuery.bindValue(0, pTextCode->toUnicode(pTraceLogBuf->code));
	sqlQuery.bindValue(1, pTraceLogBuf->highTime);
	sqlQuery.bindValue(2, pTraceLogBuf->fHighVal);
	sqlQuery.bindValue(3, pTraceLogBuf->buyTime);
	sqlQuery.bindValue(4, pTraceLogBuf->fBuyVal);
	sqlQuery.bindValue(5, pTraceLogBuf->sellTime);
	sqlQuery.bindValue(6, pTraceLogBuf->fSellVal);

	return sqlQuery.exec() ? 0 : -1;
}